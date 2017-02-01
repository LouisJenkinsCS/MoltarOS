#include <string.h>

#include <include/ds/list.h>
#include <include/kernel/mem.h>

static node_t *node_create(void *elem) {
	node_t *node = kmalloc(sizeof(*node));
	memset(node, 0, sizeof(*node));
	node->data = elem;
	return node;
}

list_t *list_create() {
	list_t *list = kmalloc(sizeof(*list));
	memset(list, 0, sizeof(*list));
	return list;
}

void list_front(list_t *list, void *elem) {
	node_t *node = node_create(elem);
	if (!list->head) {
		list->head = list->tail = list->current = node;
	} else {
		list->tail->next = node;
		node->prev = list->tail;
		list->tail = node;
	}
}

void list_back(list_t * list, void *elem) {
	node_t *node = node_create(elem);
	if (!list->head) {
		list->head = list->tail = list->current = node;
	} else {
		list->head->prev = node;
		node->next = list->head;
		list->head = node;
	}
}

void list_append(list_t *list, void *elem) {
	// Put at back of list if no current or if only item in list
	if (!list->current || (list->current == list->head && list->current == list->tail)) {
		list_front(list, elem);
	} else {
		node_t *node = node_create(elem);
		
		// If current is tail, add to front of list
		if (list->current == list->tail) {
			list_front(list, elem);
		} else {
			// Add in between...
			node_t *next = list->current->next;
			node_t *curr = list->current;

			curr->next = node;
			node->prev = curr;
			node->next = next;

			if (next) {
				next->prev = node;
			}

		}
	}
}

void list_prepend(list_t * list, void *elem) {
	// Put at back of list if no current or if only item in list
	if (!list->current || (list->current == list->head && list->current == list->tail)) {
		list_back(list, elem);
	} else {
		node_t *node = node_create(elem);
		
		// If current is head, add to back of list
		if (list->current == list->head) {
			list_back(list, elem);
		} else {
			// Add in between...
			node_t *prev = list->current->prev;
			node_t *curr = list->current;

			curr->prev = node;
			node->next = curr;
			node->prev = prev;

			if (prev) {
				prev->next = node;
			}
		}
	}
}

void *list_next(list_t *list) {
	if (list->current) {
		list->current = list->current->next;
	}
	
	return list->current ? list->current->data : NULL;
}

void *list_prev(list_t *list) {
	if (list->current) {
		list->current = list->current->prev;
	}
	
	return list->current ? list->current->data : NULL;
}

void list_remove(list_t *list, void (*del)(void *)) {
	if (list->current) {

		if (list->head == list->tail) {
			list->head = list->tail = NULL;
		} else if (list->current == list->head) {
			list->head = list->head->next;
			if (list->head) {
				list->head->prev = NULL;
			}
		} else if (list->current == list->tail) {
			list->tail = list->tail->prev;
			if (list->tail) {
				list->tail->next = NULL;
			}
		} else {
			node_t *next = list->current->next;
			node_t *prev = list->current->prev;

			if (next) {
				next->prev = prev;
			}

			if (prev) {
				prev->next = next;
			}
		}

		if (del) {
			del(list->current->data);
		}
		kfree(list->current);
		list->current = list->head;
	}
}

void *list_current(list_t *list) {
	return list->current ? list->current->data : NULL;
}

void *list_head(list_t *list) {
	list->current = list->head;
	
	return list->current ? list->current->data : NULL;
}

void *list_tail(list_t *list) {
	list->current = list->tail;
	
	return list->current ? list->current->data : NULL;
}

void list_destroy(list_t *list, void (*del)(void *)) {
	for (node_t *curr = list->head; curr;) {
		node_t *tmp = curr->next;
		if (del) {
			del(curr->data);
		}
		kfree(curr);
		curr = tmp;
	}

	kfree(list);
}