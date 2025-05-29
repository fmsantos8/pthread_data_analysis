#ifndef __CLS_STATS_H__
#define __CLS_STATS_H__

#include <float.h>
#include <stdbool.h>
#include <stdlib.h>

#include "classes/measure/cls_measure.h"

typedef struct {
    date_t date;

    float average_sum[SENSOR_MAX];
    uint32_t average_count[SENSOR_MAX];

    float min_value[SENSOR_MAX];
    float average_value[SENSOR_MAX];
    float max_value[SENSOR_MAX];
} stats_t;

stats_t *cls_stats_init(date_t *date);

void cls_stats_add_readings(stats_t *stats, sensor_readings_t *readings);

bool cls_stats_is_same_month(stats_t *stats, date_t *date);

void cls_stats_deinit(stats_t *stats);

static inline float cls_stats_get_minimum_reading(stats_t *stats, sensor_type_t sensor) {
    if (!stats || sensor >= SENSOR_MAX) {
        return FLT_MAX;
    }
    return stats->min_value[sensor];
}

float cls_stats_get_average_reading(stats_t *stats, sensor_type_t sensor);

static inline float cls_stats_get_maximum_reading(stats_t *stats, sensor_type_t sensor) {
    if (!stats || sensor >= SENSOR_MAX) {
        return FLT_MAX;
    }
    return stats->max_value[sensor];
}

#endif