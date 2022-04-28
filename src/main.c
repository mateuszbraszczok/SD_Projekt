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
#include <logging/log.h>

#define LOG_MODULE_NAME main
LOG_MODULE_REGISTER(LOG_MODULE_NAME);


#include "driver/dht.h"
#include "remote.h"

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


static struct bt_conn *current_conn;

/* Declarations */
void on_connected(struct bt_conn *conn, uint8_t err);
void on_disconnected(struct bt_conn *conn, uint8_t reason);

struct bt_conn_cb bluetooth_callbacks = {
	.connected 		= on_connected,
	.disconnected 	= on_disconnected,
};

void on_connected(struct bt_conn *conn, uint8_t err)
{
	if(err) {
		LOG_ERR("connection err: %d", err);
		return;
	}
	LOG_INF("Connected.");
	current_conn = bt_conn_ref(conn);
	//dk_set_led_on(CONN_STATUS_LED);
}

void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("Disconnected (reason: %d)", reason);
	//dk_set_led_off(CONN_STATUS_LED);
	if(current_conn) {
		bt_conn_unref(current_conn);
		current_conn = NULL;
	}
}

/* Callbacks */

void main(void)
{
	const struct device *dev;
	bool led_is_on = true;
	int ret;

	int err;


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
	nrf_gpio_pin_clear(RELAY_PIN);

	struct DHTReadings dht22;
	dht22.dhtModel = DHT22;

	err = bluetooth_init(&bluetooth_callbacks);

	if (err)
	{
		LOG_ERR("Bluetooth_init returned %d", err);
	}
	LOG_INF("Running");

	while (1) 
	{
		printk("iteration: %d\n", ++i);

		//TEMPERATURE READING
		if (dhtRead(&dht22) == DHT_FAIL)
		{
			printk("Can't read temperature!\n");
		}
		else
		{
			printk("temperature: %d.%d C\n", dht22.temperatureIntPart, dht22.temperatureDecimalPart);
			printk("humidity: %d.%d %%\n", dht22.humidityIntPart, dht22.humidityDecimalPart);
		}
		
		
		//RELAY CONTROLLING
		int SetPoint;
		int Hysteresis;

		SetPoint = 24;
		Hysteresis = 1;

		if (dht22.temperatureIntPart + (dht22.temperatureDecimalPart/10) < SetPoint - Hysteresis)
		{
			nrf_gpio_pin_set(RELAY_PIN);
			printk("Relay is ON\n");
		}
		if (dht22.temperatureIntPart + (dht22.temperatureDecimalPart/10) > SetPoint + Hysteresis)
		{
			nrf_gpio_pin_clear(RELAY_PIN);
			printk("Relay is OFF\n");
		}

		printk("\n\n");

		//BLINKING LED
		gpio_pin_set(dev, PIN, (int)led_is_on);
		led_is_on = !led_is_on;
		k_msleep(SLEEP_TIME_MS);
	}
}
