/*
 * mqttclient.h
 *
 *  Created on: Feb 9, 2020
 *      Author: 13080
 */

#ifndef MQTTCLIENT_MQTTCLIENT_H_
#define MQTTCLIENT_MQTTCLIENT_H_


void example_do_connect(mqtt_client_t *client);
void example_publish(mqtt_client_t *client, void *arg);

#endif /* MQTTCLIENT_MQTTCLIENT_H_ */
