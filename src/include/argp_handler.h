#pragma once
#include <argp.h>
struct arguments {
	char *host;
	char *port;
	char *user;
	char *password;
};

error_t parse_opt(int key, char *arg, struct argp_state *state);