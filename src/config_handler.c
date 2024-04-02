#include "config_handler.h"
#include "message_handler.h"
#include <string.h>

int handle_settings(struct uci_context *context, struct uci_section *section,
		    struct sub_config_settings sub_settings);
int handle_event(struct uci_context *context, struct uci_section *section, struct event_list *events);
int handle_topic(struct uci_context *context, struct uci_section *section, struct topic_list *topics);
int uci_set_option(char *pkt, char *sect, char *opt, char *value);

/**
 * load_config() - Config loading function
 * @arg1: Name of the config
 * @arg2: UCI context
 * @arg3: UCI package
 *
 * Return: Returns 0 on success, otherwise returns error code.
 */
int load_config(const char *config_name, struct uci_context **context, struct uci_package **package)
{
	*context = uci_alloc_context();

	if (uci_load(*context, config_name, package) != UCI_OK) {
		uci_free_context(*context);
		return ERROR_LOADING_UCI_CONFIG;
	}
	return 0;
}

/**
 * set_all_configs() - Function that handles settings, events, topics configuration loading
 * @arg1: UCI package
 * @arg2: UCI context
 * @arg3: Settings struct
 * @arg4: List of events
 * @arg5: List of topics
 *
 * Return: Returns 0 on success, otherwise returns error code.
 */
int set_all_configs(struct uci_package *package, struct uci_context *context,
		    struct sub_config_settings sub_settings, struct event_list *events,
		    struct topic_list *topics)
{
	int rt = 0;
	struct uci_element *i;
	uci_foreach_element (&package->sections, i) {
		struct uci_section *section = uci_to_section(i);
		char *section_type	    = section->type;
		if (strcmp(section_type, "mqtt_settings") == 0) {
			rt = handle_settings(context, section, sub_settings);
		}
		if (strcmp(section_type, "mqtt_event") == 0) {
			rt = handle_event(context, section, events);
		}
		if (strcmp(section_type, "mqtt_topic") == 0) {
			rt = handle_topic(context, section, topics);
		}
	}
	return rt;
}

/**
 * uci_set_option() - Helper function for modifying option
 * @arg1: Config package
 * @arg2: Section name
 * @arg3: Option name
 * @arg4: Value to store
 *
 * Return: Returns 0 on success, otherwise returns error code.
 */
int uci_set_option(char *pkt, char *sect, char *opt, char *value)
{
	if (pkt == NULL || sect == NULL || opt == NULL)
		return ERROR_SETTING_OPTION;
	struct uci_context *_ctx = uci_alloc_context();
	struct uci_ptr ptr	 = {
		      .package = pkt,
		      .section = sect,
		      .option  = opt,
		      .value   = value,
	};
	uci_set(_ctx, &ptr);
	uci_commit(_ctx, &ptr.p, false);
	uci_unload(_ctx, ptr.p);
	uci_free_context(_ctx);
	return 0;
}

/**
 * handle_settings() - Helper function for set_all_configs(), modifies the settings section
 * @arg1: UCI context
 * @arg2: UCI section
 * @arg3: Settings struct
 *
 * Return: Returns 0 on success, otherwise returns error code.
 */
int handle_settings(struct uci_context *context, struct uci_section *section,
		    struct sub_config_settings sub_settings)
{
	int rt = 0;
	struct uci_option *host, *port, *user, *password, *enabled, *ca_file, *cert_file, *key_file, *sender,
		*app_pw;
	host	  = uci_lookup_option(context, section, "host");
	port	  = uci_lookup_option(context, section, "port");
	user	  = uci_lookup_option(context, section, "user");
	password  = uci_lookup_option(context, section, "password");
	enabled	  = uci_lookup_option(context, section, "enabled");
	ca_file	  = uci_lookup_option(context, section, "cafile");
	cert_file = uci_lookup_option(context, section, "certfile");
	key_file  = uci_lookup_option(context, section, "keyfile");
	sender	  = uci_lookup_option(context, section, "sender");
	app_pw	  = uci_lookup_option(context, section, "app_pw");

	if (host != NULL) {
		rt = uci_set_option("mqtt_subscriber", section->e.name, "host", sub_settings.host);
	}
	if (port != NULL) {
		rt = uci_set_option("mqtt_subscriber", section->e.name, "port", sub_settings.port);
	}
	if (user != NULL) {
		rt = uci_set_option("mqtt_subscriber", section->e.name, "user", sub_settings.username);
	}
	if (password != NULL) {
		rt = uci_set_option("mqtt_subscriber", section->e.name, "password", sub_settings.password);
	}
	if (enabled != NULL) {
		if (sub_settings.enabled == NULL) {
			sub_settings.enabled = "0";
		}
		rt = uci_set_option("mqtt_subscriber", section->e.name, "enabled", sub_settings.enabled);
	}
	if (app_pw != NULL && sender != NULL) {
		rt = uci_set_option("mqtt_subscriber", section->e.name, "sender", sub_settings.sender);
		rt = uci_set_option("mqtt_subscriber", section->e.name, "app_pw", sub_settings.app_pw);
	}
	if (ca_file != NULL && cert_file != NULL && key_file != NULL) {
		rt = uci_set_option("mqtt_subscriber", section->e.name, "cafile", sub_settings.cafile);
		rt = uci_set_option("mqtt_subscriber", section->e.name, "certfile", sub_settings.certfile);
		rt = uci_set_option("mqtt_subscriber", section->e.name, "keyfile", sub_settings.keyfile);
	}
	return rt;
}

/**
 * handle_event() - Helper function for set_all_configs(), looks up the events section
 * @arg1: UCI context
 * @arg2: UCI section
 * @arg3: List of events
 *
 * If all the options are found will append it to event list
 *
 * Return: Returns 0 on success, otherwise returns error code.
 */
int handle_event(struct uci_context *context, struct uci_section *section, struct event_list *events)
{
	int rt = 0;
	struct uci_option *topic, *parameter, *value_type, *value, *operator, * recipient;
	topic	   = uci_lookup_option(context, section, "topic");
	parameter  = uci_lookup_option(context, section, "parameter");
	value_type = uci_lookup_option(context, section, "value_type");
	value	   = uci_lookup_option(context, section, "value");
	operator= uci_lookup_option(context, section, "operator");
	recipient = uci_lookup_option(context, section, "recipient");

	if (topic != NULL && parameter != NULL && value_type != NULL &&
	    value != NULL && operator!= NULL && recipient != NULL) {
		insert_at_event_end(topic->v.string, parameter->v.string, value_type->v.string,
				    value->v.string, operator->v.string, recipient->v.string, events);
	} else
		return ERROR_OPTION_NOT_FOUND;

	return rt;
}

/**
 * handle_topic() - Helper function for set_all_configs(), looks up the topic section
 * @arg1: UCI context
 * @arg2: UCI section
 * @arg3: List of topics
 *
 * If all the options are found will append it to topic list
 *
 * Return: Returns 0 on success, otherwise returns error code.
 */
int handle_topic(struct uci_context *context, struct uci_section *section, struct topic_list *topics)
{
	int rt = 0;
	struct uci_option *topic, *qos;

	topic = uci_lookup_option(context, section, "topic");
	qos   = uci_lookup_option(context, section, "qos");
	if (topic != NULL && qos != NULL) {
		insert_at_end(topic->v.string, qos->v.string, topics);
	} else
		return ERROR_OPTION_NOT_FOUND;
	return rt;
}