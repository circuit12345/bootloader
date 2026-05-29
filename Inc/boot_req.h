/*
 * boot_req.h
 *
 *  Created on: Jan 9, 2026
 *      Author: Sriram
 */

#ifndef BOOT_REQ_H
#define BOOT_REQ_H

#include <stdint.h>

/* Magic value */
#define OTA_REQUEST_MAGIC  0xA5A55A5A

/* Fixed RAM address for OTA flag (must be safe RAM) */
#define OTA_FLAG_ADDR      0x20000270U

/* Access macro */
#define ota_request_flag   (*(volatile uint32_t *)OTA_FLAG_ADDR)

/* API for application */
void App_RequestOTA(void);

void OTA_RequestLoop(void);

#endif /* BOOT_REQ_H */

