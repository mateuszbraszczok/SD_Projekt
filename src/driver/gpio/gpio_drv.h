#include <nrfx_gpiote.h>
#include <nrfx.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

void gpio_init (uint8_t num_pin);
uint32_t gpio_read(uint8_t num_pin);