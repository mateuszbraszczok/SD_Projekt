#include <stdint.h>
void ESP_write_Hello(void);
void ESP_write_Alive(void);
void ESP_write(uint8_t* data);
void spi_init(void);
void spi_uninit(void);
void ESP_read(void);