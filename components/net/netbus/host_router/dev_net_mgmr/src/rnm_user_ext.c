#include <sdiowifi_securec_wrap.h>
#include <rnm_server.h>
#include <stdio.h>
#include <string.h>
#include <sdio_port.h>

void rnms_handle_user_ext(rnms_t *rnm, rnm_msg_t *cmd)
{
    uint8_t msg_buf[256];
    rnm_user_ext_msg_t *rsp = (rnm_user_ext_msg_t *)msg_buf;
    rnm_user_ext_msg_t *msg = cmd->data;
    const char rsp_str[] = "response from device";

    // TODO Add your handler here
    HR_LOGI("Recv user ext\r\n");
    if (!msg) {
        return;
    }
    HR_LOGI("msg->payload:%s\r\n", msg->payload);

    MEMSET_SAFE(rsp,  sizeof(*rsp),  0,  sizeof(*rsp));
    // field cmd, session_id should be set
    // flag should be RNM_MSG_FLAG_ACK
    rnms_msg_fill_common(rnm, rsp, cmd);
    MEMCPY_SAFE(rsp->payload,  sizeof(rsp_str),  rsp_str,  sizeof(rsp_str));

    rnms_msg_output(rnm, rsp, sizeof(*rsp) + sizeof(rsp_str));
}

