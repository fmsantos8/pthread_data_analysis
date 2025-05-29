#include "lib_device_mapper_private.h"

device_t *get_device(linked_list_t *devices, const char *name) {
    if (!devices || !name) {
        return NULL;
    }

    node_t *current = cls_linked_list_get(devices, 0);
    while (current != NULL) {
        device_t *device = (device_t *)cls_linked_list_get_data(current);
        if (device && strcmp(device->name, name) == 0) {
            return device; // Device found
        }
        current = cls_linked_list_get_next(current);
    }
    return NULL; // Device not found
}