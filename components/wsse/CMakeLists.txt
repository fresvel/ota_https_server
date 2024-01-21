idf_component_register(SRCS "wsse.c" "keep_alive.c"
                    INCLUDE_DIRS "include"
                    EMBED_TXTFILES "certs/servercert.pem" "certs/prvtkey.pem"
                    REQUIRES esp_https_server wifi esp_timer fsys app_update)
