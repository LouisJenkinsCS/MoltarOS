#ifndef MOLTAROS_ARRAY_H
#define MOLTAROS_ARRAY_H

#include <stddef.h>
#include <stdbool.h>

typedef struct array {
	size_t size;
	size_t used;
	void **data;
} array_t;

array_t *array_create(size_t initial_size);

void array_add(array_t *array, void *elem);

void *array_get(array_t *array, size_t idx);

void array_remove(array_t *array, void *elem);

void array_remove_cmp(array_t *array, void *elem, bool (*cmp)(const void *, const void *));

void array_delete(array_t *array, void *elem, void (*del)(void *));

void array_delete_cmp(array_t *array, void *elem, void (*del)(void *), bool (*cmp)(const void *, const void *));

void array_destroy(array_t *array, void (*del)(void *));

#endif