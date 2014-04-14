#include <sys/types.h>
#include <stdlib.h>

#include "list.h"

void
list_add (list_t **head, list_t **tail, void *item)
{
	list_t *container = malloc(sizeof (list_t));
	container->val = item;
	container->next = NULL;
	
	if (*head == NULL) {
		*head = container;
	}
	if (*tail != NULL) {
		(*tail)->next = container;
	}
	(*tail) = container;
}

size_t
list_count (list_t *head)
{
	list_t *cursor = head;
	size_t result = 0;
	
	while (cursor != NULL) {
		cursor = cursor->next;
		++result;
	}
	
	return result;
}