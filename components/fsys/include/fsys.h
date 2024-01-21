typedef void (*file_send_t)(void *req, char* buffer);
void config_fsys(void);
void read_file(char *path, file_send_t file_send, void *param);
