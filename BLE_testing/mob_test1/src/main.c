/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <dk_buttons_and_leds.h>
#include "gss.h"
#include "gps.h"
#include "hum.h"
#include "ble.h"

LOG_MODULE_REGISTER(mob_test1, LOG_LEVEL_INF);

#define RUN_LED_BLINK_INTERVAL  1000

static bool app_button_state;

/* STEP 8.2 - Define the application callback function for controlling the LED*/
static void app_led_cb(bool led_state)
{
	dk_set_led(USER_LED, led_state);
}

struct bt_conn_cb connection_callbacks = {
	.connected = on_connected,
	.disconnected = on_disconnected,
};
static struct gss_cb_s app_callbacks = {
	.gps_cb	= app_led_cb,
	.hum_cb	= NULL,
};

static void button_changed(uint32_t button_state, uint32_t has_changed)
{
	if (has_changed & USER_BUTTON) {
		uint32_t user_button_state = button_state & USER_BUTTON;
		app_button_state = user_button_state ? true : false;
	}
}

static int init_button(void)
{
	int err;

	err = dk_buttons_init(button_changed);
	if (err) {
		printk("Cannot init buttons (err: %d)\n", err);
	}

	return err;
}

void main(void)
{
	int blink_status = 0;
	int err;

	LOG_INF("Starting MOB unit application \n");

	err = dk_leds_init();
	if (err) {
		LOG_ERR("LEDs init failed (err %d)\n", err);
		return;
	}

	err = init_button();
	if (err) {
		LOG_ERR("Button init failed (err %d)\n", err);
		return;
	}

	err = gss_init(&app_callbacks);
	if (err)
	{
		LOG_ERR("Failed to init GSS (err:%d)\n", err);
		return;
	}
	
	ble_init(&connection_callbacks);

	for (;;) {
		dk_set_led(RUN_STATUS_LED, (++blink_status) % 2);
		k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));
	}
}