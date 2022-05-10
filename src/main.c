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

#include "driver/dht.h"
#include "remote.h"

#define LOG_MODULE_NAME main
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   2500

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#endif



#define HYSTERESIS 1


#define STACKSIZE 1024
#define THREAD0_PRIORITY 1

K_MUTEX_DEFINE(test_mutex);

#define RELAY_PIN   NRF_GPIO_PIN_MAP(1,10)

static struct bt_conn *current_conn;

/* Declarations */
void on_connected(struct bt_conn *conn, uint8_t err);
void on_disconnected(struct bt_conn *conn, uint8_t reason);
void on_notif_changed(enum bt_button_notifications_enabled status);
void on_data_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len);

struct bt_conn_cb bluetooth_callbacks = 
{
	.connected 		= on_connected,
	.disconnected 	= on_disconnected,
};

struct bt_remote_service_cb remote_callbacks = 
{
	.notif_changed = on_notif_changed,
	.data_received = on_data_received,
};

#define SETPOINT 24 
int setPoint = SETPOINT;

/* Callbacks */

void on_data_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
	uint8_t temp_str[len+1];
	memcpy(temp_str, data, len);
	temp_str[len] = 0x00;


	printk("firsByte: %d\n", (temp_str[0] -48 ));
	printk("secondByte: %d\n", (temp_str[1] -48 ));

	setPoint = ((temp_str[0] -48 ) * 10) +(temp_str[1] -48 ) ;

	printk("dataLen: %d\n", len);

	printk("setPoint: %d\n", setPoint);



	LOG_INF("Received data on conn %p. Len: %d", (void *)conn, len);
	LOG_INF("Data: %s", log_strdup(temp_str));
}

void on_connected(struct bt_conn *conn, uint8_t err)
{
	if(err) 
	{
		LOG_ERR("connection err: %d", err);
		return;
	}
	LOG_INF("Connected.");
	current_conn = bt_conn_ref(conn);
}

void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("Disconnected (reason: %d)", reason);
	if(current_conn) 
	{
		bt_conn_unref(current_conn);
		current_conn = NULL;
	}
}

void on_notif_changed(enum bt_button_notifications_enabled status)
{
	if (status == BT_BUTTON_NOTIFICATIONS_ENABLED) 
	{
		LOG_INF("Notifications enabled");
	}
	else 
	{
		LOG_INF("Notificatons disabled");
	}
}

struct DHTReadings dht22;


void thread0(void)
{
	dht22.dhtModel = DHT22;
	
	while (1) 
	{
		k_mutex_lock(&test_mutex, K_FOREVER);

		//TEMPERATURE READING
		if (dhtRead(&dht22) == DHT_FAIL)
		{
			printk("Can't read temperature!\n");
			k_msleep(50);
		}
		else
		{
			printk("temperature: %d.%d C\n", dht22.temperatureIntPart, dht22.temperatureDecimalPart);
			printk("humidity: %d.%d %%\n", dht22.humidityIntPart, dht22.humidityDecimalPart);

			setTemperature(getTemperature(&dht22));
			setHumidity(getHumidity(&dht22));

			int err = send_button_notification(current_conn, getTemperature(&dht22));
			if (err) 
			{
				LOG_ERR("couldn't send notification (err: %d)", err);
			}
			k_msleep(SLEEP_TIME_MS);
		}

		k_mutex_unlock(&test_mutex);
	}
}


void main(void)
{
	const struct device *dev;
	bool led_is_on = true;

	dev = device_get_binding(LED0);
	if (dev == NULL) 
	{
		return;
	}

	int ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) 
	{
		return;
	}

	int i = 0;
	nrf_gpio_cfg_output(RELAY_PIN);
	nrf_gpio_pin_clear(RELAY_PIN);
	setState(0);

	
	int err = bluetooth_init(&bluetooth_callbacks, &remote_callbacks);

	if (err)
	{
		LOG_ERR("Bluetooth_init returned %d", err);
	}
	LOG_INF("Running");

	//int setPoint = SETPOINT;
	int hysteresis = HYSTERESIS;

	while (1) 
	{
		printk("iteration: %d\n", ++i);

		//RELAY CONTROLLING

		if (dht22.temperatureIntPart + (dht22.temperatureDecimalPart/10) < setPoint - hysteresis)
		{
			setState(1);
			nrf_gpio_pin_set(RELAY_PIN);
			printk("Relay is ON\n");
		}
		if (dht22.temperatureIntPart + (dht22.temperatureDecimalPart/10) > setPoint + hysteresis)
		{
			setState(0);
			nrf_gpio_pin_clear(RELAY_PIN);
			printk("Relay is OFF\n");
		}

		printk("\n\n");

		//BLINKING LED
		gpio_pin_set(dev, PIN, (int)led_is_on);
		led_is_on = !led_is_on;
		k_msleep(1000);
	}
}


K_THREAD_DEFINE(thread0_id, STACKSIZE, thread0, NULL, NULL, NULL,
		THREAD0_PRIORITY, 0, 0);
