#pragma once
#include <sqlite3.h>

int create_table();
int open_db();
int close_db();
int insert_message(char *topic, char *payload, int timestamp);
