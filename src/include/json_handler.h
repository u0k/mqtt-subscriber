#pragma once
#include <json-c/json.h>

int parse_property_json(char *json_string, char *parameter, char *value_type, char *value_to_check,
			char *operator);
int handle_operation_numeric(struct json_object *value, char *value_to_check, char *operator);
int handle_operation_string(struct json_object *value, char *value_to_check, char *operator);