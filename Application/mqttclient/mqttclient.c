/*
 * mqttclient.c
 *
 *  Created on: Feb 9, 2020
 *      Author: 13080
 */
#include "mqttclient.h"
#include "infra_config.h"
#ifdef DEPRECATED_LINKKIT
#include "solo.c"
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_compat.h"
#include "infra_compat.h"
#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
#endif
#include "dev_model_api.h"
#include "dm_wrapper.h"
#include "cJSON.h"
#ifdef ATM_ENABLED
    #include "at_api.h"
#endif
#include "dev_sign_api.h"
#include "mqtt_api.h"

void* HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
void HAL_Printf(const char *fmt, ...);
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1]);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1]);
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN]);
uint64_t HAL_UptimeMs(void);
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

char PRODUCT_KEY[IOTX_PRODUCT_KEY_LEN + 1] =
{ 0 };
char PRODUCT_SECRET[IOTX_PRODUCT_SECRET_LEN + 1] =
{ 0 };
char DEVICE_NAME[IOTX_DEVICE_NAME_LEN + 1] =
{ 0 };
char DEVICE_SECRET[IOTX_DEVICE_SECRET_LEN + 1] =
{ 0 };

typedef struct
{
	int master_devid;
	int cloud_connected;
	int master_initialized;
} user_ctx_t;

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        /*HAL_Printf("%s|%03d :: ", __func__, __LINE__);*/ \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\n"); \
    } while(0)

void Ali_message_arrive(void *pcontext, void *pclient,
		iotx_mqtt_event_msg_pt msg)
{
	iotx_mqtt_topic_info_t *topic_info = (iotx_mqtt_topic_info_pt) msg->msg;

	switch (msg->event_type)
	{
	case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
		/* print topic name and topic message */
		EXAMPLE_TRACE("Message Arrived:");
		EXAMPLE_TRACE("Topic  : %.*s", topic_info->topic_len,
				topic_info->ptopic);
		EXAMPLE_TRACE("Payload: %.*s", topic_info->payload_len,
				topic_info->payload);
		EXAMPLE_TRACE("\n");
		break;
	default:
		break;
	}
}

int Ali_subscribe(void *handle)
{
	int res = 0;
	const char *fmt = "/%s/%s/user/inctest";
	char *topic = NULL;
	int topic_len = 0;

	topic_len = strlen(fmt) + strlen(PRODUCT_KEY) + strlen(DEVICE_NAME) + 1;
	topic = HAL_Malloc(topic_len);
	if (topic == NULL)
	{
		EXAMPLE_TRACE("memory not enough");
		return -1;
	}
	memset(topic, 0, topic_len);
	HAL_Snprintf(topic, topic_len, fmt, PRODUCT_KEY, DEVICE_NAME);

	res = IOT_MQTT_Subscribe(handle, topic, IOTX_MQTT_QOS0, Ali_message_arrive,
	NULL);
	if (res < 0)
	{
		EXAMPLE_TRACE("subscribe failed");
		HAL_Free(topic);
		return -1;
	}

	HAL_Free(topic);
	return 0;
}

int Ali_publish(void *handle)
{
	int res = 0;
	const char *fmt = "/%s/%s/user/pub_test";
	char *topic = NULL;
	int topic_len = 0;
	char *payload = "\"message\":\"Ali,hello!\"";

	topic_len = strlen(fmt) + strlen(PRODUCT_KEY) + strlen(DEVICE_NAME) + 1;
	topic = HAL_Malloc(topic_len);
	if (topic == NULL)
	{
		EXAMPLE_TRACE("memory not enough");
		return -1;
	}
	memset(topic, 0, topic_len);
	HAL_Snprintf(topic, topic_len, fmt, PRODUCT_KEY, DEVICE_NAME);

	res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload,
			strlen(payload));
	if (res < 0)
	{
		EXAMPLE_TRACE("publish failed, res = %d", res);
		HAL_Free(topic);
		return -1;
	}

	HAL_Free(topic);
	return 0;
}

void Ali_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
	switch(msg->event_type)
	{
	case IOTX_MQTT_EVENT_UNDEF:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_UNDEF");
		break;
	case IOTX_MQTT_EVENT_DISCONNECT:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_DISCONNECT");
		break;
	case IOTX_MQTT_EVENT_RECONNECT:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_RECONNECT");
		break;
	case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS");
		break;
	case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT");
		break;
	case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_SUBCRIBE_NACK");
		break;
	case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS");
		break;
	case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT");
		break;
	case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_UNSUBCRIBE_NACK");
		break;
	case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_PUBLISH_SUCCESS");
		break;
	case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_PUBLISH_TIMEOUT");
		break;
	case IOTX_MQTT_EVENT_PUBLISH_NACK:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_PUBLISH_NACK");
		break;
	case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_PUBLISH_RECEIVED");
		break;
	case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
		EXAMPLE_TRACE("\nmsg->event_type : %s", "IOTX_MQTT_EVENT_BUFFER_OVERFLOW");
		break;
	}
}

void AliCloud_thread(void const *argument)
{
	iotx_mqtt_param_t mqtt_params;
	void *pclient = NULL;
	int res = 0;
	int loop_cnt = 0;

	HAL_GetProductKey(PRODUCT_KEY);
	HAL_GetDeviceName(DEVICE_NAME);
	HAL_GetDeviceSecret(DEVICE_SECRET);

	memset(&mqtt_params, 0, sizeof(iotx_mqtt_param_t));
	mqtt_params.handle_event.h_fp = Ali_event_handle;

	do
	{
		pclient = IOT_MQTT_Construct(&mqtt_params);
	} while (NULL == pclient);

	res = Ali_subscribe(pclient);
	if (res < 0)
	{
		IOT_MQTT_Destroy(&pclient);
		return;
	}
	while (1)
	{
		if (0 == loop_cnt % 20)
		{
			Ali_publish(pclient);
		}
		IOT_MQTT_Yield(pclient, 200);
		loop_cnt++;
	}
}
#endif
