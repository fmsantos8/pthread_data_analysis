#ifndef __LIB_DEVICE_MAPPER_PRIVATE_H__
#define __LIB_DEVICE_MAPPER_PRIVATE_H__

#include <assert.h>
#include <stdbool.h>

#include "classes/device/cls_device.h"
#include "classes/linked_list/cls_linked_list.h"

#include "libs/csv/lib_csv.h"

typedef enum {

    NAME_COLUMN = 0,
    DATE_COLUMN = 1,
    TEMPERATURE_COLUMN = 2,
    HUMIDITY_COLUMN = 3,
    LIGHT_COLUMN = 4,
    NOISE_COLUMN = 5,
    ECO2_COLUMN = 6,
    ETVOC_COLUMN = 7,

} device_mapper_column_t;

device_t *get_device(linked_list_t *devices, const char *name);
char *sensor_type_to_string(sensor_type_t sensor);
uint32_t get_number_of_rows(linked_list_t *devices);
void convert_to_csv_row(csv_row_t *row, device_t *device, stats_t *stats, sensor_type_t sensor);

#endif