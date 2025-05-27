#include "cls_device.h"

device_t *cls_device_init(const char *name) {
    if (!name) {
        return NULL; // Handle null name case
    }

    device_t *device = (device_t *)malloc(sizeof(device_t));
    if (!device) {
        return NULL; // Memory allocation failed
    }

    strncpy(device->name, name, sizeof(device->name) - 1);
    device->name[sizeof(device->name) - 1] = '\0'; // Ensure null termination
    device->measures = NULL;

    return device;
}

void cls_device_add_measure(device_t *device, measure_t *measure) {

    if (!device || !measure) {
        return; // Handle null pointers
    }

    if (device->measures == NULL) {
        device->measures = cls_linked_list_init();
        if (!device->measures) {
            return; // Failed to initialize the linked list
        }
    }

    cls_linked_list_add(device->measures, measure);
}

void cls_device_calculate_average_readings(device_t *device) {
    (void)device; // Placeholder for future implementation
}

float cls_device_get_minimum_reading(const device_t *device, sensor_type_t sensor, date_t *date) {
    (void)device;   // Placeholder for future implementation
    (void)sensor;   // Placeholder for future implementation
    (void)date;     // Placeholder for future implementation
    return FLT_MAX; // Return max float value as a placeholder
}

float cls_device_get_average_reading(const device_t *device, sensor_type_t sensor, date_t *date) {
    (void)device;   // Placeholder for future implementation
    (void)sensor;   // Placeholder for future implementation
    (void)date;     // Placeholder for future implementation
    return FLT_MAX; // Return max float value as a placeholder
}

float cls_device_get_maximum_reading(const device_t *device, sensor_type_t sensor, date_t *date) {
    (void)device;   // Placeholder for future implementation
    (void)sensor;   // Placeholder for future implementation
    (void)date;     // Placeholder for future implementation
    return FLT_MAX; // Return max float value as a placeholder
}