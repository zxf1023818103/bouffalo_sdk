#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_common.h"

#define COMPILE_TIME __DATE__ " " __TIME__
char ver_name[5] __attribute__ ((section(".verinfo"))) = "app";
char git_commit[41] __attribute__ ((section(".verinfo"))) = GIT_COMMIT;
char time_info[30] __attribute__ ((section(".verinfo"))) = COMPILE_TIME;

const blverinf_t app_ver __attribute__ ((section(".blverinf"))) = {
    .anti_rollback = 0,
    .x = 0,
    .y = 0,
    .z = 0,
    .name = (uint32_t)ver_name,
    .build_time = (uint32_t)time_info,
    .commit_id = (uint32_t)git_commit,
    .rsvd0 = 0,
    .rsvd1 = 0,
};

int main(void)
{
    uint8_t version = 0xFF;

    board_init();

    printf("anti_rollback case:\r\n");

    if(0 != hal_get_app_version_from_efuse(&version)){
        printf("error! can't read app version\r\n");
        while(1){
        }
    }else{
        printf("app version in efuse is: %d\r\n", version);
    }

    if(app_ver.anti_rollback < version){
        printf("app version in application is: %d, less than app version in efuse, the application should not run up\r\n", app_ver.anti_rollback);
    }else{
        printf("app version in application is: %d, not less than app version in efuse, the application should run up\r\n", app_ver.anti_rollback);
    }

    /* change app version in efuse to 1 */
    hal_set_app_version_to_efuse(1);

    if(0 != hal_get_app_version_from_efuse(&version)){
        printf("error! can't read app version\r\n");
        while(1){
        }
    }else{
        printf("app version in efuse is: %d\r\n", version);
    }

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
