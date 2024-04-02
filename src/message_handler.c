#include "message_handler.h"
#include <stdio.h>
#include <syslog.h>

/**
 * log_message() - Log a message.
 * @arg1: Integer which provides a number that maps to specific message
 *
 * Logs a message to a file, according to provided message code.
 *
 */
void log_message(int message_code, char *additional_msg)
{
	switch (message_code) {
	case ERROR_SQLITE_DB_OPEN:
		syslog(LOG_ERR, "[ERROR] SQLITE FAILED TO OPEN DATABASE");
		break;
	case ERROR_SQLITE_DB_CLOSE:
		syslog(LOG_ERR, "[ERROR] SQLITE FAILED TO CLOSE DATABASE");
		break;
	case ERROR_SQLITE_TABLE_CREATE:
		syslog(LOG_ERR, "[ERROR] TABLE COULDN'T BE CREATED");
		break;
	case ERROR_SQLITE_INSERT_MESSAGE:
		syslog(LOG_ERR, "[ERROR] MESSAGE COULD NOT BE INSERTED");
		break;
	case ERROR_LOADING_UCI_CONFIG:
		syslog(LOG_ERR, "[ERROR] COULD NOT LOAD UCI CONFIG");
		break;
	case ERROR_CONFIG_SET_SETTINGS:
		syslog(LOG_ERR, "[ERROR] COULD NOT SAVE SETTINGS TO CONFIG");
		break;
	case ERROR_CONFIG_SET_EVENT:
		syslog(LOG_ERR, "[ERROR] COULD NOT GET ALL OPTIONS FROM EVENT SECTION");
		break;
	case ERROR_CONFIG_SET_TOPIC:
		syslog(LOG_ERR, "[ERROR] COULD NOT GET ALL OPTION FROM TOPIC SECTION");
		break;
	case ERROR_OPTION_NOT_FOUND:
		syslog(LOG_ERR, "[ERROR] LOOKED UP OPTION IS NOT FOUND");
		break;
	case ERROR_CREATING_LIST:
		syslog(LOG_ERR, "[ERROR] COULD NOT CREATE LIST FOR %s", additional_msg);
		break;
	case ERROR_SETTING_OPTION:
		syslog(LOG_ERR, "[ERROR] COULD NOT SET OPTION");
		break;
	case ERROR_MOSQ_OUT_OF_MEMORY:
		syslog(LOG_ERR, "[ERROR] COULDN'T CREATE AN INSTANCE OF MOSQ (OUT OF MEMORY)");
		break;
	case ERROR_MOSQ_AUTH:
		syslog(LOG_ERR, "[ERROR] COULDN'T SET UP MOSQ USER AUTHENTICATION");
		break;
	case ERROR_MOSQ_TLS:
		syslog(LOG_ERR, "[ERROR] COULDN'T SET UP MOSQ TLS");
		break;
	case ERROR_MOSQ_CONNECTION:
		syslog(LOG_ERR, "[ERROR] COULD NOT CONNECT TO MQTT BROKER");
		break;
	case ERROR_MOSQ_LOOP:
		syslog(LOG_ERR, "[ERROR] ERROR DURING MOSQ LOOP");
		break;
	case ERROR_MOSQ_SETUP:
		syslog(LOG_ERR, "[ERROR] ERROR IN MOSQ SETUP");
		break;
	case ERROR_JSON_OBJECT_NULL:
		syslog(LOG_ERR, "[ERROR] JSON OBJECT DOESN'T EXIST");
		break;
	case ERROR_JSON_PARAMETER_NOT_FOUND:
		syslog(LOG_ERR, "[ERROR] JSON PARAMETER NOT FOUND");
		break;
	case ERROR_UNKNOWN_VALUE_TYPE:
		syslog(LOG_ERR, "[ERROR] JSON HAS UNKNOWN VALUE TYPE");
		break;
	case ERROR_NUMERIC_OPERATION:
		syslog(LOG_ERR, "[ERROR] NUMERIC OPERATION FAILED");
		break;
	case ERROR_ALPHANUMERIC_OPERATION:
		syslog(LOG_ERR, "[ERROR] ALPHANUMERIC OPERATION FAILED");
		break;
	case ERROR_MALLOC:
		syslog(LOG_ERR, "[ERROR] MEMORY ALLOCATION FAILED");
		break;
	case ERROR_ALLOCATING_PAYLOAD:
		syslog(LOG_ERR, "[ERROR] ALLOCATING EMAIL PAYLOAD FAILED");
		break;
	case ERROR_CURL_INIT:
		syslog(LOG_ERR, "[ERROR] CURL COULD NOT BE INITIALIZED");
		break;
	case ERROR_SEND_EMAIL:
		syslog(LOG_ERR, "[ERROR] EMAIL COULD NOT BE SENT");
		break;
	case ERROR_SUBSCRIBING:
		syslog(LOG_ERR, "[ERROR] COULD NOT SUBSCRIBE TO %s", additional_msg);
		break;
	case GOT_MESSAGE:
		syslog(LOG_INFO, "[INFO] GOT MESSAGE %s", additional_msg);
		break;
	case ERROR_JSON_PARSING:
		syslog(LOG_ERR, "[ERROR] INCOMING JSON PAYLOAD COULDN'T BE PARSED %s", additional_msg);
		break;
	case ERROR_DAEMON_START:
		syslog(LOG_ERR, "[ERROR] DAEMON COULDN'T BE STARTED");
		break;
	case ERROR_MOSQ_RECONNECT:
		syslog(LOG_ERR, "[ERROR] CONNECTION FAILED TRYING TO RECONNECT");
		break;
	case TRIGGERED_EVENT:
		syslog(LOG_INFO, "[INFO] TRIGGERED EVENT FOR TOPIC %s", additional_msg);
		break;
	case EMAIL_SENT:
		syslog(LOG_INFO, "[INFO] EMAIL HAS BEEN SENT TO %s", additional_msg);
		break;
	case MQTT_CONNECTED:
		syslog(LOG_INFO, "[INFO] MQTT CONNECTED");
		break;
	case MQTT_SUBSCRIBED:
		syslog(LOG_INFO, "[INFO] SUBSCRIBED TO TOPIC %s", additional_msg);
	case EXITING:
		syslog(LOG_INFO, "[INFO] Exiting");
		break;
	case CLEANUP:
		syslog(LOG_INFO, "[INFO] CLEANING UP");
		break;
	default:
		break;
	}
}