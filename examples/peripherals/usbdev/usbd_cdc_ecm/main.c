#include "usbd_core.h"
#include "bflb_mtimer.h"
#include "board.h"

extern void cdc_ecm_init(void);
extern void emac_poll(void);

int main(void)
{
    board_init();

    cdc_ecm_init();
    while (1) {
        emac_poll();
    }
}
