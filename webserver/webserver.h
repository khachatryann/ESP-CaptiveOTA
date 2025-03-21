/*
 * webserver.h
 *
 *  Created on: Mar 20, 2025
 *      Author: Aram
 */

#ifndef WEBSERVER_WEBSERVER_H_
#define WEBSERVER_WEBSERVER_H_

#include "esp_http_server.h"
#include "esp_log.h"

httpd_handle_t start_webserver(void);

#endif /* WEBSERVER_WEBSERVER_H_ */
