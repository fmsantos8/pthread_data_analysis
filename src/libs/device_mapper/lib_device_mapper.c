#include "lib_device_mapper.h"
#include "lib_device_mapper_private.h"

#include <assert.h>
#include <float.h>

linked_list_t *lib_device_mapper_from_csv(csv_data_t *csv) {
    if (!csv) {
        return NULL;
    }

    linked_list_t *devices = cls_linked_list_init();
    if (!devices) {
        return NULL; // Failed to initialize the linked list
    }

    for (uint32_t i = 1; i < csv->number_of_rows; ++i) {
        char *device_name = lib_csv_get_field(csv, i, NAME_COLUMN);

        device_t *device = get_device(devices, device_name);
        if (!device) { // Device not found, create a new one
            device = cls_device_init(device_name);
            assert(device != NULL); // Ensure device creation was successful
            cls_linked_list_add(devices, device);
        }

        date_t date = {0};
        cls_measure_date_from_string(&date, lib_csv_get_field(csv, i, DATE_COLUMN));
        sensor_readings_t readings = {0};
        for (uint32_t j = TEMPERATURE_COLUMN; j <= ETVOC_COLUMN; ++j) {

            const char *reading_string = lib_csv_get_field(csv, i, j);
            if (reading_string) {
                readings.values[j - TEMPERATURE_COLUMN] = cls_measure_reading_from_string(reading_string);
            } else {
                readings.values[j - TEMPERATURE_COLUMN] = FLT_MAX; // No data available, set to max float value
            }
        }

        measure_t *measure = cls_measure_init(&date, &readings);
        assert(measure != NULL);

        cls_device_add_measure(device, measure);
    }

    return devices;
}

csv_data_t *lib_device_mapper_to_csv(linked_list_t *devices) {
    if (!devices) {
        return NULL;
    }

    uint32_t total_rows = get_number_of_rows(devices);

    csv_data_t *csv = calloc(1, sizeof(csv_data_t));
    csv->number_of_columns = 6; // device, ano-mes, sensor, valor_maximo, valor_medio, valor_minimo
    csv->number_of_rows = total_rows;
    csv->rows = calloc(total_rows, sizeof(csv_row_t));

    // Header row
    csv->rows[0].fields = calloc(6, sizeof(char *));
    csv->rows[0].field_count = 6;

    char *header[6] = {"device", "ano-mes", "sensor", "valor_maximo", "valor_medio", "valor_minimo"};
    for (uint32_t j = 0; j < 6; ++j) {
        csv->rows[0].fields[j] = strdup(header[j]);
    }

    uint32_t current_row = 1;
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
                            convert_to_csv_row(&csv->rows[current_row], device, stats, j);
                            current_row++;
                        }
                    }
                }
                current_stats = cls_linked_list_get_next(current_stats);
            }
        }
        current_device = cls_linked_list_get_next(current_device);
    }
    return csv;
}