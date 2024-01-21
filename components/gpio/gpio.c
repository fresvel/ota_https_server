#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "gpio.h"
#include "driver/gpio.h"
#include "esp_log.h"


#define LED1 22
#define LED2 18
#define LED3 26
#define mask_out (1ULL <<LED1|1ULL<<LED2|1ULL<<LED3)
#define BTN1 5
#define BTN2 25
#define BTN3 19
#define mask_in (1ULL <<BTN1|1ULL<<BTN2|1ULL<<BTN3)

bool estado=false;
static const char *TAG="GPIO";

static QueueHandle_t gpio_queue=NULL;

static void IRAM_ATTR gpio_isr_handler(void*arg){
    uint32_t pin=(uint32_t)arg;
    xQueueSendFromISR(gpio_queue,&pin,pdFALSE);
}

static void gpio_task(){
    ESP_LOGI(TAG,"Tarea GPIO inicializada");

    u_int32_t pin = 0;
    while (pdTRUE)
    {
        if (xQueueReceive(gpio_queue,&pin,portMAX_DELAY))
        {
            ESP_LOGI(TAG, "Pulso detectado en el pin %"PRIu32"\n",pin);
            gpio_set_level(LED1,estado=!estado);
            gpio_set_level(LED2,estado=!estado);
            gpio_set_level(LED3,estado=!estado);
        }
        
    }
    
}

void config_gpio(){
    gpio_config_t gpio_cfg={};
    gpio_cfg.intr_type=GPIO_INTR_DISABLE;
    gpio_cfg.mode=GPIO_MODE_OUTPUT;
    gpio_cfg.pin_bit_mask=mask_out;
    gpio_cfg.pull_down_en=false;
    gpio_cfg.pull_up_en=false;
    gpio_config(&gpio_cfg);
    gpio_cfg.intr_type=GPIO_INTR_POSEDGE;
    gpio_cfg.mode=GPIO_MODE_INPUT;
    gpio_cfg.pin_bit_mask=mask_in;
    gpio_cfg.pull_down_en=true;
    gpio_config(&gpio_cfg);

    gpio_queue=xQueueCreate(10, sizeof(uint8_t));
    xTaskCreate(gpio_task,"gpio",2048,NULL,10,NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN1,gpio_isr_handler,(void*)BTN1);
    gpio_isr_handler_add(BTN2,gpio_isr_handler,(void*)BTN2);
    gpio_isr_handler_add(BTN3,gpio_isr_handler,(void*)BTN3);
}
