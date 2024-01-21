/* Simple HTTP + SSL + WS Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_ota_ops.h"
#include "esp_wifi.h"
#include "lwip/sockets.h"
#include <esp_https_server.h>
#include "keep_alive.h"
#include "sdkconfig.h"
#include "fsys.h"

#if !CONFIG_HTTPD_WS_SUPPORT
#error This example cannot be used unless HTTPD_WS_SUPPORT is enabled in esp-http-server component configuration
#endif

typedef struct{
    void *resp;
    char *data;
}ws_send_t;


struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
};

static const char *TAG = "wss_echo_server";
static const size_t max_clients = 4;



static void httpd_send_file(void*param,char * buffer){
    httpd_req_t *req =(httpd_req_t*)param;
    httpd_resp_sendstr_chunk(req,buffer);
    printf("Sending file...\n");
}

static esp_err_t panel_handler(httpd_req_t *req){
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "New client connected");
        
        httpd_resp_set_type(req, "text/html");
        read_file("/spiffs/html/panel.html",httpd_send_file,req);
        //httpd_resp_send(req, buffer,HTTPD_RESP_USE_STRLEN);
        
    }
    return ESP_OK;
    
}

static esp_err_t bulma_handler(httpd_req_t *req){
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "New client connected");
        
        httpd_resp_set_type(req, "text/css");
        read_file("/spiffs/css/bulma.css",httpd_send_file,req);
        //read_file("/spiffs/ccs/bulma.css",httpd_send_file,req);

        //httpd_resp_send(req, buffer,HTTPD_RESP_USE_STRLEN);
        
    }
    return ESP_OK;
    
}

static esp_err_t wsclient_handler(httpd_req_t *req){
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "New client connected");
        
        httpd_resp_set_type(req, "text/javascript");
        read_file("/spiffs/js/wsclient.js",httpd_send_file,req);
        //httpd_resp_send(req, buffer,HTTPD_RESP_USE_STRLEN);
        
    }
    return ESP_OK;
    
}

static esp_err_t paneljs_handler(httpd_req_t *req){
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "New client connected");
        
        httpd_resp_set_type(req, "text/javascript");
        read_file("/spiffs/js/panel.js",httpd_send_file,req);
        //httpd_resp_send(req, buffer,HTTPD_RESP_USE_STRLEN);
        
    }
    return ESP_OK;
    
}

static esp_err_t chartjs_handler(httpd_req_t *req){
    if (req->method == HTTP_GET){
        ESP_LOGI(TAG, "New client connected");
        httpd_resp_set_type(req, "text/javascript");
        read_file("/spiffs/js/chart.js",httpd_send_file,req);
        
    }
    return ESP_OK;      
}

static esp_err_t logo_handler(httpd_req_t *req){
    if (req->method == HTTP_GET){
        ESP_LOGI(TAG, "New client connected");
        httpd_resp_set_type(req, "image/svg+xml");
        read_file("/spiffs/img/logo.svg",httpd_send_file,req);
        
    }
    return ESP_OK;      
}


int getSubstring(const char *entrada, char *subcadena, int posicionInicial, size_t size_input) {
    if (entrada == NULL || subcadena == NULL || posicionInicial < 0) {
        // Verificar si los argumentos son válidos
        printf("Error1\n");
        return -1; // Error: Argumentos inválidos
    }

    
    if (posicionInicial >= size_input) {
        // La posición inicial está más allá del final de la cadena
        printf("Error2\n");
        return 0; // Tamaño del substring es cero
    }

    // Calcular el tamaño del substring
    size_t tamañoSubstring = size_input - posicionInicial;

    // Copiar el substring a la memoria asignada
    memcpy(subcadena, entrada + posicionInicial, tamañoSubstring);

    // Agregar el carácter nulo al final del substring
    subcadena[tamañoSubstring] = '\0';
    printf("Ok\n");
    //printf("Valor de substring: %s\n", subcadena);
    // Devolver el tamaño del substring
    return (int)tamañoSubstring;
}


static esp_err_t ota_handler(httpd_req_t *req){
    if (req->method == HTTP_GET){
        ESP_LOGI(TAG, "New client connected");
        httpd_resp_set_type(req, "text/html");
        read_file("/spiffs/html/ota.html",httpd_send_file,req);
        
    }else if (req->method == HTTP_POST){

        esp_err_t err;
        /* update handle : set by esp_ota_begin(), must be freed via esp_ota_end() */
        esp_ota_handle_t update_handle = 0 ;
        const esp_partition_t *update_partition = NULL;

        ESP_LOGI(TAG, "Starting OTA web server task");

        const esp_partition_t *configured = esp_ota_get_boot_partition();
        const esp_partition_t *running = esp_ota_get_running_partition();

        if (configured != running) {
            ESP_LOGW(TAG, "Configured OTA boot partition at offset 0x%08"PRIx32", but running from offset 0x%08"PRIx32,
                    configured->address, running->address);
            ESP_LOGW(TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
        }
        ESP_LOGI(TAG, "Running partition: %s", running->label);

        update_partition = esp_ota_get_next_update_partition(NULL);
        assert(update_partition != NULL);
        ESP_LOGI(TAG, "Writing to partition: %s",update_partition->label);

        
        err = esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
            esp_ota_abort(update_handle);
            //task_fatal_error();
        }
        ESP_LOGI(TAG, "esp_ota_begin succeeded");



        // Buffer para almacenar los datos del cuerpo de la solicitud
        char buffer[1024];
        char buffercp[1024];
        int received, total = 0,tosend=0;
        
        // Lee los datos del cuerpo de la solicitud en el búfer
        while ((received = httpd_req_recv(req, buffer, sizeof(buffer))) > 0) {
            
            memcpy(buffercp, buffer,1024);
            tosend = received;
            //printf("Total: %s\n", buffercp);
            if (total == 0)
            {

                char subdata[1024];

                //int ss=getSubstring(buffer,subdata,150,1024);
                //printf("Valor de ss: %s\n", subdata);
                
                //strcpy(buffercp, buffer);
                char *line = strtok(buffer, "\n");
                tosend=strlen(line);
                printf("line 1: %s\n", line);
                
                line = strtok(NULL,"\n");
                tosend+=strlen(line);
                printf("line 2: %s\n", line);
                
                line = strtok(NULL,"\n");
                tosend+=strlen(line);
                printf("line 3: %s\n", line);
                
                line = strtok(NULL,"\n");
                tosend+=strlen(line);
                printf("line 4: %s\n", line);


              
                int ss=getSubstring(buffercp,subdata,tosend+4,1024);
                

                memcpy(buffer,subdata,1020-tosend);
                tosend=1020-tosend;

                printf("Complemento y size nuevo de:%d\n", 1020-tosend);
                printf("Buffer: %s\n", buffer);
                
                
            }else if (total>= req->content_len-1024)
            {
                ESP_LOGI(TAG, "Ultimo buffer\n");

                const char *resultado = strstr(buffer, "-----------------------------");

                uint32_t posicion=0;
                
                if (resultado != NULL) {
                    posicion = (uint32_t)(resultado - buffer);
                    printf("\n\n\n\nPatrón encontrado en la posición %"PRIu32" De un received de: %d\n", posicion, received);
                } else {
                    printf("\n\n\n\nPatrón no encontrado.\n");
                }

                //buffer[posicion]=0;
                //memset(buffer,255,);
                //printf("\033[0;33m");
                printf("Buffer final:\n");
                printf("%s\n", buffer);
                //tosend=1024-posicion;
                

            }
            
            err = esp_ota_write( update_handle, (const void *)buffer,tosend);
                if (err != ESP_OK) {
                    esp_ota_abort(update_handle);
                    ESP_LOGE(TAG,"OTA WRITE FAILED!!\n");
                    //task_fatal_error();
                }
            //printf("%s", buffer); 



            total+=received;
            printf("Recieved %d of the \n", total);
            
            
            
        }

        err = esp_ota_end(update_handle);
        if (err != ESP_OK) {
            if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
                ESP_LOGE(TAG, "Image validation failed, image is corrupted");
            } else {
                ESP_LOGE(TAG, "esp_ota_end failed (%s)!", esp_err_to_name(err));
            }
            //task_fatal_error();
            ESP_LOGE(TAG,"OTA END FAILED!!\n");

        }

        err = esp_ota_set_boot_partition(update_partition);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)!", esp_err_to_name(err));
            //task_fatal_error();
        }
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req,"Archivo Subido",14);
        ESP_LOGW(TAG,"Archivo recibido, tamaño: %d total:%d\n",req->content_len,total);

        ESP_LOGI(TAG, "Prepare to restart system!");
        esp_restart();
        

        

        
    }
    return ESP_OK;      
}

static esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));

    // First receive the full ws message
    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }
    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len) {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
    }
    // If it was a PONG, update the keep-alive
    if (ws_pkt.type == HTTPD_WS_TYPE_PONG) {
        ESP_LOGD(TAG, "Received PONG message");
        free(buf);
        return wss_keep_alive_client_is_active(httpd_get_global_user_ctx(req->handle),
                httpd_req_to_sockfd(req));

    // If it was a TEXT message, just echo it back
    } else if (ws_pkt.type == HTTPD_WS_TYPE_TEXT || ws_pkt.type == HTTPD_WS_TYPE_PING || ws_pkt.type == HTTPD_WS_TYPE_CLOSE) {
        if (ws_pkt.type == HTTPD_WS_TYPE_TEXT) {
            ESP_LOGI(TAG, "Received packet with message: %s", ws_pkt.payload);
        } else if (ws_pkt.type == HTTPD_WS_TYPE_PING) {
            // Response PONG packet to peer
            ESP_LOGI(TAG, "Got a WS PING frame, Replying PONG");
            ws_pkt.type = HTTPD_WS_TYPE_PONG;
        } else if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE) {
            // Response CLOSE packet with no payload to peer
            ws_pkt.len = 0;
            ws_pkt.payload = NULL;
        }
        ret = httpd_ws_send_frame(req, &ws_pkt);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
        }
        ESP_LOGI(TAG, "ws_handler: httpd_handle_t=%p, sockfd=%d, client_info:%d", req->handle,
                 httpd_req_to_sockfd(req), httpd_ws_get_fd_info(req->handle, httpd_req_to_sockfd(req)));
        free(buf);
        return ret;
    }
    free(buf);
    return ESP_OK;
}

esp_err_t wss_open_fd(httpd_handle_t hd, int sockfd)
{
    ESP_LOGI(TAG, "New client connected %d", sockfd);
    wss_keep_alive_t h = httpd_get_global_user_ctx(hd);
    return wss_keep_alive_add_client(h, sockfd);
}

void wss_close_fd(httpd_handle_t hd, int sockfd)
{
    ESP_LOGI(TAG, "Client disconnected %d", sockfd);
    wss_keep_alive_t h = httpd_get_global_user_ctx(hd);
    wss_keep_alive_remove_client(h, sockfd);
    close(sockfd);
}

static const httpd_uri_t ws = {
        .uri        = "/ws",
        .method     = HTTP_GET,
        .handler    = ws_handler,
        .user_ctx   = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = true
};
/*
static const httpd_uri_t get_panel={
    .uri = "/panel",
    .method = HTTP_GET,
    .handler = panel_handler,
    .user_ctx = NULL
};
*/
static const httpd_uri_t panel_js_uri={
    .uri = "/panel.js",
    .method = HTTP_GET,
    .handler = paneljs_handler,
    .user_ctx = NULL
};

static const httpd_uri_t wsclient_js_uri={
    .uri = "/wsclient.js",
    .method = HTTP_GET,
    .handler = wsclient_handler,
    .user_ctx = NULL
};

static const httpd_uri_t chart_js_uri={
    .uri = "/chart.js",
    .method = HTTP_GET,
    .handler = chartjs_handler,
    .user_ctx = NULL
};

static const httpd_uri_t bulma_css_uri={
    .uri = "/bulma.css",
    .method = HTTP_GET,
    .handler = bulma_handler,
    .user_ctx = NULL
};

static const httpd_uri_t logo_img_uri={
    .uri = "/logo.svg",
    .method = HTTP_GET,
    .handler = logo_handler,
    .user_ctx = NULL
};


static const httpd_uri_t ota_uri={
    .uri = "/ota.html",
    .method = HTTP_GET,
    .handler = ota_handler,
    .user_ctx = NULL
};

static const httpd_uri_t ota_post_uri={
    .uri = "/ota_post",
    .method = HTTP_POST,
    .handler = ota_handler,
    .user_ctx = NULL
};

static void send_hello(void *arg)
{
    ws_send_t *ws_send_ms=(ws_send_t *)arg;

    struct async_resp_arg *resp_arg = ws_send_ms->resp;
    char * data=ws_send_ms->data;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t*)data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

static void send_ping(void *arg)
{
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = NULL;
    ws_pkt.len = 0;
    ws_pkt.type = HTTPD_WS_TYPE_PING;

    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

bool client_not_alive_cb(wss_keep_alive_t h, int fd)
{
    ESP_LOGE(TAG, "Client not alive, closing fd %d", fd);
    httpd_sess_trigger_close(wss_keep_alive_get_user_ctx(h), fd);
    return true;
}

bool check_client_alive_cb(wss_keep_alive_t h, int fd)
{
    ESP_LOGD(TAG, "Checking if client (fd=%d) is alive", fd);
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = wss_keep_alive_get_user_ctx(h);
    resp_arg->fd = fd;

    if (httpd_queue_work(resp_arg->hd, send_ping, resp_arg) == ESP_OK) {
        return true;
    }
    return false;
}

static httpd_handle_t start_wss_echo_server(void)
{
    // Prepare keep-alive engine
    wss_keep_alive_config_t keep_alive_config = KEEP_ALIVE_CONFIG_DEFAULT();
    keep_alive_config.max_clients = max_clients;
    keep_alive_config.client_not_alive_cb = client_not_alive_cb;
    keep_alive_config.check_client_alive_cb = check_client_alive_cb;
    wss_keep_alive_t keep_alive = wss_keep_alive_start(&keep_alive_config);

    // Start the httpd server
    httpd_handle_t server = NULL;
    ESP_LOGI(TAG, "Starting server");

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();
    conf.httpd.max_open_sockets = max_clients;
    conf.httpd.global_user_ctx = keep_alive;
    conf.httpd.open_fn = wss_open_fd;
    conf.httpd.close_fn = wss_close_fd;

    extern const unsigned char servercert_start[] asm("_binary_servercert_pem_start");
    extern const unsigned char servercert_end[]   asm("_binary_servercert_pem_end");
    conf.servercert = servercert_start;
    conf.servercert_len = servercert_end - servercert_start;

    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
    conf.prvtkey_pem = prvtkey_pem_start;
    conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

    

    esp_err_t ret = httpd_ssl_start(&server, &conf);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &ws);
    //httpd_register_uri_handler(server,&get_panel);
    httpd_register_uri_handler(server,&wsclient_js_uri);
    httpd_register_uri_handler(server,&bulma_css_uri);
    httpd_register_uri_handler(server,&panel_js_uri);
    httpd_register_uri_handler(server,&chart_js_uri);
    httpd_register_uri_handler(server,&logo_img_uri);
    httpd_register_uri_handler(server,&ota_uri);
    httpd_register_uri_handler(server,&ota_post_uri);
    wss_keep_alive_set_user_ctx(keep_alive, server);

    return server;
}

static esp_err_t stop_wss_echo_server(httpd_handle_t server)
{
    // Stop keep alive thread
    wss_keep_alive_stop(httpd_get_global_user_ctx(server));
    // Stop the httpd server
    return httpd_ssl_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        if (stop_wss_echo_server(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop https server");
        }
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        *server = start_wss_echo_server();
    }
}

// Get all clients and send async message
static void wss_server_send_messages(void* param)
{
    ESP_LOGI(TAG, "Init task ws send messages");
    httpd_handle_t *server = (httpd_handle_t*) param;
    bool send_messages = true;
    ws_send_t *ws_send_ms= malloc(sizeof(ws_send_t));;
    // Send async message to all connected clients that use websocket protocol every 10 seconds
    while (send_messages) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Trying to send a message");
        if (!*server) { // httpd might not have been created by now
            continue;
        }
        ESP_LOGI(TAG, "Server ready! Trying to send a message");
        size_t clients = max_clients;
        int    client_fds[max_clients];
        if (httpd_get_client_list(*server, &clients, client_fds) == ESP_OK) {
            for (size_t i=0; i < clients; ++i) {
                int sock = client_fds[i];
                if (httpd_ws_get_fd_info(*server, sock) == HTTPD_WS_CLIENT_WEBSOCKET) {
                    ESP_LOGI(TAG, "Active client (fd=%d) -> sending async message", sock);
                    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
                    resp_arg->hd = *server;
                    resp_arg->fd = sock;
                    ws_send_ms->data=strdup("50.3");
                    ws_send_ms->resp=resp_arg;
                    if (httpd_queue_work(resp_arg->hd, send_hello, ws_send_ms) != ESP_OK) {
                        ESP_LOGE(TAG, "httpd_queue_work failed!");
                        send_messages = false;
                        break;
                    }
                }
            }
        } else {
            ESP_LOGE(TAG, "httpd_get_client_list failed!");
            return;
        }
    }
}

void config_wsse(void)
{
    printf("\nInit websocket configuration\n");
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
    printf("\nInit websocket configuration\n");
    xTaskCreate(wss_server_send_messages,"send messages",8192,&server,10,NULL);
    printf("\nWebsocket configuration was finished successfully\n");
}