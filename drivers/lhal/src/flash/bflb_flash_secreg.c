/**
  ******************************************************************************
  * @file    bflb_flash_secreg.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
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

#include "bflb_sf_ctrl.h"
#include "bflb_sf_cfg.h"
#include "bflb_xip_sflash.h"
#include "bflb_flash_secreg_port.h"
#include "bflb_flash_secreg.h"

struct flash_params_s {
    uint32_t jedec_id;
    bflb_flash_secreg_param_t param;
};

static const ATTR_TCM_CONST_SECTION bflb_flash_secreg_param_t flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv = {
    .region_offset = 1,
    .region_count = 3,
    .region_size = 0x10,
    .secreg_size = 0x1,
    .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
    .lb_share = 0,
    .lb_offset = 11,
    .lb_write_len = 1,
    .lb_read_len = 1,
};

static const ATTR_TCM_CONST_SECTION bflb_flash_secreg_param_t flash_secreg_param_gd_le80c = {
    .region_offset = 1,
    .region_count = 3,
    .region_size = 0x10,
    .secreg_size = 0x2,
    .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
    .lb_share = 0,
    .lb_offset = 11,
    .lb_write_len = 1,
    .lb_read_len = 1,
};

static const ATTR_TCM_CONST_SECTION bflb_flash_secreg_param_t flash_secreg_param_gd_q32e_q128e = {
    .region_offset = 1,
    .region_count = 3,
    .region_size = 0x10,
    .secreg_size = 0x4,
    .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
    .lb_share = 0,
    .lb_offset = 11,
    .lb_write_len = 1,
    .lb_read_len = 1,
};

static const ATTR_TCM_CONST_SECTION bflb_flash_secreg_param_t flash_secreg_param_gd_25q256d_25s512md = {
    .region_offset = 1,
    .region_count = 3,
    .region_size = 0x10,
    .secreg_size = 0x8,
    .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
    .lb_share = 0,
    .lb_offset = 11,
    .lb_write_len = 1,
    .lb_read_len = 1,
};

static const ATTR_TCM_CONST_SECTION bflb_flash_secreg_param_t flash_secreg_param_xtx_25f256b = {
    .region_offset = 1,
    .region_count = 2,
    .region_size = 0x10,
    .secreg_size = 0x4,
    .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
    .lb_share = 0,
    .lb_offset = 11,
    .lb_write_len = 1,
    .lb_read_len = 1,
};

static const ATTR_TCM_CONST_SECTION bflb_flash_secreg_param_t flash_secreg_param_gd_25wq_80e_16e = {
    .region_offset = 0,
    .region_count = 2,
    .region_size = 0x10,
    .secreg_size = 0x4,
    .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
    .lb_share = 0,
    .lb_offset = 10,
    .lb_write_len = 1,
    .lb_read_len = 1,
};

static const ATTR_TCM_CONST_SECTION bflb_flash_secreg_param_t flash_secreg_param_gt_25q64a = {
    .region_offset = 0,
    .region_count = 3,
    .region_size = 0x8,
    .secreg_size = 0x4,
    .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
    .lb_share = 0,
    .lb_offset = 11,
    .lb_write_len = 1,
    .lb_read_len = 1,
};

static const ATTR_TCM_CONST_SECTION bflb_flash_secreg_param_t flash_secreg_param_gt_25q32a = {
    .region_offset = 0,
    .region_count = 3,
    .region_size = 0x4,
    .secreg_size = 0x4,
    .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
    .lb_share = 0,
    .lb_offset = 11,
    .lb_write_len = 1,
    .lb_read_len = 1,
};

static const ATTR_TCM_CONST_SECTION bflb_flash_secreg_param_t flash_secreg_param_fm_25q32 = {
    .region_offset = 0,
    .region_count = 4,
    .region_size = 0x10,
    .secreg_size = 0x1,
    .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
    .lb_share = 0,
    .lb_offset = 10,
    .lb_write_len = 1,
    .lb_read_len = 1,
};

static const ATTR_TCM_CONST_SECTION bflb_flash_secreg_param_t flash_secreg_param_xt_25q08b = {
    .region_offset = 0,
    .region_count = 4,
    .region_size = 0x1,
    .secreg_size = 0x1,
    .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
    .lb_share = 1,
    .lb_offset = 10,
    .lb_write_len = 1,
    .lb_read_len = 1,
};

static const ATTR_TCM_CONST_SECTION bflb_flash_secreg_param_t flash_secreg_param_xt_25f08b = {
    .region_offset = 1,
    .region_count = 3,
    .region_size = 0x1,
    .secreg_size = 0x1,
    .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
    .lb_share = 1,
    .lb_offset = 10,
    .lb_write_len = 1,
    .lb_read_len = 1,
};

static const ATTR_TCM_CONST_SECTION bflb_flash_secreg_param_t flash_secreg_param_xt_25f04d = {
    .region_offset = 0,
    .region_count = 2,
    .region_size = 0x1,
    .secreg_size = 0x1,
    .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
    .lb_share = 1,
    .lb_offset = 6,
    .lb_write_len = 1,
    .lb_read_len = 1,
};

static const ATTR_TCM_CONST_SECTION struct flash_params_s flash_params[] = {

    {
        /*!< BY25Q80BS */
        .jedec_id = 0x144068,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< BY25Q16BS */
        .jedec_id = 0x154068,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< BY25Q32AL */
        .jedec_id = 0x166068,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< BY25Q64AS */
        .jedec_id = 0x174068,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< BY25Q128AS */
        .jedec_id = 0x184068,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< FM25Q08 */
        .jedec_id = 0x1440a1,
        .param = flash_secreg_param_fm_25q32,
    },
    {
        /*!< FM25Q16A */
        .jedec_id = 0x1540a1,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< FM25Q32 */
        .jedec_id = 0x1640a1,
        .param = flash_secreg_param_fm_25q32,
    },
    {
        /*!< FM25Q64 */
        .jedec_id = 0x1740a1,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< FM25W128 */
        .jedec_id = 0x1828a1,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< GD25LE80C/GD25LQ80C */
        .jedec_id = 0x1460c8,
        .param = flash_secreg_param_gd_le80c,
    },
    {
        /*!< GD25WQ80E */
        .jedec_id = 0x1465c8,
        .param = flash_secreg_param_gd_25wq_80e_16e,
    },
    {
        /*!< GD25WQ16E */
        .jedec_id = 0x1565c8,
        .param = flash_secreg_param_gd_25wq_80e_16e,
    },
    {
        /*!< GD25VE16C */
        .jedec_id = 0x1542c8,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< GD25LQ32C/32D/32E */
        .jedec_id = 0x1660c8,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< GD25Q32C/32E */
        .jedec_id = 0x1640c8,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< GD25WQ32E */
        .jedec_id = 0x1665c8,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< GD25VE32C */
        .jedec_id = 0x1642c8,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< GD25LQ64E */
        .jedec_id = 0x1760c8,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< GD25Q64E */
        .jedec_id = 0x1740c8,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< GD25LQ128E */
        .jedec_id = 0x1860c8,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< GD25WQ128E */
        .jedec_id = 0x1865c8,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< GD25Q128E/127C */
        .jedec_id = 0x1840c8,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< GD25Q256D/GD25S512MD */
        .jedec_id = 0x1940c8,
        .param = flash_secreg_param_gd_25q256d_25s512md,
    },
    {
        /*!< FH25VQ40 */
        .jedec_id = 0x13605e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< FH25VQ20 */
        .jedec_id = 0x12605e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< FH25VQ80 */
        .jedec_id = 0x14605e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZD25Q16B */
        .jedec_id = 0x1560ba,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< P25Q80H/80L/80LE/80U */
        .jedec_id = 0x146085,
        .param = flash_secreg_param_gd_le80c,
    },
    {
        /*!< PY25Q80HB-K */
        .jedec_id = 0x142085,
        .param = flash_secreg_param_gd_le80c,
    },
    {
        /*!< P25Q16H/16L */
        .jedec_id = 0x156085,
        .param = flash_secreg_param_gd_le80c,
    },
    {
        /*!< PY25Q16HB */
        .jedec_id = 0x152085,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< PY25Q32HB */
        .jedec_id = 0x162085,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< P25Q32H/32L */
        .jedec_id = 0x166085,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< P25Q64H */
        .jedec_id = 0x176085,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< PY25Q64HA */
        .jedec_id = 0x172085,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< P25Q128H */
        .jedec_id = 0x186085,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< PY25Q128HA */
        .jedec_id = 0x182085,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< PY25Q256HB */
        .jedec_id = 0x192085,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< TH25Q-40HA */
        .jedec_id = 0x1360eb,
        .param = flash_secreg_param_gd_le80c,
    },
    {
        /*!< TH25Q-80HA */
        .jedec_id = 0x1460eb,
        .param = flash_secreg_param_gd_le80c,
    },
    {
        /*!< TH25Q-80HB */
        .jedec_id = 0x1460cd,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< TH25Q-16HB */
        .jedec_id = 0x1560eb,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< W25Q80BV/80DV/DL */
        .jedec_id = 0x1440ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q80EW */
        .jedec_id = 0x1460ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q16DV */
        .jedec_id = 0x1540ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q16FW */
        .jedec_id = 0x1560ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q16JV-IQ/JQ */
        .jedec_id = 0x1540ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q16JV-IM/JM* */
        .jedec_id = 0x1570ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q16JV-DTR */
        .jedec_id = 0x1570ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q32JV-IQ/JQ/W25Q32RV */
        .jedec_id = 0x1640ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q32JV-IM/JM* */
        .jedec_id = 0x1670ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q32JW-IQ */
        .jedec_id = 0x1660ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q32JW */
        .jedec_id = 0x1680ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q32JV-DTR */
        .jedec_id = 0x1670ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q32JW-DTR */
        .jedec_id = 0x1680ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q64JV-IQ/JQ */
        .jedec_id = 0x1740ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q64JV-IM/JM* */
        .jedec_id = 0x1770ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q64FW/W25Q64JW-IQ/JQ */
        .jedec_id = 0x1760ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q64JW */
        .jedec_id = 0x1780ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q128FV/W25Q128JV-IQ/JQ */
        .jedec_id = 0x1840ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q128FW/W25Q128JW-IQ/JQ */
        .jedec_id = 0x1860ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q128JV-IM/JM* */
        .jedec_id = 0x1870ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q128JW-IM/JM* */
        .jedec_id = 0x1880ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q256FV */
        .jedec_id = 0x1940ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q256FV */
        .jedec_id = 0x1960ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q01JV */
        .jedec_id = 0x2140ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< W25Q01JV-DTR */
        .jedec_id = 0x2170ef,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< XM25QH80B */
        .jedec_id = 0x144020,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< XM25QH16C */
        .jedec_id = 0x154020,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< XM25QH32C */
        .jedec_id = 0x164020,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< XM25LU32C */
        .jedec_id = 0x165020,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< XM25QE32C */
        .jedec_id = 0x164020,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< XM25QE32C */
        .jedec_id = 0x166020,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< XM25QW64C */
        .jedec_id = 0x174220,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< XM25QH128C */
        .jedec_id = 0x184020,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< XT25F04D */
        .jedec_id = 0x13400b,
        .param = flash_secreg_param_xt_25f04d,
    },
    {
        /*!< XT25W08F */
        .jedec_id = 0x14650b,
        .param = flash_secreg_param_gd_q32e_q128e,
    },
    {
        /*!< XT25F08B */
        .jedec_id = 0x14400b,
        .param = flash_secreg_param_xt_25f08b,
    },
    {
        /*!< XT25Q08B */
        .jedec_id = 0x14600b,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< XT25F16B */
        .jedec_id = 0x15400b,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< XT25Q16C */
        .jedec_id = 0x15600b,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< XT25Q32B */
        .jedec_id = 0x16600b,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< XT25F32B/XT25F32B-S */
        .jedec_id = 0x16400b,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< XT25F256B */
        .jedec_id = 0x19400b,
        .param = flash_secreg_param_xtx_25f256b,
    },
    {
        /*!< ZB25VQ40 */
        .jedec_id = 0x13605e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZB25VQ20 */
        .jedec_id = 0x12605e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZB25VQ80 */
        .jedec_id = 0x14605e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZB25VQ80 */
        .jedec_id = 0x14605e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZB25VQ80A */
        .jedec_id = 0x14605e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZB25WQ16A */
        .jedec_id = 0x15345e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZB25Q16B */
        .jedec_id = 0x15405e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZB25VQ16/ZB25VQ16A */
        .jedec_id = 0x15605e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZB25VQ32 */
        .jedec_id = 0x16405e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZB25VQ32 */
        .jedec_id = 0x16605e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZB25VQ64B */
        .jedec_id = 0x17405e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZB25VQ64B */
        .jedec_id = 0x17605e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< GT25Q40D */
        .jedec_id = 0x1340c4,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< GT25Q20D */
        .jedec_id = 0x1240c4,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< GT25Q10D */
        .jedec_id = 0x1140c4,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< GT25Q05D */
        .jedec_id = 0x1040c4,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< GT25Q16A/GT25Q16B */
        .jedec_id = 0x1560c4,
        .param = flash_secreg_param_xt_25q08b,
    },
    {
        /*!< GT25Q32A */
        .jedec_id = 0x1660c4,
        .param = flash_secreg_param_gt_25q32a,
    },
    {
        /*!< GT25Q64A U DTR */
        .jedec_id = 0x1760c4,
        .param = flash_secreg_param_gt_25q64a,
    },
    {
        /*!< GT25Q64AU_Adv */
        .jedec_id = 0x1860c4,
        .param = flash_secreg_param_gt_25q64a,
    },
    {
        /*!< TJ25Q08MATI */
        .jedec_id = 0x1440c7,
        .param = flash_secreg_param_gd_25wq_80e_16e,
    },
    {
        /*!< BY25Q32BS/BY25Q32ES */
        .jedec_id = 0x164068,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< GD25LE16C/GD25LQ16E */
        .jedec_id = 0x1560c8,
        .param = flash_secreg_param_gd_le80c,
    },
    {
        /*!< ZB25VQ128A/ZB25VQ128D */
        .jedec_id = 0x18405e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
    {
        /*!< ZB25VQ128A/ZB25VQ128D */
        .jedec_id = 0x18605e,
        .param = flash_secreg_param_winb_80ew_16fw_32jw_32fw_32fv,
    },
};

#define SECREG_OPLOCK_LOCKED 0x8A4F2D06
#define SECREG_OPLOCK_UNLOCK 0xB71E9C3A

static spi_flash_cfg_type *pg_flash_cfg;

static __ALIGNED(4) bflb_flash_secreg_param_t g_param = { .raw = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
static volatile uint32_t g_oplock = SECREG_OPLOCK_LOCKED;

bool ATTR_TCM_SECTION bflb_flash_secreg_callapi_before(const spi_flash_cfg_type *flash_cfg, struct callapi_content *content)
{
    int ret;
    const uint8_t group = 0;
    const uint8_t bank = 0;
    uint8_t io_mode = flash_cfg->io_mode & 0xf;

    content->flag = bflb_irq_save();
    bflb_xip_sflash_opt_enter(&content->aes_enable);
    ret = bflb_xip_sflash_state_save((void *)flash_cfg, &content->offset, group, bank);
    content->before_ret0 = (ret == 0);

    if (ret != 0) {
        bflb_sflash_set_xip_cfg((void *)flash_cfg, io_mode, 1, 0, 32, bank);
    }

    return content->before_ret0;
}

void ATTR_TCM_SECTION bflb_flash_secreg_callapi_after(const spi_flash_cfg_type *flash_cfg, struct callapi_content *content)
{
    const uint8_t group = 0;
    const uint8_t bank = 0;

    if (content->before_ret0) {
        bflb_xip_sflash_state_restore((void *)flash_cfg, content->offset, group, bank);
    }

    bflb_xip_sflash_opt_exit(content->aes_enable);
    bflb_irq_restore(content->flag);
}

/*****************************************************************************
* @brief        lock flash secreg operation (nested not supproted)
*
*
* @retval int               0:Success, -1:Already lock
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_oplock(void)
{
    if (g_oplock == SECREG_OPLOCK_LOCKED) {
        return -1;
    }

    g_oplock = SECREG_OPLOCK_LOCKED;
    return 0;
}

/*****************************************************************************
* @brief        unlock flash secreg operation (nested not supproted)
*
*
* @retval int               0:Success, -1:Already unlock
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_opunlock(void)
{
    if (g_oplock == SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    g_oplock = SECREG_OPLOCK_UNLOCK;
    return 0;
}

/*****************************************************************************
* @brief        set flash secreg param
*
* @param[in]    param          user param
*
* @retval int               0:Success, -1:Set secreg param failed
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_set_param(const bflb_flash_secreg_param_t *param)
{
    uint32_t dummy;

    if (param == NULL) {
        return -1;
    }

    g_param = *param;
    bflb_flash_get_cfg((void *)&pg_flash_cfg, &dummy);

    return 0;
}

/*****************************************************************************
* @brief        get flash secreg param
*
* @param[out]   param       pointer to save param pointer or NULL
*
* @retval int               0:Success, -1:Not found secreg param
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_get_param(const bflb_flash_secreg_param_t **param)
{
    uint32_t array_length;
    uint32_t i;
    uint32_t jid;
    uint32_t dummy;

    jid = bflb_flash_get_jedec_id();

    /*!< only search once */
    if (*((uint64_t *)&g_param) == 0xffffffffffffffff) {
        array_length = sizeof(flash_params) / sizeof(struct flash_params_s);

        for (i = 0; i < array_length; i++) {
            if (flash_params[i].jedec_id == jid) {
                break;
            }
        }

        if (i >= array_length) {
            return -1;
        }

        arch_memcpy(&g_param, &flash_params[i].param, sizeof(g_param));
        bflb_flash_get_cfg((void *)&pg_flash_cfg, &dummy);
    }

    if (param) {
        *param = &g_param;
    }

    return 0;
}

/*****************************************************************************
* @brief        get flash secreg param by jedec id
*
* @param[in]    jid         flash jedec id
* @param[out]   param       pointer to save param pointer or NULL
*
* @retval int               0:Success, -1:Not found secreg param
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_get_param_by_jedec_id(const bflb_flash_secreg_param_t **param, uint32_t jid)
{
    uint32_t array_length;
    uint32_t i;
    uint32_t dummy;

    if(jid == 0){
        return -1;
    }

    /*!< only search once */
    if (*((uint64_t *)&g_param) == 0xffffffffffffffff) {
        array_length = sizeof(flash_params) / sizeof(struct flash_params_s);

        for (i = 0; i < array_length; i++) {
            if (flash_params[i].jedec_id == jid) {
                break;
            }
        }

        if (i >= array_length) {
            return -1;
        }

        arch_memcpy(&g_param, &flash_params[i].param, sizeof(g_param));
        bflb_flash_get_cfg((void *)&pg_flash_cfg, &dummy);
    }

    if (param) {
        *param = &g_param;
    }

    return 0;
}

/*****************************************************************************
* @brief        for each security register region do callback
*
* @param[in]    cb          user callback
* @param[in]    data        user callback param
*
* @retval int               0:Success, -1:Not found secreg param
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_region_foreach(region_cb cb, void *data)
{
    int ret;
    const bflb_flash_secreg_param_t *param;
    bflb_flash_secreg_region_info_t info;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    for (uint32_t i = 0; i < param->region_count; i++) {
        if ((ret = bflb_flash_secreg_get_info_by_idx(i, &info)) != 0) {
            return ret;
        }

        if (!cb(&info, data)) {
            break;
        }
    }

    return 0;
}

/*****************************************************************************
* @brief        get security register region lock bits
*
* @param[out]   lockbits    pointer to save bitmap, some flash only one bit
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_get_lockbits(uint8_t *lockbits)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t lb;
    int ret = -1;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< get lock bit */
        ret = flash_secreg_apis[param->api_type].get_lock(
            pg_flash_cfg, param->lb_share ? 1 : param->region_count, param->lb_offset, &lb);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    *lockbits = lb;
    return 0;
}

/*****************************************************************************
* @brief        set security register region lock bits (need opunlock)
*
* @param[in]    lockbits    pointer to save bitmap, some flash only one bit
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_set_lockbits(uint8_t lockbits)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t lb_mask;
    int ret = -1;

    if (g_oplock != SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (param->lb_share) {
        if (lockbits > 1) {
            /*!< invalid lockbits */
            return -1;
        }
    } else {
        lb_mask = ~(0xff << param->region_count);
        if (lb_mask & lockbits) {
            /*!< invalid lockbits */
            return -1;
        }
    }

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< set lock bit */
        ret = flash_secreg_apis[param->api_type].set_lock(
            pg_flash_cfg, param->lb_share ? 1 : param->region_count,
            param->lb_offset, lockbits, param->lb_write_len);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    return 0;
}

/*****************************************************************************
* @brief        check security register region is OTP locked
*
* @param[in]    index       security register region index
*
* @retval int               0:Not locked, 1:OTP locked, negative value:ERROR
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_get_locked(uint8_t index)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t lb;
    int ret = -1;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< get lock bit */
        ret = flash_secreg_apis[param->api_type].get_lock(
            pg_flash_cfg, param->lb_share ? 1 : param->region_count, param->lb_offset, &lb);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    return param->lb_share ? (lb > 0) : ((lb & (1 << index)) > 0);
}

/*****************************************************************************
* @brief        lock security register region (need opunlock)
*
* @param[in]    index       security register region index
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_set_locked(uint8_t index)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t lb;
    int ret = -1;

    if (g_oplock != SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    if (param->lb_share) {
        lb = 0x1;
    } else {
        lb = 1 << index;
    }

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< set lock bit */
        ret = flash_secreg_apis[param->api_type].set_lock(
            pg_flash_cfg, param->lb_share ? 1 : param->region_count,
            param->lb_offset, lb, param->lb_write_len);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    return 0;
}

/*****************************************************************************
* @brief        get security register region info
*
* @param[in]    index       security register region index
* @param[in]    info        security register region info
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_get_info_by_idx(uint8_t index, bflb_flash_secreg_region_info_t *info)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t lb;
    int ret = -1;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< get lock bit */
        ret = flash_secreg_apis[param->api_type].get_lock(
            pg_flash_cfg, param->lb_share ? 1 : param->region_count, param->lb_offset, &lb);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    info->address = index * param->secreg_size * 256;
    info->physical_address = (index + param->region_offset) * param->region_size * 256;
    info->region_size = param->region_size * 256;
    info->secreg_size = param->secreg_size * 256;
    info->index = index;
    info->locked = param->lb_share ? (lb > 0) : ((lb & (1 << index)) > 0);
    info->lockbit_share = param->lb_share;

    return 0;
}

/*****************************************************************************
* @brief        valid security register region index and read/write len
*
* @param[in]    index       security register region index
* @param[in]    offset      read/write offset
* @param[in]    len         read/write len
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_valid_by_idx(uint8_t index, uint32_t offset, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    if ((offset + len) > param->secreg_size * 256) {
        return -1;
    }

    return 0;
}

/*****************************************************************************
* @brief        read security register region
*
* @param[in]    index       security register region index
* @param[in]    offset      read offset
* @param[in]    data        read data pointer
* @param[in]    len         read len
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_read_by_idx(uint8_t index, uint32_t offset, void *data, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint32_t address;
    int ret = -1;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    if ((offset + len) > param->secreg_size * 256) {
        return -1;
    }

    address = param->region_size * 256 * (index + param->region_offset) + offset;

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< read */
        ret = flash_secreg_apis[param->api_type].read(pg_flash_cfg, address, data, len);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    return 0;
}

/*****************************************************************************
* @brief        write security register region (need opunlock)
*
* @param[in]    index       security register region index
* @param[in]    offset      write offset
* @param[in]    data        write data pointer
* @param[in]    len         write len
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_write_by_idx(uint8_t index, uint32_t offset, const void *data, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint32_t address;
    int ret = -1;

    if (g_oplock != SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    if ((offset + len) > param->secreg_size * 256) {
        return -1;
    }

    address = param->region_size * 256 * (index + param->region_offset) + offset;

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< write */
        ret = flash_secreg_apis[param->api_type].write(pg_flash_cfg, address, data, len);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    return 0;
}

/*****************************************************************************
* @brief        erase security register region (need opunlock)
*
* @param[in]    index       security register region index
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_erase_by_idx(uint8_t index)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint32_t address;
    int ret = -1;

    if (g_oplock != SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    address = param->region_size * 256 * (index + param->region_offset);

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< erase */
        ret = flash_secreg_apis[param->api_type].erase(pg_flash_cfg, address);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    return 0;
}

/*****************************************************************************
* @brief        get security register region info
*
* @param[in]    addr        security register region address
* @param[in]    info        security register region info
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_get_info(uint32_t addr, bflb_flash_secreg_region_info_t *info)
{
    const bflb_flash_secreg_param_t *param;
    uint8_t index;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    index = addr / param->secreg_size;

    return bflb_flash_secreg_get_info_by_idx(index, info);
}

/*****************************************************************************
* @brief        valid security register region address and read/write len
*
* @param[in]    addr        security register region address
* @param[in]    len         read/write len
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_valid(uint32_t addr, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;
    uint32_t valid_len;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    valid_len = param->region_count * param->secreg_size * 256;

    if ((addr >= valid_len) || ((addr + len) > valid_len)) {
        return -1;
    }

    return 0;
}

/*****************************************************************************
* @brief        read security register region
*
* @param[in]    addr        security register region address
* @param[in]    data        read data pointer
* @param[in]    len         read len
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_read(uint32_t addr, void *data, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t index;
    uint32_t offset;
    uint32_t curlen;
    uint32_t valid_len;
    uint32_t address;
    int ret = -1;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    valid_len = param->region_count * param->secreg_size * 256;

    if ((addr >= valid_len) || ((addr + len) > valid_len)) {
        return -1;
    }

    while (len) {
        index = addr / (param->secreg_size * 256);
        offset = addr % (param->secreg_size * 256);
        address = (index + param->region_offset) * (param->region_size * 256) + offset;
        curlen = param->secreg_size * 256 - offset;

        if (curlen > len) {
            curlen = len;
        }

        if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
            /*!< read */
            ret = flash_secreg_apis[param->api_type].read(pg_flash_cfg, address, data, curlen);
        }

        bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

        if (ret) {
            return ret;
        }

        len -= curlen;
        data += curlen;
        addr += curlen;
    }

    return 0;
}

/*****************************************************************************
* @brief        write security register region (need opunlock)
*
* @param[in]    addr        security register region address
* @param[in]    data        write data pointer
* @param[in]    len         write len
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_write(uint32_t addr, const void *data, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t index;
    uint32_t offset;
    uint32_t curlen;
    uint32_t valid_len;
    uint32_t address;
    int ret = -1;

    if (g_oplock != SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    valid_len = param->region_count * param->secreg_size * 256;

    if ((addr >= valid_len) || ((addr + len) > valid_len)) {
        return -1;
    }

    while (len) {
        index = addr / (param->secreg_size * 256);
        offset = addr % (param->secreg_size * 256);
        address = (index + param->region_offset) * (param->region_size * 256) + offset;
        curlen = param->secreg_size * 256 - offset;

        if (curlen > len) {
            curlen = len;
        }

        if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
            /*!< write */
            ret = flash_secreg_apis[param->api_type].write(pg_flash_cfg, address, data, curlen);
        }

        bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

        if (ret) {
            return ret;
        }

        len -= curlen;
        data += curlen;
        addr += curlen;
    }

    return 0;
}

/*****************************************************************************
* @brief        erase security register region (need opunlock)
*
* @param[in]    addr        security register region address
* @param[in]    len         erase len
*
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_erase(uint32_t addr, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t index;
    uint32_t offset;
    uint32_t curlen;
    uint32_t valid_len;
    uint32_t address;
    int ret = -1;

    if (g_oplock != SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    valid_len = param->region_count * param->secreg_size * 256;

    if ((addr >= valid_len) || ((addr + len) > valid_len)) {
        return -1;
    }

    while (len) {
        index = addr / (param->secreg_size * 256);
        offset = addr % (param->secreg_size * 256);
        address = (index + param->region_offset) * (param->region_size * 256) + offset;
        curlen = param->secreg_size * 256 - offset;

        if (curlen > len) {
            curlen = len;
        }

        if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
            /*!< erase */
            ret = flash_secreg_apis[param->api_type].erase(pg_flash_cfg, address);
        }

        bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

        if (ret) {
            return ret;
        }

        len -= curlen;
        addr += curlen;
    }

    return 0;
}
