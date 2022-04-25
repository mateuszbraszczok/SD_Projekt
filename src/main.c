/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include "libraries/nrf_gpio.h"


#include "driver/dht.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   3000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN	0
#define FLAGS	0
#endif




#define RELAY_PIN   NRF_GPIO_PIN_MAP(1,10)


void main(void)
{
	const struct device *dev;
	bool led_is_on = true;
	int ret;


	dev = device_get_binding(LED0);
	if (dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		return;
	}

	int i = 0;
	nrf_gpio_cfg_output(RELAY_PIN);
	while (1) 
	{

		if(i%2==0)
		{
			nrf_gpio_pin_set(RELAY_PIN);
			printk("ON\n");
		}
		else 
		{
			nrf_gpio_pin_clear(RELAY_PIN);
			printk("OFF\n");
		}
		
		
		//nrf_gpio_pin_clear(RELAY_PIN);
		printk("hello: %d\n", ++i);

		//sd_ble_gap_address_get()

		// struct dht22Readings dht22;
		// dht22.dhtModel = DHT22;
		// dhtRead(&dht22);
		// printk("temperature: %d.%d C\n", dht22.temperatureIntPart, dht22.temperatureDecimalPart);
		// printk("humidity: %d.%d %%\n", dht22.humidityIntPart, dht22.humidityDecimalPart);
		// printk("\n\n");

		//BLINKING LED
		gpio_pin_set(dev, PIN, (int)led_is_on);
		led_is_on = !led_is_on;
		k_msleep(SLEEP_TIME_MS);
	}
}
