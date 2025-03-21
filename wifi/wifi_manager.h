/*
 * wifi_manager.h
 *
 *  Created on: Mar 20, 2025
 *      Author: Aram
 */

#ifndef WIFI_WIFI_MANAGER_H_
#define WIFI_WIFI_MANAGER_H_

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "lwip/inet.h"

void wifi_init_softap(void);
void init_nvs_flash(void);

#endif /* WIFI_WIFI_MANAGER_H_ */
