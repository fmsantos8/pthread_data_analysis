#ifndef __CLS_LINKED_LIST_H__
#define __CLS_LINKED_LIST_H__

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct node_t {
    void *data;
    struct node_t *next;
} node_t;

typedef struct linked_list_t {
    node_t *head;
    node_t *tail;

    uint32_t size; /** Number of elements in the list */
} linked_list_t;

linked_list_t *cls_linked_list_init(void);

void cls_linked_list_add(linked_list_t *list, void *data);

void *cls_linked_list_get_data(linked_list_t *list, uint32_t index);

void cls_linked_list_remove(linked_list_t *list, uint32_t index);

void cls_linked_list_free(linked_list_t *list);

static inline uint32_t cls_linked_list_size(linked_list_t *list) {
    return list->size;
}

#endif