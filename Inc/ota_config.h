/*
 * ota_config.h
 *
 *  Created on: Jan 10, 2026
 *      Author: Sriram
 */


#ifndef __BL_OTA_CONFIG_H
#define __BL_OTA_CONFIG_H

#include "flash_layout.h"   /* need APP_CODE_ADDR/FLASH_END_ADDR */

/* ---------- OTA limits ---------- */
/* Maximum firmware size per partition (118 kB minus OTA header in partition A) */
#define OTA_MAX_SIZE             (PART_A_SIZE - OTA_HDR_SIZE)   /* 118k - 2k = ~116k usable */
#define PARTITION_SIZE           PART_A_SIZE                     /* 118 kB per partition */

#define OTA_CHUNK_SIZE           256

/* ---------- Timeouts (ms) ---------- */
#define OTA_RX_TIMEOUT_MS    3000
#define OTA_CHUNK_TIMEOUT    5000

/* ---------- Retry ---------- */
#define OTA_MAX_RETRY        3

#endif


