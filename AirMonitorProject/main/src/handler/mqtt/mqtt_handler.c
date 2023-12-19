
#include <stdio.h>
#include <regex.h>
#include "esp_log.h"

#include "mqtt_handler.h"
#include "c_sensorSGP30.h"
#include "c_mqtt.h"

#include "mqtt_parser.h"
#include "context.h"

static const char* TAG = "MQTT_MANAGER";
//char *NODE_CONTEXT_MAIN = "2/3";


static void freq_topic_handler(char *data){
    int res = parse_int_data(data);
    if (res > 0) {
        //change_sample_period_sgp30(res);
        ESP_LOGI(TAG, "FREQ value is %d", res);
    } else {
        ESP_LOGE(TAG, "FREQ value is invalid: %s", data);
    }
}


static void onoff_topic_handler(char *data){
    switch (parse_int_data(data)) {
        case 0:
            ESP_LOGI(TAG, "SENSOR OFF");
            //stop_sensor_sgp30();
            break;
        case 1:
            ESP_LOGI(TAG, "SENSOR ON");
            //start_sensor_sgp30();
            break;
        default:
            ESP_LOGE(TAG, "ONOFF value is invalid: %s", data);
    }
}

void mqtt_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data) {
    
    switch(id){
        case C_MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT CONNECTED");
            mqtt_subscribe_to_topic(CONFIG_MQTT_LWT_TOPIC);
            mqtt_subscribe_to_topic(build_topic(context_get_node_ctx(), "/+"));
            
            //mqtt_publish_to_topic("2/3/ONOFF", (uint8_t *)"10");
        break;
        case C_MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT DISCONNECTED");
        break;
        case C_MQTT_EVENT_RECEIVED_DATA:
            struct mqtt_com_data* mqtt_data = (struct mqtt_com_data*)event_data;
            ESP_LOGI(TAG, "MQTT Received data %s from topìc %s", mqtt_data->data, mqtt_data->topic);
            int res;
            switch(mqtt_topic_parser(mqtt_data->topic)){
                case MQTT_FREQ_TOPIC:
                    freq_topic_handler(mqtt_data->data);
                break;
                case MQTT_ONOFF_TOPIC:
                    onoff_topic_handler(mqtt_data->data);
                break;
                case MQTT_MODE_TOPIC:
                    res = parse_int_data(mqtt_data->data);
                    ESP_LOGI(TAG, "SENSOR MODE SET TO %d", res);

                    //set_sensor_mode_sgp30(res);
                break;

                default:
                    ESP_LOGE(TAG, "UNKNOWN TOPIC: %s", mqtt_data->topic);
                break;
            }
        break;

    }
}


void mqtt_init(){
    _mqtt_init(context_get_node_ctx());
}
