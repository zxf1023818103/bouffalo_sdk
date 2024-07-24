#include "usbd_core.h"
#include "usbd_cdc_ecm.h"
#include "usbd_cdc.h"
#include "bflb_emac.h"
#include "ethernet_phy.h"
#include "board.h"
#include "ring_buffer.h"

/*!< endpoint address */
#define CDC_IN_EP          0x81
#define CDC_OUT_EP         0x02
#define CDC_INT_EP         0x86

#define CDC_IN_EP2         0x83
#define CDC_OUT_EP2        0x04
#define CDC_INT_EP2        0x85

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE    (9 + CDC_ECM_DESCRIPTOR_LEN + CDC_ACM_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

#define CDC_ECM_ETH_STATISTICS_BITMAP 0x00000000

/* str idx = 4 is for mac address: aa:bb:cc:dd:ee:ff*/
#define CDC_ECM_MAC_STRING_INDEX      4

/*!< global descriptor */
static const uint8_t cdc_ecm_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x04, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ECM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, CDC_ECM_ETH_STATISTICS_BITMAP, CONFIG_CDC_ECM_ETH_MAX_SEGSZE, 0, 0, CDC_ECM_MAC_STRING_INDEX),
    CDC_ACM_DESCRIPTOR_INIT(0x02, CDC_INT_EP2, CDC_OUT_EP2, CDC_IN_EP2, CDC_MAX_MPS, 0x02),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x2E,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'C', 0x00,                  /* wcChar10 */
    'D', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'E', 0x00,                  /* wcChar14 */
    'C', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    ' ', 0x00,                  /* wcChar17 */
    'D', 0x00,                  /* wcChar18 */
    'E', 0x00,                  /* wcChar19 */
    'M', 0x00,                  /* wcChar20 */
    'O', 0x00,                  /* wcChar21 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
    ///////////////////////////////////////
    /// string4 descriptor
    ///////////////////////////////////////
    0x1A,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'a', 0x00,                  /* wcChar0 */
    'a', 0x00,                  /* wcChar1 */
    'b', 0x00,                  /* wcChar2 */
    'b', 0x00,                  /* wcChar3 */
    'c', 0x00,                  /* wcChar4 */
    'c', 0x00,                  /* wcChar5 */
    'd', 0x00,                  /* wcChar6 */
    'd', 0x00,                  /* wcChar7 */
    'e', 0x00,                  /* wcChar8 */
    'e', 0x00,                  /* wcChar9 */
    'f', 0x00,                  /* wcChar10 */
    'f', 0x00,                  /* wcChar11 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

struct bflb_device_s *emac0;
volatile uint8_t g_emac_tx_idle_flag = 1;
volatile uint8_t g_emac_rx_idle_flag = 1;

#define ETH_RXBUFNB 110
#define ETH_TXBUFNB 10

ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(4))) uint8_t ethRxBuff[ETH_RXBUFNB][ETH_RX_BUFFER_SIZE]; /* Ethernet Receive Buffers */
ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(4))) uint8_t ethTxBuff[ETH_TXBUFNB][ETH_TX_BUFFER_SIZE]; /* Ethernet Transmit Buffers */

Ring_Buffer_Type emac_rx_rb;

struct eth_buf {
    uint8_t *pbuf;
    uint32_t len;
    uint32_t idx;
};

struct eth_buf emac_rx_rb_buffer[ETH_RXBUFNB];
ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(4))) uint8_t usb_tx_temp_buffer[ETH_RX_BUFFER_SIZE];

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[2048];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[2048];

volatile bool ep_tx_busy_flag = false;

void usbd_event_handler(uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            g_emac_tx_idle_flag = 1;
            g_emac_rx_idle_flag = 1;
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            break;
        case USBD_EVENT_SUSPEND:
            break;
        case USBD_EVENT_CONFIGURED:
            /* EMAC transmit start */
            usbd_cdc_ecm_send_connect_status(true);
            //printf("EMAC start\r\n");
            bflb_emac_start(emac0);
            bflb_irq_enable(emac0->irq_num);
            /* setup first out ep read transfer */
            usbd_ep_start_read(CDC_OUT_EP2, read_buffer, 512);
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

void usbd_cdc_acm_bulk_out(uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual out len:%d\r\n", nbytes);
    // for (int i = 0; i < 100; i++) {
    //     printf("%02x ", read_buffer[i]);
    // }
    // printf("\r\n");
    /* setup next out ep read transfer */
    usbd_ep_start_read(CDC_OUT_EP2, read_buffer, 2048);
}

void usbd_cdc_acm_bulk_in(uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual in len:%d\r\n", nbytes);

    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(CDC_IN_EP2, NULL, 0);
    } else {
        ep_tx_busy_flag = false;
    }
}

/*!< endpoint call back */
struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP2,
    .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP2,
    .ep_cb = usbd_cdc_acm_bulk_in
};

void usbd_cdc_ecm_packet_recv_done(uint8_t *buf, uint32_t len)
{
    //printf("rxlen:%d\r\n", len);
    if (0 != bflb_emac_bd_tx_enqueue(EMAC_NORMAL_PACKET, len, buf)) {
        printf("emac_bd_tx_enqueue error!\r\n");
        g_emac_rx_idle_flag = 1;
    }
}

void usbd_cdc_ecm_packet_send_done(void)
{
    g_emac_tx_idle_flag = 1;
}

void emac_isr(int irq, void *arg)
{
    uint32_t int_sts_val;
    uint32_t index = 0;
    int_sts_val = bflb_emac_get_int_status(emac0);
    // printf("emac int:%08lx\r\n", int_sts_val);
    if (int_sts_val & EMAC_INT_STS_TX_DONE) {
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_TX);
        bflb_emac_bd_tx_dequeue(index);
        bflb_emac_int_clear(emac0, EMAC_INT_STS_TX_DONE);
        usbd_cdc_ecm_start_read_next_packet();
    }

    if (int_sts_val & EMAC_INT_STS_TX_ERROR) {
        bflb_emac_int_clear(emac0, EMAC_INT_STS_TX_ERROR);
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_TX);
        bflb_emac_bd_tx_on_err(index);

        printf("EMAC tx error !!!\r\n");
        usbd_cdc_ecm_start_read_next_packet();
    }

    if (int_sts_val & EMAC_INT_STS_RX_DONE) {
        bflb_emac_int_clear(emac0, EMAC_INT_STS_RX_DONE);
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_RX);
        bflb_emac_bd_rx_enqueue(index);

        struct eth_buf buf;

        while (1) {
            buf.idx = bflb_emac_bd_rx_dequeue(-1, &buf.len, &buf.pbuf);

            if (buf.len) {
                if (Ring_Buffer_Get_Status(&emac_rx_rb) == RING_BUFFER_FULL) {
                    printf("data full, drop\r\n");
                    continue;
                }

                Ring_Buffer_Write(&emac_rx_rb, (uint8_t *)&buf, sizeof(struct eth_buf));
            } else {
                break;
            }
        }
    }

    if (int_sts_val & EMAC_INT_STS_RX_ERROR) {
        bflb_emac_int_clear(emac0, EMAC_INT_STS_RX_ERROR);
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_RX);
        bflb_emac_bd_rx_on_err(index);

        printf("EMAC rx error!!!\r\n");
    }

    if (int_sts_val & EMAC_INT_STS_RX_BUSY) {
        printf("emac rx busy\r\n");
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_RX);
        printf("index:%d\r\n", index);
        bflb_emac_int_clear(emac0, EMAC_INT_STS_RX_BUSY);
    }
}

void emac_init()
{
    struct bflb_emac_config_s emac_cfg = {
        .inside_clk = EMAC_CLK_USE_EXTERNAL,
        .mii_clk_div = 49,
        .min_frame_len = 64,
        .max_frame_len = ETH_MAX_PACKET_SIZE,
        .mac_addr[0] = 0x18,
        .mac_addr[1] = 0xB9,
        .mac_addr[2] = 0x05,
        .mac_addr[3] = 0x12,
        .mac_addr[4] = 0x34,
        .mac_addr[5] = 0x56,
    };

    struct bflb_emac_phy_cfg_s phy_cfg = {
        .auto_negotiation = 1, /*!< Speed and mode auto negotiation */
        .full_duplex = 1,      /*!< Duplex mode */
        .speed = 100,            /*!< Speed mode */
    #ifdef PHY_8720
        .phy_address = 0x01,  /*!< PHY address */
        .phy_id = 0x7c0f0, /*!< PHY OUI, masked */
    #else
    #ifdef PHY_8201F
        .phy_address = 0, /*!< PHY address */
        .phy_id = 0x120,  /*!< PHY OUI, masked */
    #endif
    #ifdef PHY_11X1
        .phy_address = 0, /*!< PHY address */
        .phy_id = 0xc4020,  /*!< PHY OUI, masked */
    #endif
    #endif
        .phy_state = PHY_STATE_DOWN,
    };

    /* emac gpio init */
    board_emac_gpio_init();

    /* emac & BD init and interrupt attach */
    emac0 = bflb_device_get_by_name("emac0");
    bflb_emac_init(emac0, &emac_cfg);
    bflb_emac_bd_init(emac0, (uint8_t *)ethTxBuff, ETH_TXBUFNB, (uint8_t *)ethRxBuff, ETH_RXBUFNB);
    bflb_irq_attach(emac0->irq_num, emac_isr, emac0);
    bflb_emac_int_clear(emac0, EMAC_INT_EN_ALL);
    bflb_emac_int_enable(emac0, EMAC_INT_EN_ALL, 1);

    /* phy module init */
    ethernet_phy_init(emac0, &phy_cfg);
    printf("ETH PHY init ok!\r\n");
    ethernet_phy_status_get();
    if (PHY_STATE_UP == phy_cfg.phy_state) {
        printf("PHY[%lx] @%d ready on %dMbps, %s duplex\n\r", phy_cfg.phy_id, phy_cfg.phy_address, phy_cfg.speed, phy_cfg.full_duplex ? "full" : "half");
    } else {
        printf("PHY Init fail\n\r");
        while (1) {
            bflb_mtimer_delay_ms(10);
        }
    }
}

struct usbd_interface intf0;
struct usbd_interface intf1;
struct usbd_interface intf2;
struct usbd_interface intf3;

/* ecm only supports in linux, and you should input the following command
 * 
 * sudo ifconfig enxaabbccddeeff up
 * sudo dhcpclient enxaabbccddeeff
*/
void cdc_ecm_init(void)
{
    emac_init();
    Ring_Buffer_Init(&emac_rx_rb, (uint8_t *)emac_rx_rb_buffer, sizeof(emac_rx_rb_buffer), NULL, NULL);

    usbd_desc_register(cdc_ecm_descriptor);
    usbd_add_interface(usbd_cdc_ecm_init_intf(&intf0, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP));
    usbd_add_interface(usbd_cdc_ecm_init_intf(&intf1, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf2));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf3));
    usbd_add_endpoint(&cdc_out_ep);
    usbd_add_endpoint(&cdc_in_ep);
    usbd_initialize();
}

void emac_poll()
{
    struct eth_buf buf;
    uint32_t len;

    if (g_emac_tx_idle_flag == 1) {
        len = Ring_Buffer_Read(&emac_rx_rb, (uint8_t *)&buf, sizeof(struct eth_buf));
        if (len) {
            g_emac_tx_idle_flag = 0;
            usbd_ep_start_write(CDC_IN_EP, buf.pbuf, buf.len);
        } else {
        }
    }
}


volatile uint8_t dtr_enable = 0;

void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr)
{
    if (dtr) {
        dtr_enable = 1;
    } else {
        dtr_enable = 0;
    }
}

void cdc_acm_data_send_with_dtr_test(void)
{
    if (dtr_enable) {
        ep_tx_busy_flag = true;
        usbd_ep_start_write(CDC_IN_EP2, write_buffer, 2048);
        while (ep_tx_busy_flag) {
        }
    }
}