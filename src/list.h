#ifndef _LIST_H_
#define _LIST_H_

#include <sys/types.h>

typedef struct list {
	void *val;
	struct list *next;
} list_t;

void
list_add (list_t **head, list_t **tail, void *item);

size_t
list_count (list_t *head);

#endif