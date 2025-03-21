/*
 * ota_handler.c
 *
 *  Created on: Mar 20, 2025
 *      Author: Aram
 */

#include "ota_handler.h"

static const char *TAG = "OTA";

// Shared buffers and variables for OTA update
#define OTA_BUFFER_SIZE 1024
int received = 0;
char buf[OTA_BUFFER_SIZE];
static SemaphoreHandle_t ota_semaphore = NULL;
static bool ota_update_in_progress = false;
static int total_received = 0;
static bool ota_end_flag = false;

// OTA handles
static TaskHandle_t ota_update_task_handle = NULL;
static esp_ota_handle_t ota_handle;
static const esp_partition_t *ota_partition = NULL;

void print_firmware_version(void)
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
        printf("Running firmware version: %s\n", running_app_info.version);
    }
}

void ota_update_task(void* pvParameters)
{
    esp_err_t err;
    
    // Get next OTA partition
    ota_partition = esp_ota_get_next_update_partition(NULL);
    if (ota_partition == NULL) {
        ESP_LOGE(TAG, "OTA partition not found");
    }
    
    ESP_LOGI(TAG, "Starting OTA update to partition: %s", ota_partition->label);
    
    // Start OTA
    err = esp_ota_begin(ota_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed, error=%d", err);
    }

    // Process data blocks
    ESP_LOGI(TAG, "OTA STARTS WRITING");
    while (true) {
        if(xSemaphoreTake(ota_semaphore, 100) == pdTRUE){
            esp_ota_write(ota_handle, buf, received);
            ESP_LOGI(TAG, "Ota writes %d", received);
        }
        else if(ota_end_flag && !received){ // upload is completed
            break;
        }
        else if(ota_end_flag){
            ESP_LOGE(TAG, "OTA update failed");
            vTaskDelete(NULL);
        }
    }
    
    // Finish OTA update
    err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed, error=%d", err);
    }
    
    // Set the new boot partition
    err = esp_ota_set_boot_partition(ota_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed, error=%d", err);
    }
    
    ESP_LOGI(TAG, "OTA update successful. Rebooting in 5 seconds...");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    esp_restart();

    vTaskDelete(NULL);
}

void ota_init(void)
{
    // Initialize semaphore for OTA buffer synchronization
    ota_semaphore = xSemaphoreCreateBinary();
    if (ota_semaphore == NULL) {
        ESP_LOGE(TAG, "Failed to create semaphore");
        return;
    }
    xSemaphoreGive(ota_semaphore);
    
    // Reset counters
    total_received = 0;
    received = 0;
    ota_end_flag = false;
    ota_update_in_progress = false;
}

void start_ota_task(void)
{
    xTaskCreate(&ota_update_task, "ota_update_task", 10 * 1024, NULL, 23, &ota_update_task_handle);
    ota_update_in_progress = true;
}

esp_err_t handle_ota_update(httpd_req_t *req)
{
    esp_err_t ret = ESP_OK;
    
    // Receive firmware chunks
    while ((received = httpd_req_recv(req, buf, sizeof(buf))) > 0) {
        // Copy data to OTA buffer
        total_received += received;
        
        ESP_LOGI(TAG, "Received firmware bytes: %d, Total: %d", received, total_received);
        xSemaphoreGive(ota_semaphore);
    }
    
    // Check if connection closed or error
    if (received < 0 && received != HTTPD_SOCK_ERR_TIMEOUT) {
        if (ota_update_in_progress) {
            ota_end_flag = true;
            xSemaphoreGive(ota_semaphore);
        }
        ESP_LOGE(TAG, "Error during firmware reception: %d", received);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    
    // Upload completed successfully
    if (received == 0) {
        ESP_LOGI(TAG, "Firmware upload complete, total bytes: %d", total_received);
        ota_end_flag = true;
        xSemaphoreGive(ota_semaphore);
        httpd_resp_sendstr(req, "Upload Complete. Device will reboot shortly.");
    }
    
    return ret;
}


