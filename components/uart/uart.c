#include <stdio.h>
#include "uart.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include "driver/uart.h"
#include "driver/gpio.h"

#define BUFF_SIZE 2048
static const char * TAG = "UART";
QueueHandle_t uart_queue=NULL;
static void uart_event_handler(void *arg){

uart_event_t event;
uint8_t* buffer=(uint8_t*)malloc(BUFF_SIZE);
    while (pdTRUE)
    {
        if (xQueueReceive(uart_queue,&event,portMAX_DELAY))
        {
            switch (event.type)
            {
            case UART_DATA:
            uart_read_bytes(UART_NUM_0,buffer,event.size,portMAX_DELAY);
            printf("event size: %d\n",event.size);
            buffer[event.size]='\0';
            ESP_LOGI(TAG, "Datos recibidos: %s",buffer);
            
            if (buffer[0]=='a')
            {
                gpio_set_level(22,1);
                gpio_set_level(18,1);
                gpio_set_level(26,1);
                printf("led on\n");
            }else if (buffer[0]=='b')
            {
                gpio_set_level(22,0);
                gpio_set_level(18,0);
                gpio_set_level(26,0);
                printf("led off\n");
            }
            
            
            
            default:
                break;
            }
        }
        
    }
    
}


void config_uart(void)
{
    uart_config_t uart_cfg={};
    uart_cfg.baud_rate=115200;
    uart_cfg.data_bits=UART_DATA_8_BITS;
    uart_cfg.flow_ctrl=UART_HW_FLOWCTRL_DISABLE;
    uart_cfg.parity=UART_PARITY_DISABLE;
    uart_cfg.source_clk=UART_SCLK_DEFAULT;
    uart_cfg.stop_bits=UART_STOP_BITS_1;
    uart_param_config(UART_NUM_0,&uart_cfg);
    uart_set_pin(UART_NUM_0,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);

    uart_driver_install(UART_NUM_0,BUFF_SIZE,BUFF_SIZE,10,&uart_queue,0);
    xTaskCreate(uart_event_handler,"uart",2048,NULL,10,NULL);

}
