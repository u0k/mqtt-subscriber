#include "mqtt_subscriber.h"
#include "mqtt_database.h"
#include "message_handler.h"
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include "config_handler.h"
#include <linked_list.h>
#include <uci.h>
#include <daemon.h>
#include <unistd.h>
#include <string.h>
#include "argp_handler.h"

void signal_handler(int signum);
int running	    = 1;
const int max_recon = 5;

int main(int argc, char *argv[])
{
	int rt_open;
	int rt_create_table;
	int rt_load_config;
	int rt_set_all_settings;
	int rt_daemon;
	int rt_mosq;
	struct all_lists all_lists;
	const char *config_name		= "mqtt_subscriber";
	struct mosquitto *mosq		= NULL;
	struct uci_context *context	= NULL;
	struct uci_package *package	= NULL;
	all_lists.sub_settings.app_pw	= NULL;
	all_lists.sub_settings.cafile	= NULL;
	all_lists.sub_settings.certfile = NULL;
	all_lists.sub_settings.enabled	= NULL;
	all_lists.sub_settings.host	= NULL;
	all_lists.sub_settings.keyfile	= NULL;
	all_lists.sub_settings.password = NULL;
	all_lists.sub_settings.port	= NULL;
	all_lists.sub_settings.sender	= NULL;
	all_lists.sub_settings.username = NULL;

	struct argp_option options[] = {
		{ "host", 'h', "HOST", 0, "Host or IP" },
		{ "port", 'p', "PORT", 0, "Port number" },
		{ "user", 'u', "USER", 0, "Username for auth" },
		{ "password", 's', "PASSWORD", 0, "Password for auth, use with user" },
		{ "cafile", 'c', "CAFILE", 0,
		  "Path to a file containing the PEM encoded trusted CA certificate files" },
		{ "certfile", 'x', "CERTFILE", 0,
		  "Path to a file containing the PEM encoded certificate file for this client" },
		{ "keyfile", 'k', "KEYFILE", 0,
		  "Path to a file containing the PEM encoded private key for this client." },
		{ "email", 'o', "SENDER", 0, "Email address that will be used to send emails" },
		{ "app_pw", 'i', "PASSWORD", 0, "Email access token (currently gmail app password)" },
		{ "enabled", 'D', 0, OPTION_ARG_OPTIONAL, "Option to run program as daemon" },
		{ 0 }
	};
	struct argp argp = { options, parse_opt, 0, 0 };
	argp_parse(&argp, argc, argv, 0, 0, &(all_lists.sub_settings));
	openlog("mqtt-subscriber", LOG_PID, LOG_USER);

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGTSTP, signal_handler);

	rt_load_config = load_config(config_name, &context, &package);
	if (rt_load_config) {
		log_message(rt_load_config, "");
		goto cleanup;
	}

	all_lists.topics = create_list();
	if (all_lists.topics == NULL) {
		log_message(ERROR_CREATING_LIST, "topics");
		goto cleanup;
	}
	all_lists.events = create_event_list();
	if (all_lists.events == NULL) {
		log_message(ERROR_CREATING_LIST, "events");
		goto cleanup;
	}
	rt_set_all_settings =
		set_all_configs(package, context, all_lists.sub_settings, all_lists.events, all_lists.topics);
	if (rt_set_all_settings < 0) {
		log_message(rt_set_all_settings, "");
		if (rt_set_all_settings != ERROR_OPTION_NOT_FOUND)
			goto cleanup;
	}
	rt_daemon = setup_daemon(all_lists.sub_settings);
	if (rt_daemon) {
		log_message(ERROR_DAEMON_START, "");
		goto cleanup;
	}
	rt_mosq = setup_mosq(&mosq, &all_lists, all_lists.sub_settings);
	if (rt_mosq < 0) {
		log_message(ERROR_MOSQ_SETUP, "");
		goto cleanup;
	}
	rt_open = open_db();
	if (rt_open) {
		log_message(rt_open, "");
		goto cleanup;
	}

	rt_create_table = create_table();
	if (rt_create_table) {
		log_message(rt_create_table, "");
		goto cleanup;
	}
	while (running) {
		int rt_loop	 = mosquitto_loop(mosq, -1, 1);
		int recon_amount = 0;
		if (rt_loop == MOSQ_ERR_CONN_LOST) {
			while (mosquitto_reconnect(mosq) != MOSQ_ERR_SUCCESS && recon_amount < max_recon) {
				log_message(ERROR_MOSQ_RECONNECT, "");
				recon_amount++;
				sleep(5);
			}
		} else if (rt_loop != MOSQ_ERR_SUCCESS) {
			log_message(ERROR_MOSQ_LOOP, "");
			goto cleanup;
		}
	}

cleanup:
	log_message(CLEANUP, "");
	closelog();
	if (context != NULL)
		uci_free_context(context);
	close_db();
	if (mosq != NULL) {
		mosquitto_disconnect(mosq);
		mosquitto_destroy(mosq);
	}
	free_all_lists(&all_lists);

	return 0;
}

/**
 * signal_handler() - Handler for processing signals.
 * @arg1: An integer that specifies signal's number
 *
 * On called signal changes global variable to not running.
 *
 */
void signal_handler(int signum)
{
	running = 0;
}