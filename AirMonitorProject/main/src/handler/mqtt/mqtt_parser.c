#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include "esp_log.h"
#include "cJSON.h"
#include "context.h"

#include "mqtt_parser.h"

int its_for_me(const char *payload){
    cJSON* root = cJSON_Parse(payload);
    int res = -1;
    if (root != NULL){
        cJSON* paramsItem = cJSON_GetObjectItem(root, "params");
        cJSON* deviceItem = cJSON_GetObjectItem(paramsItem, "device");

        if(deviceItem!=NULL){
            char deviceID[20];
            snprintf(deviceID, sizeof(deviceID), "%s", deviceItem->valuestring);
            printf("ist me? %s\n", context_it_is_i(deviceID)==0?"yep":"nope");
            res = 0;
        }
    }

    cJSON_Delete(root);
    return res;
}

int parse_bool_value(const char *payload){
    cJSON* root = cJSON_Parse(payload);
    int result = MQTT_INVALID_VALUE;
    if (root != NULL){
        cJSON* paramsItem = cJSON_GetObjectItem(root, "params");
        cJSON* valueItem = cJSON_GetObjectItem(paramsItem, "val");

        if(valueItem!=NULL){
            result = cJSON_IsTrue(valueItem)?1:0;
        }
    }
    cJSON_Delete(root);
    return result;
}

int parse_int_value(const char *payload){
    cJSON* root = cJSON_Parse(payload);
    int result = MQTT_INVALID_VALUE;

    if (root != NULL){
        cJSON* paramsItem = cJSON_GetObjectItem(root, "params");
        cJSON* valueItem = cJSON_GetObjectItem(paramsItem, "val");

        if(valueItem!=NULL){
            result = valueItem->valueint;
        }
    }
    cJSON_Delete(root);
    return result;
}


static int parse_topic(char *topic){

    if (strcmp(topic, "FREQ") == 0) {
        return MQTT_SET_FREQ_TOPIC;
    } 
    else if (strcmp(topic, "ONOFF") == 0){
        return MQTT_SET_ONOFF_TOPIC;
    }
    else if (strcmp(topic, "MODE") == 0){
        return MQTT_SET_MODE_TOPIC;
    }
    else if (strcmp(topic, "G_FREQ") == 0) {
        return MQTT_GET_FREQ_TOPIC;
    } 
    else if (strcmp(topic, "G_ONOFF") == 0) {
        return MQTT_GET_ONOFF_TOPIC;
    } 
    else if (strcmp(topic, "G_MODE") == 0) {
        return MQTT_GET_MODE_TOPIC;
    }
    else{
        return MQTT_INVALID_VALUE;
    }
}

int parse_method(const char *payload) {
    cJSON* root = cJSON_Parse(payload);
    int res = MQTT_INVALID_VALUE;
    if (root != NULL){
        cJSON* methodItem = cJSON_GetObjectItem(root, "method");

        if(methodItem!=NULL){
            char method[20];
            snprintf(method, sizeof(method), "%s", methodItem->valuestring);
            res = parse_topic(method);
        }
    }
    cJSON_Delete(root);
    return res;
}



char *build_topic(char *base, char* comp){
    if (base != NULL) {
        // Calculate the length of the resulting string
        int length = strlen(base) + strlen(comp);

        // Allocate memory for the concatenated string +1 for null terminator
        char* context_topic = (char*)malloc(length + 1); 

        // Copy the original string to the new buffer
        strcpy(context_topic, base);

        // Concatenate "/+" to the string
        strcat(context_topic, comp);

        printf("%s\n", context_topic);

        return context_topic;
    }

    return NULL;
}



char *mqtt_topic_last_token(char *topic){

    char *last_topic = strrchr(topic, '/') + 1;

    if(last_topic!=NULL){
        int token_length = strlen(last_topic);
        char *last_token = (char *)malloc(token_length + 1); // +1 for the null terminator
        if (last_token != NULL) {
            strncpy(last_token, last_topic, token_length);
            last_token[token_length] = '\0'; // Ensure null termination
            return last_token;
        }
    }
    
    return NULL;
}


int parse_int_data(char *data){
    // Attempt to convert str1 to an integer
    char *endptr;
    int res = (int)strtol(data, &endptr, 10);

    // Check if conversion was successful
    if (*endptr == '\0') {
        return res;
    }
    return -1;
}

