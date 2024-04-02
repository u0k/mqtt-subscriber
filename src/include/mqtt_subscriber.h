#pragma once
#include <mosquitto.h>
#include <stdio.h>
#include "linked_list.h"
#include "config_handler.h"

void on_connect(struct mosquitto *mosq, struct all_lists *all_lists, int reason_code);
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos);
void on_message(struct mosquitto *mosq, struct all_lists *all_lists, const struct mosquitto_message *msg);
int setup_mosq(struct mosquitto **mosq, struct all_lists *all_lists, struct sub_config_settings sub_settings);