#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_size(struct topic_list *list);

/**
 * create_node() - Create a topic node.
 * @arg1: Topic string
 * @arg2: Quality of Service string
 *
 * Return: Returns NULL if parsing has failed, or memory for the node couldn't be allocated, otherwise returns
 * the created topic.
 */
struct topic *create_node(char *topic, char *qos)
{
	struct topic *newNode = (struct topic *)malloc(sizeof(struct topic));
	if (newNode == NULL || topic == NULL || qos == NULL) {
		return NULL;
	}
	strncpy(newNode->topic, topic, sizeof(newNode->topic));
	strncpy(newNode->qos, qos, sizeof(newNode->qos));
	newNode->next = NULL;
	return newNode;
}

/**
 * create_event_node() - Create an event node.
 * @arg1: Topic string
 * @arg2: Event parameter
 * @arg3: Event value type to compare to (numeric, alphanumeric)
 * @arg4: Value that will be used for comparison
 * @arg5: Operator that will be used for comparison
 * @arg6: Email of the sender
 * @arg7: Email of the recipient
 * @arg8: App password for GMAIL SMTP
 *
 * Return: Returns NULL if parsing has failed, or memory for the node couldn't be allocated, otherwise returns
 * the created event.
 */
struct event *create_event_node(char *topic, char *parameter, char *value_type, char *value, char *operator,
				char * recipient)
{
	struct event *newNode = (struct event *)malloc(sizeof(struct event));
	if (newNode == NULL || topic == NULL || parameter == NULL || value_type == NULL ||
	    value == NULL || operator== NULL || recipient == NULL) {
		return NULL;
	}
	strncpy(newNode->topic, topic, sizeof(newNode->topic));
	strncpy(newNode->parameter, parameter, sizeof(newNode->parameter));
	strncpy(newNode->value_type, value_type, sizeof(newNode->value_type));
	strncpy(newNode->value, value, sizeof(newNode->value));
	strncpy(newNode->operator, operator, sizeof(newNode->operator));
	strncpy(newNode->recipient, recipient, sizeof(newNode->recipient));

	newNode->next = NULL;
	return newNode;
}

/**
 * create_list() - Create a topic linked list.
 *
 * Additionaly allocates memory for the linked list, initializes its members.
 *
 * Return: Returns NULL if the memory for list couldn't be allocated, otherwise returns the list
 */
struct topic_list *create_list()
{
	struct topic_list *list = (struct topic_list *)malloc(sizeof(struct topic_list));
	if (list == NULL) {
		return NULL;
	}
	list->head = NULL;
	list->size = 0;
	return list;
}

/**
 * create_event_list() - Create an event linked list.
 *
 * Additionaly allocates memory for the linked list, initializes its members.
 *
 * Return: Returns NULL if the memory for list couldn't be allocated, otherwise returns the list
 */
struct event_list *create_event_list()
{
	struct event_list *list = (struct event_list *)malloc(sizeof(struct event_list));
	if (list == NULL) {
		return NULL;
	}
	list->head = NULL;
	list->size = 0;
	return list;
}

/**
 * insert_at_end() - Insert a topic at the end of linked list.
 * @arg1: Topic string
 * @arg2: Quality of Service string
 * @arg3: Linked list of type 'struct topic_list *'
 *
 */
void insert_at_end(char *topic, char *qos, struct topic_list *list)
{
	struct topic *current = NULL;
	if (list->head == NULL) {
		list->head = create_node(topic, qos);
		if (list->head == NULL)
			return;
		list->size++;
	} else {
		current = list->head;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = create_node(topic, qos);
		if (current->next == NULL)
			return;
		list->size++;
	}
}

/**
 * insert_at_event_end() - Insert an event at the end of linked list.
 * @arg1: Topic string
 * @arg2: Event parameter
 * @arg3: Event value type to compare to (numeric, alphanumeric)
 * @arg4: Value that will be used for comparison
 * @arg5: Operator that will be used for comparison
 * @arg6: Email of the sender
 * @arg7: Email of the recipient
 * @arg8: App password for GMAIL SMTP
 * @arg9: Linked list of type 'struct event_list *'
 *
 */
void insert_at_event_end(char *topic, char *parameter, char *value_type, char *value, char *operator,
			 char * recipient, struct event_list *list)
{
	struct event *current = NULL;
	if (list->head == NULL) {
		list->head = create_event_node(topic, parameter, value_type, value, operator, recipient);
		if (list->head == NULL)
			return;
		list->size++;
	} else {
		current = list->head;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = create_event_node(topic, parameter, value_type, value, operator, recipient);
		if (current->next == NULL)
			return;
		list->size++;
	}
}

/**
 * delete_list() - Delete the topic list.
 * @arg1: A linked list of type 'struct topic_list *'
 *
 */
void delete_list(struct topic_list **list)
{
	if (*list == NULL)
		return;
	struct topic *current = (*list)->head;
	struct topic *next;
	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	free(*list);
	*list = NULL;
}

/**
 * delete_event_list() - Delete the event list.
 * @arg1: A linked list of type 'struct event_list *'
 *
 */
void delete_event_list(struct event_list **list)
{
	if (*list == NULL)
		return;
	struct event *current = (*list)->head;
	struct event *next;
	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	free(*list);
	*list = NULL;
}

/**
 * free_all_lists() - Deletes both event and topic lists from combined struct.
 * @arg1: A struct of type 'all_lists *'
 *
 */
void free_all_lists(struct all_lists *lists)
{
	if (lists != NULL) {
		if (lists->topics != NULL) {
			delete_list(&(lists->topics));
		}
		if (lists->events != NULL) {
			delete_event_list(&(lists->events));
		}
	}
}

/**
 * get_size() - Get size of the topic linked list.
 * @arg1: A linked list of type 'struct topic_list *'
 *
 * Return: Returns 0 if list doesn't exist, otherwise returns the size of list.
 */
int get_size(struct topic_list *list)
{
	if (list != NULL)
		return list->size;
	else
		return 0;
}

/**
 * get_event_size() - Get size of the event linked list.
 * @arg1: A linked list of type 'struct event_list *'
 *
 * Return: Returns 0 if list doesn't exist, otherwise returns the size of list.
 */
int get_event_size(struct event_list *list)
{
	if (list != NULL)
		return list->size;
	else
		return 0;
}