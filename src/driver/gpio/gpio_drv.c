#include "gpio_drv.h"

  

void gpio_init(uint8_t num_pin)
{
    uint8_t num_pin_local;
    num_pin_local = NRF_GPIO_PIN_MAP(0,num_pin);
    nrf_gpio_cfg_input(num_pin_local, NRF_GPIO_PIN_PULLDOWN);
}
uint32_t gpio_read(uint8_t num_pin)
{
    uint8_t num_pin_local;
    uint32_t pin_level = false;
    num_pin_local = NRF_GPIO_PIN_MAP(0,num_pin);
    pin_level = nrf_gpio_pin_read(num_pin_local);
    return pin_level;

}