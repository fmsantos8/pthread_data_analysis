#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "classes/device/cls_device.h"
#include "classes/linked_list/cls_linked_list.h"

#include "libs/csv/lib_csv.h"
#include "libs/device_mapper/lib_device_mapper.h"

static char *sensor_type_to_string(sensor_type_t sensor) {
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
            return "Unknown Sensor";
    }
}

int main(void) {

    csv_data_t *data = lib_csv_parse_file("devices_clean.csv", '|');
    if (!data) {
        printf("Failed to parse CSV file.\n");
        return 1;
    }

    linked_list_t *devices = lib_device_mapper_from_csv(data);

    uint32_t number_of_devices = cls_linked_list_size(devices);
    printf("Number of devices: %u\n", number_of_devices);

    FILE *output_file = fopen("output.csv", "w");
    if (!output_file) {
        printf("Failed to open output file.\n");
        cls_linked_list_free(devices);
        return 1;
    }
    fprintf(output_file, "device;ano-mes;sesnor;valor_maximo;valor_medio;valor_minimo\n");

    node_t *current_device = cls_linked_list_get(devices, 0);
    while (current_device != NULL) {
        device_t *device = (device_t *)cls_linked_list_get_data(current_device);
        if (device) {

            cls_device_calculate_average_readings(device);

            node_t *current_stats = cls_linked_list_get(device->stats, 0);
            while (current_stats != NULL) {
                stats_t *stats = (stats_t *)cls_linked_list_get_data(current_stats);

                if (stats) {
                    for (uint32_t j = 0; j < SENSOR_MAX; ++j) {
                        if (stats->average_count[j] > 0) {

                            float min_reading = cls_device_get_minimum_reading(device, j, &stats->date);
                            float avg_reading = cls_device_get_average_reading(device, j, &stats->date);
                            float max_reading = cls_device_get_maximum_reading(device, j, &stats->date);
                            printf("%s;%u-%02u;%s;%.2f;%.2f;%.2f\n", device->name, stats->date.year, stats->date.month, sensor_type_to_string(j), max_reading, avg_reading, min_reading);
                            fprintf(output_file, "%s;%u-%02u;%s;%.2f;%.2f;%.2f\n", device->name, stats->date.year, stats->date.month, sensor_type_to_string(j), max_reading, avg_reading, min_reading);
                        }
                    }
                }
                current_stats = cls_linked_list_get_next(current_stats);
            }
        }
        current_device = cls_linked_list_get_next(current_device);
    }

    fclose(output_file);

    return 0;
}