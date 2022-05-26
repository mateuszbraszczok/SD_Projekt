#include <stdint.h>
void ESP_write_data(float temperature, float humidity, int setPoint, int heaterState);
void spi_init(void);
void spi_deinit(void);
void ESP_read(void);