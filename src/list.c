#include <sys/types.h>
#include <stdlib.h>

#include "list.h"

void 
list_init (list_t *list)
{
	list->head = NULL;
	list->tail = NULL;
}

void
list_add (list_t *list, void *item)
{
	list_node_t *node = malloc(sizeof (list_node_t));
	node->val = item;
	node->next = NULL;
	
	if (list->head == NULL) {
		list->head = node;
		list->tail = node;
	} else {
		list->tail->next = node;
	}
	list->tail = node;
}

size_t
list_count (list_t *list)
{
	list_node_t *cursor = list->head;
	size_t result = 0;
	
	while (cursor != NULL) {
		cursor = cursor->next;
		++result;
	}
	
	return result;
}