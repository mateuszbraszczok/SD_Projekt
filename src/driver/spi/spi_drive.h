#include <stdint.h>
void ESP_write_Hello(void);
void ESP_write_Alive(void);
void ESP_write(uint8_t* data);
void ESP_write_data(float temperature, float humidity, int setPoint, int heaterState);
void spi_init(void);
void spi_uninit(void);
void ESP_read(void);