#ifndef BT_LBS_H_
#define BT_LBS_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <zephyr/types.h>

/* STEP 1 - Define the 128 bit UUIDs for the GATT service and its characteristics in */
#define BT_UUID_GSS_VAL \
	BT_UUID_128_ENCODE(0x6dea7370, 0x0bdf, 0x43db, 0x8ea8, 0x1c3a6a7e5909)

/** @brief LED Characteristic UUID. */
#define BT_UUID_GSS_GPS_VAL \
	BT_UUID_128_ENCODE(0x6dea7371, 0x0bdf, 0x43db, 0x8ea8, 0x1c3a6a7e5909)

/** @brief Button Characteristic UUID. */
#define BT_UUID_GSS_SENSOR_VAL \
	BT_UUID_128_ENCODE(0x6dea7372, 0x0bdf, 0x43db, 0x8ea8, 0x1c3a6a7e5909)

#define BT_UUID_GSS BT_UUID_DECLARE_128(BT_UUID_GSS_VAL)
#define BT_UUID_GSS_GPS BT_UUID_DECLARE_128(BT_UUID_GSS_GPS_VAL)
#define BT_UUID_GSS_SENSOR BT_UUID_DECLARE_128(BT_UUID_GSS_SENSOR_VAL)

	/** @brief Callback type for when an LED state change is received. */
	typedef void (*led_cb_t)(const bool led_state);

	/** @brief Callback type for when the button state is pulled. */
	typedef bool (*button_cb_t)(void);

	/** @brief Callback struct used by the LBS Service. */
	struct my_lbs_cb
	{
		/** LED state change callback. */
		led_cb_t led_cb;
		/** Button read callback. */
		button_cb_t button_cb;
	};

	/** @brief Initialize the LBS Service.
	 *
	 * This function registers application callback functions with the My LBS
	 * Service
	 *
	 * @param[in] callbacks Struct containing pointers to callback functions
	 *			used by the service. This pointer can be NULL
	 *			if no callback functions are defined.
	 *
	 *
	 * @retval 0 If the operation was successful.
	 *           Otherwise, a (negative) error code is returned.
	 */
	int my_lbs_init(struct my_lbs_cb *callbacks);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* BT_LBS_H_ */
