#include "argp_handler.h"
#include "config_handler.h"

/**
 * parse_opt() - Parse given options
 *
 * Accept '-D' '-u' '-s' '-ca' '-cert' '-key' '-h' '-p' options
 *
 * Return: Error code on failure, otherwise 0.
 */
error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct sub_config_settings *sub_settings = state->input;
	switch (key) {
	case 'D':
		sub_settings->enabled = "1";
		break;
	case 'u':
		sub_settings->username = arg;
		break;
	case 's':
		sub_settings->password = arg;
		break;
	case 'c':
		sub_settings->cafile = arg;
		break;
	case 'x':
		sub_settings->certfile = arg;
		break;
	case 'o':
		sub_settings->sender = arg;
		break;
	case 'i':
		sub_settings->app_pw = arg;
		break;
	case 'k':
		sub_settings->keyfile = arg;
		break;
	case 'h':
		sub_settings->host = arg;
		break;
	case 'p':
		sub_settings->port = arg;
		break;

	case ARGP_KEY_ARG:
		argp_failure(state, 1, 0, "required -h, -p options. See --help for more information");
		return ARGP_ERR_UNKNOWN;
	case ARGP_KEY_END:
		if (sub_settings->host == NULL || sub_settings->port == NULL) {
			argp_failure(state, 1, 0,
				     "required -h and -p options. See --help for more information");
			return ARGP_ERR_UNKNOWN;
		}
		if (sub_settings->cafile != NULL && sub_settings->certfile == NULL) {
			argp_failure(state, 1, 0, "missing certfile. See --help for more information");
			return ARGP_ERR_UNKNOWN;
		}
		if (sub_settings->cafile != NULL && sub_settings->keyfile == NULL) {
			argp_failure(state, 1, 0, "missing keyfile. See --help for more information");
			return ARGP_ERR_UNKNOWN;
		}
		if (sub_settings->cafile == NULL &&
		    (sub_settings->certfile != NULL && sub_settings->keyfile != NULL)) {
			argp_failure(state, 1, 0, "missing cafile. See --help for more information");
			return ARGP_ERR_UNKNOWN;
		}
		if (sub_settings->app_pw == NULL && sub_settings->sender != NULL) {
			argp_failure(state, 1, 0, "missing email app token. See --help for more information");
			return ARGP_ERR_UNKNOWN;
		}
		if (sub_settings->sender == NULL && sub_settings->app_pw != NULL) {
			argp_failure(state, 1, 0,
				     "missing sender email address. See --help for more information");
			return ARGP_ERR_UNKNOWN;
		}
		if ((sub_settings->certfile == NULL && sub_settings->keyfile != NULL) ||
		    (sub_settings->certfile != NULL && sub_settings->keyfile == NULL)) {
			argp_failure(
				state, 1, 0,
				"both certfile and keyfile have to be present. See --help for more information");
			return ARGP_ERR_UNKNOWN;
		}
	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}