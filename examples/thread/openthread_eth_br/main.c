#include <bflb_irq.h>
#include <bflb_uart.h>

#include <bl616_glb.h>
#include <rfparam_adapter.h>
#include <bflb_wdg.h>
#include <bflb_emac.h>
#include <ethernet_phy.h>
#include <bflb_mtd.h>
#if defined (CONFIG_EASYFLASH4)
#include <easyflash.h>
#endif

#include <log.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <mem.h>

#include <lwip/tcpip.h>
#include <lwip/dhcp6.h>
#include <ethernetif.h>

#include <openthread/thread.h>
#include <openthread/dataset_ftd.h>
#include <openthread/cli.h>
#include <openthread_port.h>
#include <openthread_br.h>

#include "board.h"

#define THREAD_CHANNEL      15
#define THREAD_PANID        0x6677
#define THREAD_EXTPANID     {0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22}
#define THREAD_NETWORK_KEY  {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}

static struct bflb_device_s *uart0;

/* global network interface struct define */
static struct netif gnetif;

/* For emac tx and rx buffer,we put here to make controlling it's size easy */
#define ETH_RXBUFNB 5
#define ETH_TXBUFNB 5
ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(4))) uint8_t ethRxBuff[ETH_RXBUFNB][ETH_RX_BUFFER_SIZE]; /* Ethernet Receive Buffers */
ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(4))) uint8_t ethTxBuff[ETH_TXBUFNB][ETH_TX_BUFFER_SIZE]; /* Ethernet Transmit Buffers */

extern void __libc_init_array(void);
extern void shell_init_with_task(struct bflb_device_s *shell);

struct netif * otbr_getInfraNetif(void) 
{
    return &gnetif;
}

static void netif_status_callback(struct netif *netif)
{
    typedef enum {
        ADDRESS_SHOW_IDX_IPV4 = 0,
        ADDRESS_SHOW_IDX_IPV6 = 1,
    } address_shown_t;
    static address_shown_t address_show_msk = 0;
    bool isIPv6AddressAssigend = false;

    if (netif->flags & NETIF_FLAG_UP) {
        if(!ip4_addr_isany(netif_ip4_addr(netif)) && (0 == (address_show_msk & (1 << ADDRESS_SHOW_IDX_IPV4)))) {
            printf("IPv4 address: %s\r\n", ip4addr_ntoa(netif_ip4_addr(netif)));
            printf("IPv4 mask: %s\r\n", ip4addr_ntoa(netif_ip4_netmask(netif)));
            printf("Gateway address: %s\r\n", ip4addr_ntoa(netif_ip4_gw(netif)));
            address_show_msk |= (1 << ADDRESS_SHOW_IDX_IPV4);
        }

        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i ++ ) {
            if (!ip6_addr_isany(netif_ip6_addr(netif, i))
                && ip6_addr_ispreferred(netif_ip6_addr_state(netif, i))) {

                const ip6_addr_t* ip6addr = netif_ip6_addr(netif, i);
                if (ip6_addr_isany(ip6addr)) {
                    continue;
                }

                if(ip6_addr_islinklocal(ip6addr)){
                    if (0 == (address_show_msk & (1 << (i + ADDRESS_SHOW_IDX_IPV6)))) {
                        printf("IPv6 linklocal address: %s\r\n", ip6addr_ntoa(ip6addr));
                    }
                }
                else{
                    if (0 == (address_show_msk & (1 << (i + ADDRESS_SHOW_IDX_IPV6)))) {
                        printf("IPv6 address %d: %s\r\n", i, ip6addr_ntoa(ip6addr));
                    }
                    isIPv6AddressAssigend = true;
                }
                address_show_msk |= (1 << (i + ADDRESS_SHOW_IDX_IPV6));
            }
        }

        if (isIPv6AddressAssigend) {
            otbr_instance_routing_init();
        }
    }
    else {
        address_show_msk = 0;
        printf("Interface is down status.\r\n");
    }
}

/**
  * @brief  Setup the network interface
  * @param  None
  * @retval None
  */
static void netif_config(void * arg)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    memset(&ipaddr, 0, sizeof(ip_addr_t));
    memset(&netmask, 0, sizeof(ip_addr_t));
    memset(&gw, 0, sizeof(ip_addr_t));

    /* add the network interface */
    netif_add(&gnetif, &ipaddr.u_addr.ip4, &netmask.u_addr.ip4, &gw.u_addr.ip4, NULL, &ethernetif_init, &tcpip_input);

    /*  Registers the default network interface */
    netif_set_default(&gnetif);
    ethernet_link_status_updated(&gnetif);

    netif_create_ip6_linklocal_address(&gnetif, 1);
    gnetif.ip6_autoconfig_enabled = 1;
    dhcp6_enable_stateless(&gnetif);

#if LWIP_NETIF_LINK_CALLBACK
    netif_set_link_callback(&gnetif, ethernet_link_status_updated);
#endif

    netif_set_status_callback(&gnetif, netif_status_callback);
}

void emac_init_txrx_buffer(struct bflb_device_s *emac)
{
    bflb_emac_bd_init(emac, (uint8_t *)ethTxBuff, ETH_TXBUFNB, (uint8_t *)ethRxBuff, ETH_RXBUFNB);
}

void otr_start_default(void) 
{
    otOperationalDataset ds;
    uint8_t default_network_key[] = THREAD_NETWORK_KEY;
    uint8_t default_extend_panid[] = THREAD_EXTPANID;

    if (!otDatasetIsCommissioned(otrGetInstance())) {

        if (OT_ERROR_NONE != otDatasetCreateNewNetwork(otrGetInstance(), &ds)) {
            printf("Failed to create dataset for Thread Network\r\n");
        }

        memcpy(&ds.mNetworkKey, default_network_key, sizeof(default_network_key));
        strncpy(ds.mNetworkName.m8, "OTBR-BL702", sizeof(ds.mNetworkName.m8));
        memcpy(&ds.mExtendedPanId, default_extend_panid, sizeof(default_extend_panid));
        ds.mChannel = THREAD_CHANNEL;
        ds.mPanId = THREAD_PANID;
        
        if (OT_ERROR_NONE != otDatasetSetActive(otrGetInstance(), &ds)) {
            printf("Failed to set active dataset\r\n");
        }
    }

    otIp6SetEnabled(otrGetInstance(), true);
    otThreadSetEnabled(otrGetInstance(), true);
}

void otrInitUser(otInstance * instance)
{
    otAppCliInit((otInstance * )instance);
    otr_start_default();
    otbr_netif_init();
    otbr_nat64_init(OPENTHREAD_OTBR_CONFIG_NAT64_CIDR);
}

int main(void)
{
    otRadio_opt_t opt;

    board_init();
    /* emac gpio init */
    board_emac_gpio_init();

    bflb_mtd_init();
#if defined (CONFIG_EASYFLASH4)
    easyflash_init();
#endif

#if defined(BL616)
    /* Init rf */
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }
#endif

    __libc_init_array();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    memset(otbr_getThreadNetif(), 0, sizeof(struct netif));

    tcpip_init(netif_config, NULL);

    opt.byte = 0;

    opt.bf.isCoexEnable = false;
    opt.bf.isFtd = true;
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    opt.bf.isLinkMetricEnable = true;
#endif
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    opt.bf.isCSLReceiverEnable = true;
#endif
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    opt.bf.isTimeSyncEnable = true;
#endif

    otrStart(opt);

    puts("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();

    while (1) {
    }
}
