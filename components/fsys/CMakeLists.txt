idf_component_register(SRCS "fsys.c"
                    INCLUDE_DIRS "include"
                    REQUIRES spiffs)
spiffs_create_partition_image(storage ../../files FLASH_IN_PROJECT)