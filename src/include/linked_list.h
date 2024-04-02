#pragma once
struct topic {
	char topic[100];
	char qos[100];
	struct topic *next;
};
struct sub_config_settings {
	char *host;
	char *port;
	char *username;
	char *password;
	char *enabled;
	char *cafile;
	char *certfile;
	char *keyfile;
	char *sender;
	char *app_pw;
};
struct event {
	char topic[100];
	char parameter[100];
	char value_type[100];
	char value[100];
	char operator[100];
	char recipient[100];

	struct event *next;
};
struct topic_list {
	int size;
	struct topic *head;
};
struct event_list {
	int size;
	struct event *head;
};

struct all_lists {
	struct topic_list *topics;
	struct event_list *events;
	struct sub_config_settings sub_settings;
};

struct topic_list *create_list();
struct event_list *create_event_list();
void insert_at_end(char *topic, char *qos, struct topic_list *list);
void insert_at_event_end(char *topic, char *parameter, char *value_type, char *value, char *operator,
			 char * recipient, struct event_list *list);
void delete_list(struct topic_list **list);
void delete_event_list(struct event_list **list);
int get_size(struct topic_list *list);
void free_all_lists(struct all_lists *lists);
int get_event_size(struct event_list *list);