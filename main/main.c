#include "ota_handler.h"
#include "wifi_manager.h"
#include "dns_server.h"
#include "webserver.h"

void app_main(void)
{
    // Print current firmware version
    print_firmware_version();
    
    // Initialize OTA
    ota_init();
    
    // Initialize networking stack
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize NVS flash
    init_nvs_flash();
    
    // Initialize Wi-Fi including netif with default config
    esp_netif_create_default_wifi_ap();
    
    // Initialize Wi-Fi in SoftAP mode
    wifi_init_softap();

    // Start the web server
    start_webserver();
		
    // Start the DNS server
    dns_server_config_t dns_config = DNS_SERVER_CONFIG_SINGLE("*", "WIFI_AP_DEF");
    start_dns_server(&dns_config);
    
    ESP_LOGI("MAIN", "Captive portal started. Connect to WiFi SSID: %s", CONFIG_ESP_WIFI_SSID);
    
    // Start OTA task
    start_ota_task();
}

