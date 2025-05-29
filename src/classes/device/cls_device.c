#include "cls_device.h"

#include <assert.h>
#include <stdio.h>

static void free_stats_data(linked_list_t *stats) {
    if (!stats) {
        return;
    }

    node_t *current = cls_linked_list_get(stats, 0);
    while (current != NULL) {
        stats_t *stats = (stats_t *)cls_linked_list_get_data(current);
        cls_stats_deinit(stats);

        current = cls_linked_list_get_next(current);
    }

    cls_linked_list_deinit(stats);
}

static void free_measure_data(linked_list_t *measures) {
    if (!measures) {
        return;
    }

    node_t *current = cls_linked_list_get(measures, 0);
    while (current != NULL) {
        measure_t *measure = (measure_t *)cls_linked_list_get_data(current);
        cls_measure_deinit(measure);

        current = cls_linked_list_get_next(current);
    }
    cls_linked_list_deinit(measures);
}

static stats_t *get_stats_by_date(linked_list_t *stats, date_t *date) {
    if (!stats || !date) {
        return NULL;
    }

    node_t *current = cls_linked_list_get(stats, 0);
    while (current != NULL) {
        stats_t *stat = (stats_t *)cls_linked_list_get_data(current);
        if (cls_stats_is_same_month(stat, date)) {
            return stat;
        }
        current = cls_linked_list_get_next(current);
    }
    return NULL;
}

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
    device->stats = NULL;

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

    if (!device || !device->measures) {
        return; // Handle null device or no measures
    }

    if (cls_linked_list_size(device->measures) == 0) {
        return; // No measures to calculate average
    }

    free_stats_data(device->stats);
    device->stats = cls_linked_list_init();
    assert(device->stats != NULL);

    node_t *current = cls_linked_list_get(device->measures, 0);
    while (current != NULL) {
        measure_t *measure = (measure_t *)cls_linked_list_get_data(current);
        assert(measure != NULL);

        date_t *date = cls_measure_get_date(measure);
        stats_t *stats = get_stats_by_date(device->stats, date);
        if (stats == NULL) {
            stats = cls_stats_init(date);
            assert(stats != NULL);
            cls_linked_list_add(device->stats, stats);
        }

        sensor_readings_t *readings = cls_measure_get_readings(measure);
        cls_stats_add_readings(stats, readings);
        current = cls_linked_list_get_next(current);
    }
}

float cls_device_get_minimum_reading(const device_t *device, sensor_type_t sensor, date_t *date) {
    if (!device || !device->stats || sensor >= SENSOR_MAX || !date) {
        return FLT_MAX;
    }

    stats_t *stats = get_stats_by_date(device->stats, date);
    if (stats == NULL) {
        return FLT_MAX;
    }

    return cls_stats_get_minimum_reading(stats, sensor);
}

float cls_device_get_average_reading(const device_t *device, sensor_type_t sensor, date_t *date) {
    if (!device || !device->stats || sensor >= SENSOR_MAX || !date) {
        return FLT_MAX;
    }

    stats_t *stats = get_stats_by_date(device->stats, date);
    if (stats == NULL) {
        return FLT_MAX;
    }

    return cls_stats_get_average_reading(stats, sensor);
}

float cls_device_get_maximum_reading(const device_t *device, sensor_type_t sensor, date_t *date) {
    if (!device || !device->stats || sensor >= SENSOR_MAX || !date) {
        return FLT_MAX;
    }

    stats_t *stats = get_stats_by_date(device->stats, date);
    if (stats == NULL) {
        return FLT_MAX;
    }

    return cls_stats_get_maximum_reading(stats, sensor);
}

void cls_device_deinit(device_t *device) {
    if (!device) {
        return; // Handle null pointer
    }

    free_measure_data(device->measures);
    free_stats_data(device->stats);
    free(device); // Free the allocated memory for device
}