#include "lib_device_mapper_private.h"

device_t *get_device(linked_list_t *devices, const char *name) {
    if (!devices || !name) {
        return NULL;
    }

    uint32_t number_of_devices = cls_linked_list_size(devices);
    for (uint32_t i = 0; i < number_of_devices; ++i) {
        device_t *device = (device_t *)cls_linked_list_get_data(devices, i);
        if (device && strcmp(device->name, name) == 0) {
            return device;
        }
    }
    return NULL; // Device not found
}