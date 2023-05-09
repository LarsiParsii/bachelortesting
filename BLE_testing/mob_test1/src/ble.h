#ifndef ble_H
#define ble_H

// Includes
#include <zephyr/bluetooth/conn.h>

// Defines
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define MODEL CONFIG_BT_DIS_MODEL
#define MODEL_LEN (sizeof(MODEL) - 1)

// Prototypes
int ble_init(struct bt_conn_cb *bt_connection_cb);

#endif  // ble_H