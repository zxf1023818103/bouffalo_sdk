#include "sdh_osal.h"
#include "sdh_host.h"

#if defined(BL616)
#include "bl616_glb.h"
#elif defined(BL808)
#include "bl808_glb.h"
#elif defined(BL606P)
#include "bl606p_glb.h"
#endif

#include "bflb_sdh.h"
#include "bflb_clock.h"

/* log */
#define DBG_TAG "SDH_HOST"
#ifdef CONFIG_BSP_SDH_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_BSP_SDH_LOG_LEVEL
#endif
#include "log.h"

/* causion: ADMA related variables must be nocache ram */
static __ALIGNED(32) ATTR_NOCACHE_RAM_SECTION struct bflb_sdh_adma2_hw_desc_s adam2_hw_desc[SDH_HOST_ADMA2_HW_DESC_NUM];
static __ALIGNED(32) ATTR_NOCACHE_RAM_SECTION uint8_t internal_buffer[SDH_HOST_INTER_BLK_BUFF_SEZE];

#if (CONFIG_BSP_SDH_OSAL_POLLING_MODE == 0)

static void sdh_irq(int irq, void *arg)
{
    struct sdh_host_s *host = (struct sdh_host_s *)arg;

    /* diable all int (normal and error) */
    bflb_sdh_sta_int_en(host->sdh_dev, 0xffffffff, false);
    /* give semb */
    sdh_osal_semb_give(host->osal_semb);
}

#endif

int sdh_host_init(struct sdh_host_s *host)
{
#if (CONFIG_BSP_SDH_OSAL_POLLING_MODE)
    /* polling mode */
    LOG_I("OSAL: Polling mode\r\n");
#elif (SDH_OSAL_TYPE == 1)
    /* Interrupt freeRTOS mode */
    LOG_I("OSAL: Interrupt mode, API: FreeRTOS\r\n");
#else
    /* Interrupt mode */
    LOG_I("OSAL: Interrupt mode, NO OS\r\n");
#endif

    host->sdh_dev = bflb_device_get_by_name("sdh");
    if (host->sdh_dev == NULL) {
        LOG_E("Get device failed\r\n");
        return -1;
    }

#if (CONFIG_BSP_SDH_OSAL_POLLING_MODE == 0)
    /* check irq num */
    if (host->sdh_dev->irq_num == 0xff) {
        LOG_E("irq num error, please use the polling mode\r\n");
        return -1;
    }

    /* Create binary semaphore */
    host->osal_semb = sdh_osal_semb_create();
    if (host->osal_semb == NULL) {
        return -1;
    }
    /* enable cpu irq */
    bflb_irq_attach(host->sdh_dev->irq_num, sdh_irq, host);
    bflb_irq_enable(host->sdh_dev->irq_num);
#endif

    /* internal buffer */
    host->internal_buff = internal_buffer;
    host->internal_buff_size = SDH_HOST_INTER_BLK_BUFF_SEZE;

    /* adma transfer hw desc init */
    host->adma2_hw_desc = adam2_hw_desc;
    host->adma2_hw_desc_num = sizeof(adam2_hw_desc) / sizeof(adam2_hw_desc[0]);

    /* reset */
    GLB_Set_SDH_CLK(ENABLE, GLB_SDH_CLK_WIFIPLL_96M, 7);
    PERIPHERAL_CLOCK_SDH_ENABLE();
#if defined(BL616)
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_EXT_SDH);
#elif defined(BL808) || defined(BL606P)
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SDH);
#endif

    host->clk_src_hz = 96000000 / 8;
    host->sdh_div = 1;

    /* init sdh */
    struct bflb_sdh_config_s cfg = {
        .dma_burst = SDH_DMA_BURST_64,
        .dma_fifo_th = SDH_DMA_FIFO_THRESHOLD_128,
        .power_vol = 0x07, /* 3.3v */
    };
    bflb_sdh_init(host->sdh_dev, &cfg);

    /* sd bus enable */
    bflb_sdh_feature_control(host->sdh_dev, SDH_CMD_SET_SD_BUS_POWER, true);
    /* sd bus clock enable */
    bflb_sdh_feature_control(host->sdh_dev, SDH_CMD_SET_BUS_CLK_EN, true);

    return 0;
}

int sdh_host_deinit(struct sdh_host_s *host)
{
#if (CONFIG_BSP_SDH_OSAL_POLLING_MODE == 0)
    if (host->osal_semb) {
        sdh_osal_semb_delete(host->osal_semb);
    }
#endif

    /* disable sdh */
#if defined(BL616)
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_EXT_SDH);
#elif defined(BL606P) || defined(BL808)
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SDH);
#endif

    return 0;
}

int sdh_host_transfer(struct sdh_host_s *host, struct sdh_host_transfer_s *transfer)
{
    int ret;

    LOG_D("host_transfer CMD%d, agr 0x%08X, data:%s\r\n",
          transfer->cmd_cfg->index, transfer->cmd_cfg->argument, transfer->data_cfg ? "ture" : "false");

    if (transfer->data_cfg && transfer->data_cfg->adma2_hw_desc == NULL) {
        transfer->data_cfg->adma2_hw_desc = host->adma2_hw_desc;
        transfer->data_cfg->adma2_hw_desc_cnt = host->adma2_hw_desc_num;
    }

    /* clr comp sta */
    bflb_sdh_sta_clr(host->sdh_dev, SDH_NORMAL_STA_CMD_COMP | SDH_NORMAL_STA_TRAN_COMP);
    /* clr all error sta */
    bflb_sdh_sta_clr(host->sdh_dev, SDH_ERROR_ALL_MASK);

    /*  */
    ret = bflb_sdh_tranfer_start(host->sdh_dev, transfer->cmd_cfg, transfer->data_cfg);

    if (ret < 0) {
        LOG_W("CMD%d send failed, agr 0x%08X\r\n", transfer->cmd_cfg->index, transfer->cmd_cfg->argument);
    }

    return ret;
}

int sdh_host_wait_done(struct sdh_host_s *host, struct sdh_host_transfer_s *transfer)
{
    int ret = 0;
    uint32_t sta;

    /* wait CMD done */
#if (CONFIG_BSP_SDH_OSAL_POLLING_MODE)
    /* Polling mode */
    do {
        sta = bflb_sdh_sta_get(host->sdh_dev);
    } while ((sta & (SDH_NORMAL_STA_CMD_COMP | SDH_NORMAL_STA_ERROR)) == 0);
#else
    /* Interrupt mode */
    bflb_sdh_sta_int_en(host->sdh_dev, SDH_NORMAL_STA_CMD_COMP | SDH_ERROR_ALL_MASK, true);
    sdh_osal_semb_take(host->osal_semb, 0);

    sta = bflb_sdh_sta_get(host->sdh_dev);
#endif

    /* get resp */
    if (sta & SDH_NORMAL_STA_CMD_COMP) {
        bflb_sdh_sta_clr(host->sdh_dev, SDH_NORMAL_STA_CMD_COMP);
        bflb_sdh_get_resp(host->sdh_dev, transfer->cmd_cfg);
    } else {
        ret = -1;
        goto wait_exit;
    }

    if (transfer->data_cfg == NULL &&
        transfer->cmd_cfg->resp_type != SDH_RESP_TPYE_R1b &&
        transfer->cmd_cfg->resp_type != SDH_RESP_TPYE_R5b) {
        /* no data and busy, end */
        ret = 0;
        goto wait_exit;
    }

    /* wait DATA/busy done */
#if (CONFIG_BSP_SDH_OSAL_POLLING_MODE)
    /* Polling mode */
    do {
        sta = bflb_sdh_sta_get(host->sdh_dev);
    } while ((sta & (SDH_NORMAL_STA_TRAN_COMP | SDH_NORMAL_STA_ERROR)) == 0);
#else
    /* Interrupt mode */
    bflb_sdh_sta_int_en(host->sdh_dev, SDH_NORMAL_STA_TRAN_COMP | SDH_ERROR_ALL_MASK, true);
    sdh_osal_semb_take(host->osal_semb, 0);

    sta = bflb_sdh_sta_get(host->sdh_dev);
#endif

    if (sta & SDH_NORMAL_STA_TRAN_COMP) {
        bflb_sdh_sta_clr(host->sdh_dev, SDH_NORMAL_STA_CMD_COMP);
        ret = 0;
        goto wait_exit;
    } else {
        ret = -2;
        goto wait_exit;
    }

wait_exit:
    if (ret < 0) {
        LOG_W("CMD%d wait failed, agr: 0x%08X, sta: 0x%08X\r\n", transfer->cmd_cfg->index, transfer->cmd_cfg->argument, sta);
    } else {
        LOG_D("transfer CMD%d done, \r\n", transfer->cmd_cfg->index);
    }

    return ret;
}

int sdh_set_bus_width(struct sdh_host_s *host, uint8_t bus_width)
{
    int ret;

    if (bus_width != 1 && bus_width != 4 && bus_width != 8) {
        return -1;
    }

    ret = bflb_sdh_feature_control(host->sdh_dev, SDH_CMD_SET_BUS_WIDTH, bus_width);
    return ret;
}

int sdh_set_bus_clock(struct sdh_host_s *host, uint32_t freq, uint32_t *actual_freq)
{
    int ret;
    uint32_t actual_freq_hz;

    bflb_sdh_feature_control(host->sdh_dev, SDH_CMD_SET_BUS_CLK_EN, false);

#if defined(BL616) || defined(BL808) || defined(BL606P)
    uint32_t div = 96000000 / freq;
    if (96000000 / div > freq) {
        div += 1;
    }
    if (div > 8) {
        div = 8;
    } else if (div == 0) {
        div = 1;
    }

    actual_freq_hz = 96000000 / div;

    if (actual_freq) {
        *actual_freq = actual_freq_hz;
    }

    GLB_Set_SDH_CLK(ENABLE, GLB_SDH_CLK_WIFIPLL_96M, div - 1);

    host->clk_src_hz = actual_freq_hz;
    host->sdh_div = 1;

    bflb_sdh_feature_control(host->sdh_dev, SDH_CMD_SET_BUS_CLK_DIV, 0);

#endif

    LOG_D("set bus clk, want %dhz, actual %dhz\r\n", freq, actual_freq_hz);

    ret = bflb_sdh_feature_control(host->sdh_dev, SDH_CMD_SET_BUS_CLK_EN, true);

    return ret;
}

int sdh_send_active_clk(struct sdh_host_s *host)
{
    bflb_sdh_feature_control(host->sdh_dev, SDH_CMD_ACTIVE_CLK_OUT, 80);
    return 0;
}

int sdh_set_data_timeout_ms(struct sdh_host_s *host, uint16_t timeout_ms, uint32_t *actual_ms)
{
    int ret;
    uint32_t tmclk;
    uint32_t timeout_cnt;
    uint32_t cnt_vaule;
    uint32_t actual_timeout_ms;
#if defined(BL616)
    tmclk = host->clk_src_hz;
#else
    tmclk = host->clk_src_hz;
#endif
    timeout_cnt = tmclk / 1000 * timeout_ms;

    for (cnt_vaule = 1; cnt_vaule < 32; cnt_vaule++) {
        if ((timeout_cnt >> cnt_vaule) == 0) {
            break;
        }
    }
    cnt_vaule -= 1;

    if (cnt_vaule >= 27) {
        cnt_vaule = 27;
    } else if (cnt_vaule < 13) {
        cnt_vaule = 13;
    } else if (timeout_cnt & (~(1 << cnt_vaule))) {
        cnt_vaule += 1;
    }

    actual_timeout_ms = (int64_t)(1 << cnt_vaule) * 1000 / tmclk;

    if (actual_ms) {
        *actual_ms = actual_timeout_ms;
    }

    LOG_D("set data timeout, want %dms, actual %dms\r\n", timeout_ms, actual_timeout_ms);

    cnt_vaule -= 13;

    /*!< Data timeout_s = (2^(cnt_vaule+13)) / tmclk_freq */
    ret = bflb_sdh_feature_control(host->sdh_dev, SDH_CMD_SET_DATA_TIMEOUT_CNT_VAL, cnt_vaule);

    return ret;
}

int sdh_get_signal_status(struct sdh_host_s *host)
{
    return 0;
}

int sdh_force_clock(struct sdh_host_s *host, bool en)
{
    bflb_sdh_feature_control(host->sdh_dev, SDH_CMD_FORCE_CLK_OUTPUT, en);
    return 0;
}

int sdh_switch_voltage(struct sdh_host_s *host, uint32_t voltage)
{
    return 0;
}
