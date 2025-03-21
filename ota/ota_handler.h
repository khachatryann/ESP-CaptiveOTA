/*
 * ota_handler.h
 *
 *  Created on: Mar 20, 2025
 *      Author: Aram
 */

#ifndef OTA_OTA_HANDLER_H_
#define OTA_OTA_HANDLER_H_

#include "esp_http_server.h"
#include "esp_ota_ops.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

void print_firmware_version(void);
void ota_init(void);
void start_ota_task(void);
esp_err_t handle_ota_update(httpd_req_t *req);

#endif /* OTA_OTA_HANDLER_H_ */
