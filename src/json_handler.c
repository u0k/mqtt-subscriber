#include "json_handler.h"
#include "message_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/**
 * parse_property_json() - Parse json string
 * @arg1: A json string
 * @arg2: A parameter that will be looked for in the json string
 * @arg3: Type of value expected (numeric, alphanumeric)
 * @arg4: Value that will be compared against
 * @arg5: Operator that will be used for comparison
 *
 * Return: Returns 1 on success, otherwise returns error code.
 */
int parse_property_json(char *json_string, char *parameter, char *value_type, char *value_to_check,
			char *operator)
{
	int rt;
	struct json_object *jobj;
	struct json_object *value;
	jobj = json_tokener_parse(json_string);
	if (jobj != NULL) {
		if (json_object_object_get_ex(jobj, parameter, &value)) {
			if (strcmp(value_type, "numeric") == 0) {
				if (json_object_is_type(value, json_type_double) ||
				    json_object_is_type(value, json_type_int)) {
					rt = handle_operation_numeric(value, value_to_check, operator);
					goto cleanup;
				} else {
					rt = ERROR_UNKNOWN_VALUE_TYPE;
					goto cleanup;
				}

			} else if (strcmp(value_type, "alphanumeric") == 0) {
				if (json_object_is_type(value, json_type_string)) {
					rt = handle_operation_string(value, value_to_check, operator);
					goto cleanup;
				} else {
					rt = ERROR_UNKNOWN_VALUE_TYPE;
					goto cleanup;
				}
			} else {
				rt = ERROR_UNKNOWN_VALUE_TYPE;
				goto cleanup;
			}
		} else {
			rt = ERROR_JSON_PARAMETER_NOT_FOUND;
			goto cleanup;
		}
	} else {
		rt = ERROR_JSON_OBJECT_NULL;
		goto cleanup;
	}

cleanup:
	if (jobj != NULL)
		json_object_put(jobj);
	return rt;
}

/**
 * handle_operation_string() - Handler for alphanumeric comparison
 * @arg1: A json object
 * @arg2: Value that will be compared against
 * @arg3: Operator that will be used for comparison
 *
 * Return: Returns 1 on success, otherwise returns error code.
 */
int handle_operation_string(struct json_object *value, char *value_to_check, char *operator)
{
	const char *string_value;
	string_value = json_object_get_string(value);

	if (strcmp(operator, "==") == 0)
		return strcmp(string_value, value_to_check) == 0;
	else if (strcmp(operator, "!=") == 0)
		return strcmp(string_value, value_to_check) != 0;
	else
		return ERROR_ALPHANUMERIC_OPERATION;
}

/**
 * handle_operation_numeric() - Handler for numeric comparison
 * @arg1: A json object
 * @arg2: Value that will be compared against
 * @arg3: Operator that will be used for comparison
 *
 * Return: Returns 1 on success, otherwise returns error code.
 */
int handle_operation_numeric(struct json_object *value, char *value_to_check, char *operator)
{
	double numeric_value;
	double numeric_value_to_check;
	char *pEnd;
	errno = 0;
	if (json_object_is_type(value, json_type_double)) {
		numeric_value	       = json_object_get_double(value);
		numeric_value_to_check = strtod(value_to_check, &pEnd);
		if (errno != 0 && numeric_value_to_check == 0) {
			return ERROR_NUMERIC_OPERATION;
		}

	} else if (json_object_is_type(value, json_type_int)) {
		numeric_value	       = json_object_get_int(value);
		numeric_value_to_check = strtol(value_to_check, &pEnd, 10);
		if (errno != 0 && numeric_value_to_check == 0) {
			return ERROR_NUMERIC_OPERATION;
		}
	}
	if (strcmp(operator, "<=") == 0)
		return numeric_value <= numeric_value_to_check;
	else if (strcmp(operator, ">=") == 0)
		return numeric_value >= numeric_value_to_check;
	else if (strcmp(operator, ">") == 0)
		return numeric_value > numeric_value_to_check;
	else if (strcmp(operator, "<") == 0)
		return numeric_value < numeric_value_to_check;
	else if (strcmp(operator, "==") == 0)
		return numeric_value == numeric_value_to_check;
	else if (strcmp(operator, "!=") == 0)
		return numeric_value != numeric_value_to_check;
	else
		return ERROR_NUMERIC_OPERATION;
}