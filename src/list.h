#ifndef _LIST_H_
#define _LIST_H_

#include <sys/types.h>

/**
 * A linked list node
 */
typedef struct list {
	/** The value of the node */
	void *val;
	/** A pointer to the next node */
	struct list *next;
} list_node_t;

/**
 * A linked list container
 */
typedef struct {
	/** The first item in the list */
	list_node_t *head;
	/** The last item in the list */
	list_node_t *tail;
} list_t;

/**
 * Initialize a linked list
 * @param list A pointer to the list to be initialized
 */
void 
list_init (list_t * list);

/**
 * Add an item to a linked list
 * @param list A pointer to the target list
 * @param item A pointer to the value that should be added
 * @return 0 on success
 */
int
list_add (list_t *list, void *item);

/**
 * Count the items in a linked list
 * @param list A pointer to the target list
 * @return The total count of items
 */
size_t
list_count (list_t *list);

#endif
