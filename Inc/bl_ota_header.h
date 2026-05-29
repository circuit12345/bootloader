/*
 * bl_ota_header.h
 *
 *  Created on: Jan 11, 2026
 *      Author: Sriram
 */

#ifndef INC_BL_OTA_HEADER_H_
#define INC_BL_OTA_HEADER_H_

#include <stdint.h>
#include "flash_layout.h"

typedef struct
{
    uint32_t magic;
    uint32_t image_size;
    uint32_t image_crc;
    uint32_t state;                 /* OTA_STATE_EMPTY, UPDATING, or VALID */
    uint32_t bootloader_version;
    uint16_t partition_select;      /* PARTITION_A=0 or PARTITION_B=1 (which is running) */
    uint32_t partition_b_crc;       /* CRC of partition B for verification */
    uint32_t reserved;              /* For future use */

} ota_header_t;

#define OTA_HDR   ((volatile ota_header_t*)OTA_HDR_ADDR)


#endif /* INC_BL_OTA_HEADER_H_ */
