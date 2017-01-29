#ifndef MOLTAROS_LIST_H
#define MOLTAROS_LIST_H

#include <stddef.h>

typedef struct node {
	struct node *next;
	struct node *prev;
	void *data;
} node_t;

typedef struct list {
	node_t *head;
	node_t *tail;
	node_t *current;
	size_t size;
} list_t;

list_t *list_create();

void list_front(list_t *list, void *elem);

void list_back(list_t * list, void *elem);

void *list_next(list_t *list);

void *list_prev(list_t *list);

void *list_current(list_t *list);

void list_append(list_t *list, void *elem);

void list_prepend(list_t * list, void *elem);

void list_remove(list_t *list, void (*del)(void *));

void *list_head(list_t *list);

void *list_tail(list_t *list);

void list_destroy(list_t *list, void (*del)(void *));

#endif /* endif MOLTAROS_LIST_H */