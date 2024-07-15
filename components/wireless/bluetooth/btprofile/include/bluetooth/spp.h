/*
 * SPP Header File
 *
 * Copyright (c) 2021-2022 Bouffalolab Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef _INCLUDE_BLUETOOTH_SPP_H_
#define _INCLUDE_BLUETOOTH_SPP_H_

#ifdef __cplusplus
extern "C" {
#endif


int bt_spp_init(void);
int bt_spp_connect(struct bt_conn *conn);
int bt_spp_disconnect(void);
int bt_spp_send(uint8_t *buf_data);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_BLUETOOTH_AVCTP_H_ */

