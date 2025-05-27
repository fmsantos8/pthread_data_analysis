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

int main(void) {

    // START
    time_t start_time = time(NULL);

    csv_data_t *data = lib_csv_parse_file("sample_data_2_clean.csv", '|');
    if (!data) {
        printf("Failed to parse CSV file.\n");
        return 1;
    }

    linked_list_t *devices = lib_device_mapper_from_csv(data);

    uint32_t number_of_devices = cls_linked_list_size(devices);

    printf("Number of devices: %u\n", number_of_devices);
    for (uint32_t i = 0; i < number_of_devices; ++i) {
        device_t *device = (device_t *)cls_linked_list_get_data(devices, i);
        if (device) {
            printf("Device Name: %s\n", device->name);

            uint32_t number_of_measures = cls_linked_list_size(device->measures);
            printf("Number of measures: %u\n", number_of_measures);

            for (uint32_t j = 0; j < number_of_measures; ++j) {
                measure_t *measure = (measure_t *)cls_linked_list_get_data(device->measures, j);
                if (measure) {
                    printf("Measure %u: Date: %u-%02u, Readings: [", j + 1, measure->date.year, measure->date.month);
                    for (uint32_t k = 0; k < SENSOR_MAX; ++k) {
                        printf("%.1f", measure->readings.values[k]);
                        if (k < SENSOR_MAX - 1) {
                            printf(", ");
                        }
                    }
                    printf("]\n");
                }
            }
        }
    }

    return 0;
}