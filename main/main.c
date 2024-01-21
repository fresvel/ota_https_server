#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <gpio.h>
#include <uart.h>
#include <wifi.h>
#include <fsys.h>

#include <ota.h>

void funcion_send(void*param, char*buffer){
    if (buffer!=NULL)
    {
        //printf("Función send %s\n", buffer);    
    }else{ 
        printf("Función send finalizada\n");
    }
    
}

void app_main(void)
{

config_gpio();
config_uart();
config_wifi();

config_fsys();
read_file("/spiffs/html/panel.html", funcion_send,NULL);

//funcion_ota();
printf("\n*****Iniciando OTA*****\n");
config_ota();
printf("\n*****Fin de OTA*****");
//ota_main();
}
