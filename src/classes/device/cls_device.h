#ifndef __CLS_DEVICE_H__
#define __CLS_DEVICE_H__

#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "classes/linked_list/cls_linked_list.h"
#include "classes/measure/cls_measure.h"
#include "classes/stats/cls_stats.h"

typedef struct {
    char name[32];

    linked_list_t *measures;
    linked_list_t *stats;

} device_t;

device_t *cls_device_init(const char *name);

void cls_device_add_measure(device_t *device, measure_t *measure);
void cls_device_calculate_average_readings(device_t *device);
float cls_device_get_minimum_reading(const device_t *device, sensor_type_t sensor, date_t *date);
float cls_device_get_average_reading(const device_t *device, sensor_type_t sensor, date_t *date);
float cls_device_get_maximum_reading(const device_t *device, sensor_type_t sensor, date_t *date);

#endif
