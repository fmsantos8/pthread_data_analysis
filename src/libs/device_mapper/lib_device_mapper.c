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
        assert(measure != NULL); // Ensure measure creation was successful

        cls_device_add_measure(device, measure);
    }

    return devices;
}