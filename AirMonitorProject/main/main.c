#include <stdio.h>
#include "esp_log.h"
#include "esp_event.h"
#include "c_sensorSI7021.h"
#include "c_wifiConnection.h"

static const char* TAG = "main";

static void sensor_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data){
   
    float value = *((float*)event_data);
    
     if(id==SENSORSI7021_HUM_DATA){
            ESP_LOGI(TAG, "HUM: %f",  value);
        }
        else{
            ESP_LOGI(TAG, "TEMP: %f",  value);
        }
}


void app_main(void) {
    //wifi_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(SENSORSI7021_EVENT_BASE, SENSORSI7021_TEMP_DATA, sensor_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SENSORSI7021_EVENT_BASE, SENSORSI7021_HUM_DATA, sensor_event_handler, NULL));
    
    init_sensor_si7021();
}


