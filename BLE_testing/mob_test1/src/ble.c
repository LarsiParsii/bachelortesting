#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/conn.h>

#include "ble.h"
#include "gss.h"

/* ADVERTISING PARAMETERS */
/* Connectable advertising and use identity address */
static struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM((BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_IDENTITY),
														   800,	  /*Min Advertising Interval 500ms (800*0.625ms) */
														   801,	  /*Max Advertising Interval 500.625ms (801*0.625ms)*/
														   NULL); /* Set to NULL for undirected advertising*/

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, CONFIG_BT_DIS_MODEL, MODEL_LEN),
};

static const struct bt_data sd[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_GSS_VAL),
};

int ble_init(struct bt_conn_cb *connection_cb)
{
	int err;
	err = bt_enable(NULL);
	if (err)
	{
		LOG_ERR("Bluetooth init failed (err %d)\n", err);
		return err;
	}
	bt_conn_cb_register(connection_cb);
	
	LOG_INF("Bluetooth initialized\n");
	err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad),
						  sd, ARRAY_SIZE(sd));
	if (err)
	{
		LOG_ERR("Advertising failed to start (err %d)\n", err);
		return;
	}

	LOG_INF("Advertising successfully started\n");
}