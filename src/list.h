#ifndef _LIST_H_
#define _LIST_H_

#include <sys/types.h>

typedef struct list {
	void *val;
	struct list *next;
} list_node_t;

typedef struct {
	list_node_t *head;
	list_node_t *tail;
} list_t;

void 
list_init (list_t * list);

void
list_add (list_t *list, void *item);

size_t
list_count (list_t *list);

#endif