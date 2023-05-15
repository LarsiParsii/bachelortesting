#include "gnss.h"
#include <stdio.h>
#include <nrf_modem_gnss.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(GNSS, LOG_LEVEL_INF);

/* SEMAPHORES */
K_SEM_DEFINE(gnss_fix_sem, 0, 1);

/* VARIABLES */
extern enum tracker_status device_status;

/* FUNCTION DEFINITIONS */
void print_fix_data(struct nrf_modem_gnss_pvt_data_frame *pvt_data)
{
	printk("Latitude:       %.06f\n", pvt_data->latitude);
	printk("Longitude:      %.06f\n", pvt_data->longitude);
	printk("Altitude:       %.01f m\n", pvt_data->altitude);
	printk("Time (UTC):     %02u:%02u:%02u.%03u\n",
		   pvt_data->datetime.hour,
		   pvt_data->datetime.minute,
		   pvt_data->datetime.seconds,
		   pvt_data->datetime.ms);
}

static void gnss_event_handler(int event)
{
	int retval;
	switch (event)
	{
	case NRF_MODEM_GNSS_EVT_PVT:
		LOG_INF("Searching for GNSS Satellites....\n\r");
		device_status = status_searching;
		break;
	case NRF_MODEM_GNSS_EVT_FIX:
		LOG_INF("GNSS fix event\n\r");
		break;
	case NRF_MODEM_GNSS_EVT_PERIODIC_WAKEUP:
		LOG_INF("GNSS woke up in periodic mode\n\r");
		break;
	case NRF_MODEM_GNSS_EVT_BLOCKED:
		LOG_INF("GNSS is blocked by LTE event\n\r");
		break;
	case NRF_MODEM_GNSS_EVT_SLEEP_AFTER_FIX:
		LOG_INF("GNSS enters sleep because fix was achieved in periodic mode\n\r");
		device_status = status_fixed;
		retval = nrf_modem_gnss_read(&last_pvt, sizeof(last_pvt), NRF_MODEM_GNSS_DATA_PVT);
		if (retval == 0)
		{
			current_pvt = last_pvt;
			print_fix_data(&current_pvt);
			k_sem_give(&gnss_fix_sem);
		}
		break;
	case NRF_MODEM_GNSS_EVT_SLEEP_AFTER_TIMEOUT:
		LOG_INF("GNSS enters sleep because fix retry timeout was reached\n\r");
		break;

	default:

		break;
	}
}

int gnss_init_and_start(void)
{
#if defined(CONFIG_GNSS_HIGH_ACCURACY_TIMING_SOURCE)
	if (nrf_modem_gnss_timing_source_set(NRF_MODEM_GNSS_TIMING_SOURCE_TCXO))
	{
		LOG_ERR("Failed to set TCXO timing source");
		return -1;
	}
#endif
#if defined(CONFIG_GNSS_LOW_ACCURACY) || defined(CONFIG_BOARD_THINGY91_NRF9160_NS)
	uint8_t use_case;
	use_case = NRF_MODEM_GNSS_USE_CASE_MULTIPLE_HOT_START | NRF_MODEM_GNSS_USE_CASE_LOW_ACCURACY;
	if (nrf_modem_gnss_use_case_set(use_case) != 0)
	{
		LOG_ERR("Failed to set low accuracy use case");
		return -1;
	}
#endif
	/* Configure GNSS event handler . */
	if (nrf_modem_gnss_event_handler_set(gnss_event_handler) != 0)
	{
		LOG_ERR("Failed to set GNSS event handler");
		return -1;
	}

	if (nrf_modem_gnss_fix_interval_set(CONFIG_TRACKER_PERIODIC_INTERVAL) != 0)
	{
		LOG_ERR("Failed to set GNSS fix interval");
		return -1;
	}

	if (nrf_modem_gnss_fix_retry_set(CONFIG_TRACKER_PERIODIC_TIMEOUT) != 0)
	{
		LOG_ERR("Failed to set GNSS fix retry");
		return -1;
	}

	if (nrf_modem_gnss_start() != 0)
	{
		LOG_ERR("Failed to start GNSS");
		return -1;
	}
	if (nrf_modem_gnss_prio_mode_enable() != 0)
	{
		LOG_ERR("Error setting GNSS priority mode");
		return -1;
	}
	return 0;
}
