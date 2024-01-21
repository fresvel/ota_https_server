#include <stdio.h>
#include "wifi.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_flash.h>
#include <nvs_flash.h>
#include <esp_err.h>
#include <esp_system.h>
#include <esp_log.h>
#include <esp_wifi.h>

#include <wsse.h>

#include "lwip/netif.h"
#include "lwip/err.h"
#define STA_SSID "Asterisk"
#define STA_PASS "Ytjda3toGe"
const uint8_t retry=10;
uint8_t count=0;

static const char *TAG="WifiManager";
static esp_netif_t *s_ap_netif = NULL;

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){

    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START){
        ESP_LOGI(TAG, "Iniciando conexión con el punto de acceso\n");
        esp_wifi_connect();
    }else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        if (retry<count){
            ESP_LOGI(TAG, "Error de conexión, reintentando, número de intento %d\n",count++);
            esp_wifi_connect();
        }else{
            ESP_LOGE(TAG, "Error de conexión con el punto de acceso"); 
        }
        
    }else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP){
        ip_event_got_ip_t *event= (ip_event_got_ip_t*)event_data;
        printf("\033[0;35m");
        printf("Conexión establecida, ip obtenida: "IPSTR"\n", IP2STR(&event->ip_info.ip)); 
        
        
    }else{
        ESP_LOGE(TAG, "Error!!!");
        printf("\033[1;36m");
        printf("Valor de impresion\n");
        ESP_LOGI(TAG, "event base: %s id: %" PRId32, event_base, event_id);

    }

}

void config_wifi(void)
{
    esp_err_t ret =nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES||ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        ESP_ERROR_CHECK(nvs_flash_erase());
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    s_ap_netif=esp_netif_create_default_wifi_sta();


    esp_event_handler_instance_t ev_wifi;
    esp_event_handler_instance_t ev_ip;
    ESP_ERROR_CHECK( esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,wifi_event_handler,NULL,&ev_wifi));
    ESP_ERROR_CHECK( esp_event_handler_instance_register(IP_EVENT,IP_EVENT_STA_GOT_IP,wifi_event_handler,NULL,&ev_ip));
    config_wsse();//Configura el servicio de websockets
    wifi_init_config_t wifi_cfg=WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_cfg);

    wifi_config_t wifi_sta={};
    strcpy((char *)wifi_sta.sta.ssid,(char*)STA_SSID);
    strcpy((char *)wifi_sta.sta.password,(char*)STA_PASS);
    wifi_sta.sta.threshold.authmode=WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA,&wifi_sta));
    ESP_ERROR_CHECK(esp_wifi_start());
       
    
}
