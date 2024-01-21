idf_component_register(SRCS "ota.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_event app_update esp_http_client driver
                    EMBED_TXTFILES "server_certs/ca_cert.pem")
