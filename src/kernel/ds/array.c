#include <include/ds/array.h>
#include <include/kernel/mem.h>
#include <include/kernel/logger.h>

#include <string.h>
#include <stdint.h>

static const uint32_t growth_factor = 2;

static void remove_elem(array_t *array, void *elem, void (*del)(void *), bool (*cmp)(const void *, const void *));

array_t *array_create(size_t initial_size) {
	array_t *arr = kmalloc(sizeof(*arr));
	arr->used = 0;
	arr->size = initial_size;
	arr->data = kmalloc(initial_size * sizeof(void *));

	return arr;
}

void array_add(array_t *array, void *elem) {
	// Full? Allocate more
	if (array->used == array->size) {
		array->size *= growth_factor;
		void **tmp = kmalloc(array->size * sizeof(void *));
		memcpy(tmp, array->data, array->used * sizeof(void *));

		kfree(array->data);
		array->data = tmp;
	}

	// Add to array
	array->data[array->used++] = elem;
}

void *array_get(array_t *array, size_t idx) {
	if (idx > array->used) {
		KWARNING("Attempt to index into array of size %d with %d filled slots at index %d!", array->size, array->used, idx);
		return NULL;
	}

	return array->data[idx];
}

void array_remove(array_t *array, void *elem) {
	remove_elem(array, elem, NULL, NULL);
}

void array_remove_cmp(array_t *array, void *elem, bool (*cmp)(const void *, const void *)) {
	remove_elem(array, elem, NULL, cmp);
}

void array_delete(array_t *array, void *elem, void (*del)(void *)) {
	remove_elem(array, elem, del, NULL);
}

void array_delete_cmp(array_t *array, void *elem, void (*del)(void *), bool (*cmp)(const void *, const void *)) {
	remove_elem(array, elem, del, cmp);
}

void array_destroy(array_t *array, void (*del)(void *)) {
	// Delete all used slots
	for (size_t i = 0; i < array->used; i++) {
		(del ? del : kfree)(array->data[i]);
	}

	// Dispose of the array and it's data
	kfree(array->data);
	kfree(array);
}


static void remove_elem(array_t *array, void *elem, void (*del)(void *), bool (*cmp)(const void *, const void *)) {
	// Find the index of the element, if it exists.
	size_t idx = 0;
	for (; idx < array->used; idx++) {
		if ((cmp && cmp(elem, array->data[idx])) || array->data[idx] == elem) {
			break;
		}
	}

	// Check if it was found (should be less than the index of the last used slot)
	if (idx == array->used) {
		// Not found
		KTRACE("Failed to find a requested element to remove");
		return;
	}

	// Given that we know the index, we can handle deletion
	(del ? del : kfree)(array->data[idx]);

	// Shift the data out... it is O(N), but we accept this since it is a simple dynamic array
	for (; idx < (array->size - 1); idx++) {
		array->data[idx] = array->data[idx + 1];
	}

	// We are done; the garbage data is at the end of the array, and we don't even consider it used anymore.
	array->used--;
}