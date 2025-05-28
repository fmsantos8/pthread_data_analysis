#include "cls_linked_list.h"

linked_list_t *cls_linked_list_init(void) {
    linked_list_t *list = (linked_list_t *)malloc(sizeof(linked_list_t));
    list->head = NULL;
    list->tail = NULL; // Initialize tail
    list->size = 0;
    return list;
}

void cls_linked_list_add(linked_list_t *list, void *data) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->data = data;
    new_node->next = NULL;

    list->size++;

    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node; // Set tail to new node
        return;
    }
    list->tail->next = new_node; // Add after tail
    list->tail = new_node;       // Update tail
}

void *cls_linked_list_get_data(linked_list_t *list, uint32_t index) {
    if (list->head == NULL) {
        return NULL; // List is empty
    }
    node_t *current = list->head;
    for (uint32_t i = 0; i < index && current != NULL; i++) {
        current = current->next;
    }
    return current ? current->data : NULL;
}

void cls_linked_list_remove(linked_list_t *list, uint32_t index) {
    if (list->head == NULL) {
        return; // List is empty
    }
    node_t *current = list->head;
    node_t *previous = NULL;

    for (uint32_t i = 0; i < index && current != NULL; i++) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        return; // Index out of bounds
    }

    if (previous == NULL) {
        list->head = current->next; // Remove head
        if (list->head == NULL) {
            list->tail = NULL; // List is now empty
        }
    } else {
        previous->next = current->next; // Bypass the current
        if (previous->next == NULL) {
            list->tail = previous; // Removed last node, update tail
        }
    }
    free(current); // Free the removed node
    list->size--;
}

void cls_linked_list_free(linked_list_t *list) {
    node_t *current = list->head;
    while (current != NULL) {
        node_t *next = current->next;
        free(current);
        current = next;
    }
    free(list);
}