/**
  ******************************************************************************
  * @file    blsp_media_boot.h
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __BLSP_MEDIA_BOOT_H__
#define __BLSP_MEDIA_BOOT_H__

#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "blsp_bootinfo.h"

int32_t blsp_mediaboot_read(uint32_t addr, uint8_t *data, uint32_t len);
int32_t blsp_mediaboot_main(uint32_t cpu_boot_header_addr[BLSP_BOOT2_CPU_GROUP_MAX], uint8_t cpu_roll_back[BLSP_BOOT2_CPU_GROUP_MAX], uint8_t roll_back);
void blsp_boot2_show_timer(void);

#define BLSP_BOOT2_PSRAM_BASE 0x50000000

int32_t blsp_mediaboot_parse_one_group_xz(boot2_image_config *boot_img_cfg, uint32_t boot_header_addr, uint32_t img_addr, uint8_t *input, uint32_t len, uint8_t last_packet);
int32_t blsp_mediaboot_version_check(uint8_t *image_start, uint8_t group_roll_back[BLSP_BOOT2_CPU_GROUP_MAX]);
extern uint32_t g_boot2_parse_xz_image_status;
extern uint32_t g_anti_rollback_flag[3];

#define BLSP_APP_VERSION_LINK_OFFSET          (0xC00)
#define BLSP_APP_BFLB_FLAG_PRE                (0x42464c42)
#define BLSP_APP_VERF_FLAG_PRE                (0x46524556)
#ifdef CHIP_BL602
#define BLSP_APP_VERSION_MAX                  (128)
#endif
#ifdef CHIP_BL616
#define BLSP_APP_VERSION_MAX                  (128)
#endif
#ifdef CHIP_BL702
#define BLSP_APP_VERSION_MAX                  (128)
#endif
#ifdef CHIP_BL702L
#define BLSP_APP_VERSION_MAX                  (64)
#endif
#ifdef CHIP_BL606p
#define BLSP_APP_VERSION_MAX                  (128)
#endif
#ifdef CHIP_BL808
#define BLSP_APP_VERSION_MAX                  (128)
#endif

#ifndef BLSP_APP_VERSION_MAX
#error"NO CHIP DEFINE BLSP_APP_VERSION_MAX MAYBE ADD!"
#endif
#endif /* __BLSP_MEDIA_BOOT_H__ */
