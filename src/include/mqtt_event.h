#pragma once
#include "linked_list.h"
int handle_event_report(char *topic, char *payload, struct event_list *events, char *sender_email,
			char *app_pw);