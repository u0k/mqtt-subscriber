#include "mqtt_event.h"
#include "json_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message_handler.h"
#include "mqtt_email.h"

void process_event_details(struct event *temp, char *email_body, char **recipient_email, int *seen_recipient,
			   char *sender_email, char *app_pw, char *topic);

/**
 * handle_event_report() - Handle the process of reporting events
 * @arg1: Topic of the message received
 * @arg2: Payload of the message received
 * @arg3: Linked list of events
 *
 * Return: Error code on failure, otherwise success code.
 */
int handle_event_report(char *topic, char *payload, struct event_list *events, char *sender_email,
			char *app_pw)
{
	int rc;
	int rt_email;
	struct event *temp    = events->head;
	char email_body[4096] = "";
	char *recipient_email = NULL;
	int event_triggered   = 0;
	int seen_recipient    = 0;
	while (temp != NULL) {
		if (strcmp(temp->topic, topic) == 0) {
			rc = parse_property_json(payload, temp->parameter, temp->value_type, temp->value,
						 temp->operator);
			if (rc == 1) {
				log_message(TRIGGERED_EVENT, temp->topic);
				event_triggered = 1;
				process_event_details(temp, email_body, &recipient_email, &seen_recipient,
						      sender_email, app_pw, topic);
			} else
				log_message(rc, "");
			temp = temp->next;
		} else {
			temp = temp->next;
		}
	}
	if (event_triggered && seen_recipient && recipient_email != NULL) {
		rt_email = send_email(sender_email, recipient_email, app_pw, topic, email_body);
		if (rt_email != 0) {
			log_message(rt_email, "");
		} else {
			log_message(EMAIL_SENT, recipient_email);
		}
	}

	return rc;
}

void process_event_details(struct event *temp, char *email_body, char **recipient_email, int *seen_recipient,
			   char *sender_email, char *app_pw, char *topic)
{
	int rt_email;
	if (*recipient_email == NULL || strcmp(*recipient_email, temp->recipient) != 0) {
		if (*seen_recipient && *recipient_email != NULL) {
			rt_email = send_email(sender_email, *recipient_email, app_pw, topic, email_body);
			if (rt_email != 0) {
				log_message(rt_email, "");
			} else {
				log_message(EMAIL_SENT, *recipient_email);
			}
		}
		strncpy(email_body, temp->topic, sizeof(email_body) - 1);
		email_body[sizeof(email_body) - 1] = '\0';
		strncat(email_body, temp->operator, sizeof(email_body) - strlen(email_body) - 1);
		strncat(email_body, "\t", sizeof(email_body) - strlen(email_body) - 1);
		strncat(email_body, temp->value, sizeof(email_body) - strlen(email_body) - 1);
		strncat(email_body, "\n", sizeof(email_body) - strlen(email_body) - 1);
		*recipient_email = temp->recipient;
		*seen_recipient	 = 1;
	} else {
		strncat(email_body, temp->topic, sizeof(email_body) - strlen(email_body) - 1);
		strncat(email_body, "\t", sizeof(email_body) - strlen(email_body) - 1);
		strncat(email_body, temp->operator, sizeof(email_body) - strlen(email_body) - 1);
		strncat(email_body, "\t", sizeof(email_body) - strlen(email_body) - 1);
		strncat(email_body, temp->value, sizeof(email_body) - strlen(email_body) - 1);
		strncat(email_body, "\n", sizeof(email_body) - strlen(email_body) - 1);
	}
}