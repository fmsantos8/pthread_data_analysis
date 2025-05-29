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

char *sensor_type_to_string(sensor_type_t sensor) {
    switch (sensor) {
        case SENSOR_TEMPERATURE:
            return "temperatura";
        case SENSOR_HUMIDITY:
            return "umidade";
        case SENSOR_LIGHT:
            return "luminosidade";
        case SENSOR_NOISE:
            return "ruido";
        case SENSOR_ECO2:
            return "eco2";
        case SENSOR_ETVOC:
            return "etvoc";
        default:
            assert(0);
    }
}

uint32_t get_number_of_rows(linked_list_t *devices) {

    if (!devices) {
        return 0; // No devices, no rows
    }

    uint32_t total_rows = 1; // Start with 1 for the header row

    node_t *current_device = cls_linked_list_get(devices, 0);
    while (current_device != NULL) {
        device_t *device = (device_t *)cls_linked_list_get_data(current_device);
        if (device) {
            node_t *current_stats = cls_linked_list_get(device->stats, 0);
            while (current_stats != NULL) {
                stats_t *stats = (stats_t *)cls_linked_list_get_data(current_stats);
                if (stats) {
                    for (uint32_t j = 0; j < SENSOR_MAX; ++j) {
                        if (stats->average_count[j] > 0) {
                            total_rows++;
                        }
                    }
                }
                current_stats = cls_linked_list_get_next(current_stats);
            }
        }
        current_device = cls_linked_list_get_next(current_device);
    }

    return total_rows;
}

void convert_to_csv_row(csv_row_t *row, device_t *device, stats_t *stats, sensor_type_t sensor) {
    if (!row || !device || !stats) {
        return;
    }

    row->fields = calloc(6, sizeof(char *));
    row->field_count = 6;
    char buf1[32], buf2[32], buf3[32], buf4[32], buf5[32], buf6[32];
    snprintf(buf1, sizeof(buf1), "%s", device->name);
    snprintf(buf2, sizeof(buf2), "%u-%02u", stats->date.year, stats->date.month);
    snprintf(buf3, sizeof(buf3), "%s", sensor_type_to_string(sensor));
    snprintf(buf4, sizeof(buf4), "%.2f", cls_device_get_maximum_reading(device, sensor, &stats->date));
    snprintf(buf5, sizeof(buf5), "%.2f", cls_device_get_average_reading(device, sensor, &stats->date));
    snprintf(buf6, sizeof(buf6), "%.2f", cls_device_get_minimum_reading(device, sensor, &stats->date));
    row->fields[0] = strdup(buf1);
    row->fields[1] = strdup(buf2);
    row->fields[2] = strdup(buf3);
    row->fields[3] = strdup(buf4);
    row->fields[4] = strdup(buf5);
    row->fields[5] = strdup(buf6);
}