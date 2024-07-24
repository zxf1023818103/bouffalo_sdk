#include "bflb_mtimer.h"
#include "board.h"
#include "stdlib.h"
#include "string.h"
#include "memtester.h"

#if defined(BL616)
#include "bl616_common.h"
#include "bl616_glb.h"
// #include "bl616_sec_eng.h"
#include "bl616_tzc_sec.h"
#include "bl616_psram.h"
#endif

#if defined(BL606P)
#include "bl606p_common.h"
#include "bl606p_glb.h"
// #include "bl606p_sec_eng.h"
#include "bl606p_psram.h"
#endif

#define UHS_PSRAM_ADDR       (0x50000000)
#define BL606P_X8_PSRAM_ADDR (0x54000000)
#define BL616_X8_PSRAM_ADDR  (0xA8000000)
#define MEMTESTER_M0
// #define MEMTESTER_D0

typedef struct _semc_test_config {
    uint32_t baseAddr;
    uint32_t testSize;
    uint32_t loopNum;
    uint32_t dramFreq;
    uint32_t enableCache;
} semc_test_config_t;

int fail_stop = 1;

// uint64_t l1c_hit = 0, l1c_miss = 0;
// uint32_t read_hit_h = 0, read_hit_l = 0, read_miss_h = 0, read_miss_l = 0;
// uint32_t write_hit_h = 0, write_hit_l = 0, write_miss_h = 0, write_miss_l = 0;
static void bflb_init_psram_gpio(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    for (uint8_t i = 0; i < 12; i++) {
        bflb_gpio_init(gpio, (41 + i), GPIO_INPUT | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0);
    }
}

static void psram_winbond_default_init(void)
{
    PSRAM_Ctrl_Cfg_Type default_psram_ctrl_cfg = {
        .vendor = PSRAM_CTRL_VENDOR_WINBOND,
        .ioMode = PSRAM_CTRL_X8_MODE,
        .size = PSRAM_SIZE_4MB,
        .dqs_delay = 0xfff0,
    };

    PSRAM_Winbond_Cfg_Type default_winbond_cfg = {
        .rst = DISABLE,
        .clockType = PSRAM_CLOCK_DIFF,
        .inputPowerDownMode = DISABLE,
        .hybridSleepMode = DISABLE,
        .linear_dis = ENABLE,
        .PASR = PSRAM_PARTIAL_REFRESH_FULL,
        .disDeepPowerDownMode = ENABLE,
        .fixedLatency = DISABLE,
        .brustLen = PSRAM_WINBOND_BURST_LENGTH_64_BYTES,
        .brustType = PSRAM_WRAPPED_BURST,
        .latency = PSRAM_WINBOND_6_CLOCKS_LATENCY,
        .driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_35_OHMS_FOR_4M_115_OHMS_FOR_8M,
    };

    PSram_Ctrl_Init(PSRAM0_ID, &default_psram_ctrl_cfg);
    // PSram_Ctrl_Winbond_Reset(PSRAM0_ID);
    PSram_Ctrl_Winbond_Write_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_CR0, &default_winbond_cfg);
}

static uint32_t board_psram_x8_init(void)
{
    uint16_t reg_read = 0;

    GLB_Set_PSRAMB_CLK_Sel(ENABLE, GLB_PSRAMB_EMI_WIFIPLL_320M, 0);

    bflb_init_psram_gpio();

    /* psram init*/
    psram_winbond_default_init();
    /* check psram work or not */
    PSram_Ctrl_Winbond_Read_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_ID0, &reg_read);
    return reg_read;
}

int main(void)
{
    board_init();
    board_psram_x8_init();
    Tzc_Sec_PSRAMB_Access_Release();

    char memsuffix = 'B';
    /* --------------- stress test --------------- */
    semc_test_config_t testConfig;
#if defined(BL808)
    testConfig.baseAddr = UHS_PSRAM_ADDR;
    testConfig.dramFreq = Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_PSRAMA) / 1000000;
#elif defined(BL606P)
    testConfig.baseAddr = BL606P_X8_PSRAM_ADDR;
    testConfig.dramFreq = Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_PSRAMB) / 1000000;
#elif defined(BL616)
    testConfig.baseAddr = BL616_X8_PSRAM_ADDR;
    testConfig.dramFreq = Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_PSRAMB) / 1000000;
    printf(" BL616 \r\n");
#endif
    testConfig.testSize = 4 * 1024 * 1024;
    testConfig.loopNum = 1000;
    testConfig.enableCache = 1;

    printf("mtimer clk:%d\r\n", CPU_Get_MTimer_Clock());
    printf("psram clk init ok!\r\n");
    printf("Now CPU size_t:%d\r\n", sizeof(size_t));
#if defined(MEMTESTER_M0)
    if (!testConfig.enableCache) {
        /* Disable D cache */
        csi_dcache_disable();
    }

    printf("\r\n########## Print out from target board ##########\r\n");
    printf("\r\n PSRAM r/w test settings:\r\n");
    printf("      Base Addr: 0x%x;\r\n", testConfig.baseAddr);
    printf("      Test Size: %d Bytes;\r\n", testConfig.testSize);
    printf("      Test Loop: %d;\r\n", testConfig.loopNum);
    printf(" PSRAM PLL Freq: %d MHz;\r\n", testConfig.dramFreq);
    printf("   Enable Cache: %d;\r\n\r\n", testConfig.enableCache);

    /* Run memory stress test: 64MByte, loop=1, page_size = 1kbyte */
    memtester_main(testConfig.baseAddr, testConfig.testSize, &memsuffix, testConfig.loopNum, (1 * 1024));
#endif

    while (1) {
#ifdef __riscv_muldiv
        int dummy;
        /* In lieu of a halt instruction, induce a long-latency stall. */
        __asm__ __volatile__("div %0, %0, zero"
                             : "=r"(dummy));
#endif
    }

    printf("memtester stress test done!\r\n");
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
