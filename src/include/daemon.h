#pragma once
#include "config_handler.h"

int daemon_init();
void daemon_signal_handler(int signum);
int setup_daemon(struct sub_config_settings sub_settings);