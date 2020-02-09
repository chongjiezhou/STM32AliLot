/*
 * mqttclient.c
 *
 *  Created on: Feb 9, 2020
 *      Author: 13080
 */
#include "mqttclient.h"
#include "lwip.h"
#include <mqtt.h>
#include "main.h"
#include <string.h>

ip_addr_t mqttSeverAddr;
#define Sever_IP_ADDR0	192
#define Sever_IP_ADDR1	168
#define Sever_IP_ADDR2	192
#define Sever_IP_ADDR3	168

static void mqtt_connection_cb(mqtt_client_t *client, void *arg,
		mqtt_connection_status_t status);
static void mqtt_sub_request_cb(void *arg, err_t result);
static void mqtt_incoming_publish_cb(void *arg, const char *topic,
		u32_t tot_len);
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
		u8_t flags);
static void mqtt_pub_request_cb(void *arg, err_t result);
/*
 * 与服务器建立连接
 */
void example_do_connect(mqtt_client_t *client)
{
	struct mqtt_connect_client_info_t ci;
	err_t err;
	/* 设置一个空的客户端信息结构 */
	memset(&ci, 0, sizeof(ci));
	/* 最少的信息量作为客户标识符 */
	ci.client_id = "lwip_test";
	IP4_ADDR(&mqttSeverAddr, Sever_IP_ADDR0, Sever_IP_ADDR1, Sever_IP_ADDR2,
			Sever_IP_ADDR3);
	/* 启动客户端并连接到服务器 */
	err = mqtt_client_connect(client, &mqttSeverAddr, MQTT_PORT,
			mqtt_connection_cb, 0, &ci);

	if (err != ERR_OK)
	{
		printf("mqtt_connect return %d\n", err);
	}
}

/*
 * 连接状态回调函数
 */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg,
		mqtt_connection_status_t status)
{
	err_t err;

	if (status == MQTT_CONNECT_ACCEPTED)
	{
		printf("mqtt_connection_cb: Successfully connected\n");

		/* 为传入的参数设置回调 */
		mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb,
				mqtt_incoming_data_cb, arg);

		/* 订阅名为"subtopic" 的Qos级别为1的主题，调用mqtt_sub_request_cb并显示结果 */
		err = mqtt_subscribe(client, "subtopic", 1, mqtt_sub_request_cb, arg);

		if (err != ERR_OK)
		{
			printf("mqtt_subscribe return:%d\n", err);
		}
	}
	else
	{
		printf("mqtt_connection_cb:Disconnected,reason: %d\n", status);
		/* 重新连接 */
		example_do_connect(client);
	}
}

/*
 * 订阅失败回调函数
 */
static void mqtt_sub_request_cb(void *arg, err_t result)
{
	/* 为了简单起见，只需在此处打印结果代码，
	 * 如果订阅失败，正常的行为是采取一些措施，例如
	 * 通知用户，重试订阅或与服务器断开连接 */
	printf("Subscribe result: %d\n", result);
}

static int inpub_id;
/*
 * 接收到订阅回调函数
 */
static void mqtt_incoming_publish_cb(void *arg, const char *topic,
		u32_t tot_len)
{
	printf("Incoming publish at topic \" %s \" with total length %u\r\n", topic,
			(unsigned int) tot_len);

	/* Decode topic string into a user defined reference */
	if (strcmp(topic, "subtopic") == 0)
	{
		inpub_id = 0;
	}
	else if (topic[0] == 'A')
	{
		/* All topics starting with 'A' might be handled at the same way */
		inpub_id = 1;
	}
	else
	{
		/* For all other topics */
		inpub_id = 2;
	}
}

/*
 * 接收到数据回调函数
 */
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
		u8_t flags)
{
	u8_t tempBuff[20] =
	{ 0 };
	u8_t i;
	/* 对接收的数据进行拷贝 */
	for (i = 0; i < len; i++)
	{
		tempBuff[i] = data[i];
	}
	tempBuff[i] = 0;

	printf("Incoming publish payload with length %d,flags %u\n", len,
			(unsigned int) flags);

	if (flags & MQTT_DATA_FLAG_LAST)
	{
		if (inpub_id == 0)
		{
			/* Don't trust the publisher, check zero termination */
			if (tempBuff[len] == 0)
			{
				printf("mqtt_incoming_data_cb: %s\n", (const char*) data);
			}
		}
		else if (inpub_id == 1)
		{
			/* Call an 'A' function... */
		}
		else
		{
			printf("mqtt_incoming_data_cb: Ignoring payload...\n");
		}
	}
	else
	{

	}
}

/*
 * 向服务器发布信息
 */
void example_publish(mqtt_client_t *client, void *arg)
{
	const char *pub_payload = "Hello,MQTT\r\n";
	err_t err;
	u8_t qos = 1;
	u8_t retain = 0;

	err = mqtt_publish(client, "pub_topic", pub_payload, strlen(pub_payload),
			qos, retain, mqtt_pub_request_cb, arg);
	if (err != ERR_OK)
	{
		printf("Publish err: %d.\r\n", err);
	}
	else
	{
		printf("Publish Success.\r\n");
	}
}

/*
 * 发布结果回调函数
 */
static void mqtt_pub_request_cb(void *arg, err_t result)
{
	if (result != ERR_OK)
	{
		printf("Publish result: %d\n", result);
	}
}

void StartTaskTcpClient(void const *argument)
{
	/* USER CODE BEGIN StartTaskTcp */
	struct netconn *conn;
	int ret;
	ip4_addr_t ipaddr;
	ip4_addr_t ipaddr2;
	uint8_t DEST_IP_ADDR0 = 192;
	uint8_t DEST_IP_ADDR1 = 168;
	uint8_t DEST_IP_ADDR2 = 1;
	uint8_t DEST_IP_ADDR3 = 6;
	uint16_t DEST_PORT = 8080;
	uint16_t LOCAL_PORT;

	uint8_t send_buf[] = "This is a TCP Client test...\r\n";

	while (DHCP_state != DHCP_ADDRESS_ASSIGNED )
		;

	printf("DEST IP:%d.%d.%d.%d \t PORT:%d \n\n", DEST_IP_ADDR0, DEST_IP_ADDR1,
			DEST_IP_ADDR2, DEST_IP_ADDR3, DEST_PORT);

	while (1)
	{
		conn = netconn_new(NETCONN_TCP);
		if (conn == NULL)
		{
			printf("creat conn failed!\n");
			osDelay(10);
			continue;
		}
		IP4_ADDR(&ipaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2,
				DEST_IP_ADDR3);
		ret = netconn_connect(conn, &ipaddr, DEST_PORT);

		if (ret == -1)
		{
			printf("Connect failed!\n");
			netconn_close(conn);
			continue;
		}
		printf("Connect successful!\n");
		netconn_getaddr(conn, &ipaddr2, &LOCAL_PORT, 1);
		while (1)
		{
			ret = netconn_write(conn, send_buf, sizeof(send_buf), 0);
			osDelay(1000);
		}
	}
	/* USER CODE END StartTaskTcp */
}
void StartTaskTcpServer(void const *argument)
{
	struct netconn *conn, *newconn;
	err_t err;
	LWIP_UNUSED_ARG(argument);

	while (DHCP_state != DHCP_ADDRESS_ASSIGNED )
		;
#if LWIP_IPV6
	conn = netconn_new(NETCONN_TCP_IPV6);
	netconn_bind(conn,IP6_ADDR_ANY,5001);
#else
	conn = netconn_new(NETCONN_TCP);
	netconn_bind(conn, IP_ADDR_ANY, 5001);
#endif
	LWIP_ERROR("tcpecho: invalid conn", (conn != NULL), return;);

	netconn_listen(conn);
	while (1)
	{
		err = netconn_accept(conn, &newconn);
		if (err == ERR_OK)
		{
			struct netbuf *buf;
			void *data;
			u16_t len;
			while ((err = netconn_recv(newconn, &buf)) == ERR_OK)
			{
				do
				{
					netbuf_data(buf, &data, &len);
					err = netconn_write(newconn, data, len, NETCONN_COPY);
					if (err != ERR_OK)
					{
						printf("tcpecho: netconn write:error \"%s\n",
								lwip_strerr(err));
					}
				} while (netbuf_next(buf) >= 0);
				netbuf_delete(buf);
			}
			netconn_close(newconn);
			netconn_delete(newconn);
		}
	}
}
