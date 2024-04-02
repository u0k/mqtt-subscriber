#include "mqtt_database.h"
#include <stdlib.h>
#include "message_handler.h"
#include <stdio.h>

sqlite3 *db_mqtt;

/**
 * open_db() - Open database file
 *
 * Return: 0 on success, error code on failure
 */
int open_db()
{
	int rt;
	rt = sqlite3_open("mqtt_messages.db", &db_mqtt);
	if (rt) {
		return ERROR_SQLITE_DB_OPEN;
	}
	return 0;
}

/**
 * close_db() - Close database file
 *
 * Return: 0 on success, error code on failure
 */
int close_db()
{
	int rt;
	rt = sqlite3_close(db_mqtt);
	if (rt) {
		db_mqtt = NULL;
		return ERROR_SQLITE_DB_CLOSE;
	}
	return 0;
}

/**
 * create_table() - Create a database table
 *
 * Return: 0 on success, error code on failure
 */
int create_table()
{
	char *createErrMsg;
	int rt;
	char *query;
	createErrMsg = 0;
	query = "CREATE TABLE IF NOT EXISTS 'MESSAGE' (topic varchar(255), payload varchar(255), timestamp datetime);";
	rt = sqlite3_exec(db_mqtt, query, 0, 0, &createErrMsg);

	if (rt != SQLITE_OK) {
		sqlite3_free(createErrMsg);
		return ERROR_SQLITE_TABLE_CREATE;
	}
	return rt;
}

/**
 * insert_message() - Insert a new record into 'MESSAGE' table
 *
 * Return: 0 on success, error code on failure
 */
int insert_message(char *topic, char *payload, int timestamp)
{
	char *insertErrMsg;
	char *query;
	int rt;
	query = sqlite3_mprintf("INSERT INTO 'MESSAGE' VALUES ('%s', '%s', '%d');", topic, payload,
				timestamp);
	rt    = sqlite3_exec(db_mqtt, query, 0, 0, &insertErrMsg);
	if (rt != SQLITE_OK) {
		sqlite3_free(insertErrMsg);
		return ERROR_SQLITE_INSERT_MESSAGE;
	}
	sqlite3_free(query);
	return rt;
}