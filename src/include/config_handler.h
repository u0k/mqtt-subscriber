#pragma once
#include <uci.h>
#include <linked_list.h>
int load_config(const char *config_name, struct uci_context **context, struct uci_package **package);
int set_all_configs(struct uci_package *package, struct uci_context *context,
		    struct sub_config_settings sub_settings, struct event_list *events,
		    struct topic_list *topics);