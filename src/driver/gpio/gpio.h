#include <nrfx_gpiote.h>
#include <nrfx.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

void gpio_init_input (uint8_t num_pin);
void gpio_init_output (uint8_t num_pin);
uint32_t gpio_read(uint8_t num_pin);
void gpio_write(uint8_t num_pin, uint8_t value);