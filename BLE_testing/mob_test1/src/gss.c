/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *  @brief LED Button Service (LBS) sample
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "gss.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(gss, LOG_LEVEL_DBG);

static bool button_state;
static struct gss_cb_s gss_cb;

/* GPS Sensor Service Declaration */
/* Creates and adds the GSS service to the Bluetooth LE stack */
BT_GATT_SERVICE_DEFINE(gss_svc,
					   BT_GATT_PRIMARY_SERVICE(BT_UUID_GSS),
					   /* STEP 3 - Create and add the Button characteristic */
					   BT_GATT_CHARACTERISTIC(BT_UUID_GSS_GPS,
											  BT_GATT_CHRC_READ,
											  BT_GATT_PERM_READ, NULL, NULL,
											  NULL),
					   /* STEP 4 - Create and add the LED characteristic. */
					   BT_GATT_CHARACTERISTIC(BT_UUID_GSS_HUM,
											  BT_GATT_CHRC_WRITE,
											  BT_GATT_PERM_WRITE,
											  NULL, write_led, NULL),

);


/* CALLBACKS */

/* A function to register application callbacks */
int gss_init(struct gss_cb_s *callbacks)
{
	if (callbacks)
	{
		gss_cb.led_cb = callbacks->led_cb;
		gss_cb.button_cb = callbacks->button_cb;
	}

	return 0;
}

/* Write callback */
static ssize_t write_led(struct bt_conn *conn,
						 const struct bt_gatt_attr *attr,
						 const void *buf,
						 uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle,
			(void *)conn);

	if (len != 1U)
	{
		LOG_DBG("Write led: Incorrect data length");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0)
	{
		LOG_DBG("Write led: Incorrect data offset");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	if (gss_cb.led_cb)
	{
		// Read the received value
		uint8_t val = *((uint8_t *)buf);

		if (val == 0x00 || val == 0x01)
		{
			// Call the application callback function to update the LED state
			gss_cb.led_cb(val ? true : false);
		}
		else
		{
			LOG_DBG("Write led: Incorrect value");
			return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
		}
	}

	return len;
}