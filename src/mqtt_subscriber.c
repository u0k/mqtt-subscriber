#include "mqtt_subscriber.h"
#include "mqtt_database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "mqtt_event.h"
#include <errno.h>
#include <message_handler.h>

/**
 * on_connect() - MQTT connection callback function
 * @arg1: A valid mosquitto instance
 * @arg2: Both event and topic lists
 * @arg3: The return code of the connection response.
 *
 */
void on_connect(struct mosquitto *mosq, struct all_lists *all_lists, int reason_code)
{
	char *end_ptr;
	long qos_value;
	log_message(MQTT_CONNECTED, "");
	int rc;
	if (reason_code != 0) {
		mosquitto_disconnect(mosq);
	}
	struct topic *temp = all_lists->topics->head;
	while (temp != NULL) {
		errno	  = 0;
		qos_value = strtol(temp->qos, &end_ptr, 10);
		if (errno != 0 && qos_value == 0) {
			log_message(ERROR_SUBSCRIBING, temp->topic);
			mosquitto_disconnect(mosq);
		}
		rc = mosquitto_subscribe(mosq, NULL, temp->topic, (int)qos_value);
		if (rc != MOSQ_ERR_SUCCESS) {
			log_message(ERROR_SUBSCRIBING, temp->topic);
			mosquitto_disconnect(mosq);
		} else
			log_message(MQTT_SUBSCRIBED, temp->topic);
		temp = temp->next;
	}
}

/**
 * on_subscribe() - MQTT subscription callback function
 * @arg1: The mosquitto instance making the callback
 * @arg2: User data
 * @arg3: The message id of the subscribe message
 * @arg4: The number of granted subscriptions
 * @arg5: aAn array of integers indicating the granted QoS for each of the subscriptions.
 *
 */
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
	int i;
	bool have_subscription = false;

	for (i = 0; i < qos_count; i++) {
		if (granted_qos[i] <= 2) {
			have_subscription = true;
		}
	}
	if (have_subscription == false) {
		mosquitto_disconnect(mosq);
	}
}

/**
 * on_message() - MQTT message callback function
 * @arg1: The mosquitto instance making the callback
 * @arg2: User data
 * @arg3: The message data
 *
 */
void on_message(struct mosquitto *mosq, struct all_lists *all_lists, const struct mosquitto_message *msg)
{
	int insert_rt;
	int timestamp;
	if (msg->payloadlen == 0) {
		return;
	}
	log_message(GOT_MESSAGE, msg->payload);

	timestamp = (int)time(NULL);
	insert_rt = insert_message(msg->topic, msg->payload, timestamp);
	if (insert_rt) {
		log_message(insert_rt, "");
		return;
	}
	handle_event_report(msg->topic, msg->payload, all_lists->events, all_lists->sub_settings.sender,
			    all_lists->sub_settings.app_pw);
}

/**
 * setup_mosq() - Set up the mosquitto subscriber
 * @arg1: A valid mosquitto instance
 * @arg2: Both event ant topic lists
 * @arg3: Settings structure
 *
 * Return: Error code on failure, otherwise 0.
 */
int setup_mosq(struct mosquitto **mosq, struct all_lists *all_lists, struct sub_config_settings sub_settings)
{
	int rt_set_userpw;
	int rt_set_tls;
	int rt_conn;
	char *end_ptr;
	long port_value;
	mosquitto_lib_init();
	*mosq = mosquitto_new(NULL, true, all_lists);
	if (*mosq == NULL) {
		log_message(ERROR_MOSQ_OUT_OF_MEMORY, "");
		return ERROR_MOSQ_OUT_OF_MEMORY;
	}

	mosquitto_connect_callback_set(*mosq, on_connect);
	mosquitto_subscribe_callback_set(*mosq, on_subscribe);
	mosquitto_message_callback_set(*mosq, on_message);

	if (sub_settings.username != NULL && sub_settings.password != NULL) {
		rt_set_userpw =
			mosquitto_username_pw_set(*mosq, sub_settings.username, sub_settings.password);
		if (rt_set_userpw != MOSQ_ERR_SUCCESS) {
			log_message(ERROR_MOSQ_AUTH, "");
			return ERROR_MOSQ_AUTH;
		}
	}
	if (sub_settings.cafile != NULL && sub_settings.certfile != NULL && sub_settings.keyfile != NULL) {
		rt_set_tls = mosquitto_tls_set(*mosq, sub_settings.cafile, NULL, sub_settings.certfile,
					       sub_settings.keyfile, NULL);
		if (rt_set_tls != MOSQ_ERR_SUCCESS) {
			log_message(ERROR_MOSQ_TLS, "");
			return ERROR_MOSQ_TLS;
		}
	}

	errno	   = 0;
	port_value = strtol(sub_settings.port, &end_ptr, 10);
	if (errno != 0 && port_value == 0) {
		log_message(ERROR_MOSQ_CONNECTION, "");
		return ERROR_MOSQ_CONNECTION;
	}
	rt_conn = mosquitto_connect(*mosq, sub_settings.host, (int)port_value, 60);
	if (rt_conn != MOSQ_ERR_SUCCESS) {
		log_message(ERROR_MOSQ_CONNECTION, "");
		return ERROR_MOSQ_CONNECTION;
	}

	return 0;
}
