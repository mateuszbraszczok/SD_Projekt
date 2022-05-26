#include "gpio.h"

void gpio_init_input(uint8_t num_pin)
{
    nrf_gpio_cfg_input(num_pin, NRF_GPIO_PIN_PULLDOWN);
}

void gpio_init_output(uint8_t num_pin)
{
    nrf_gpio_cfg_output(num_pin);
}

uint32_t gpio_read(uint8_t num_pin)
{
    uint32_t pin_level = false;
    pin_level = nrf_gpio_pin_read(num_pin);
    return pin_level;
}

void gpio_write(uint8_t num_pin, uint8_t value)
{
    if (value == 0)
    {
        nrf_gpio_pin_clear(num_pin);
    }
    else
    {
        nrf_gpio_pin_set(num_pin);
    }
}