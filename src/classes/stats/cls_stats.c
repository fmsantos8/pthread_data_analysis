#include "cls_stats.h"

stats_t *cls_stats_init(date_t *date) {
    if (!date) {
        return NULL; // Handle null date case
    }

    stats_t *stats = (stats_t *)malloc(sizeof(stats_t));
    if (!stats) {
        return NULL; // Memory allocation failed
    }

    stats->date = *date;

    for (int i = 0; i < SENSOR_MAX; i++) {
        stats->average_sum[i] = 0.0f;
        stats->average_count[i] = 0;
        stats->min_value[i] = FLT_MAX;
        stats->average_value[i] = 0.0f;
        stats->max_value[i] = -FLT_MAX;
    }

    return stats;
}

void cls_stats_add_readings(stats_t *stats, sensor_readings_t *readings) {
    if (!stats || !readings) {
        return; // Handle null pointers
    }

    for (int i = 0; i < SENSOR_MAX; i++) {
        float reading = readings->values[i];
        if (reading == FLT_MAX) {
            continue; // Skip invalid readings
        }

        stats->average_sum[i] += reading;
        stats->average_count[i]++;

        if (reading < stats->min_value[i]) {
            stats->min_value[i] = reading;
        }
        if (reading > stats->max_value[i]) {
            stats->max_value[i] = reading;
        }
    }
}

bool cls_stats_is_same_month(stats_t *stats, date_t *date) {
    if (!stats || !date) {
        return false;
    }

    return (stats->date.month == date->month && stats->date.year == date->year);
}

float cls_stats_get_average_reading(stats_t *stats, sensor_type_t sensor) {
    if (!stats || sensor >= SENSOR_MAX) {
        return FLT_MAX;
    }

    for (int i = 0; i < SENSOR_MAX; i++) {
        if (stats->average_count[i] > 0) {
            stats->average_value[i] = stats->average_sum[i] / stats->average_count[i];
        }
    }

    return stats->average_value[sensor];
}

void cls_stats_deinit(stats_t *stats) {
    if (!stats) {
        return;
    }

    free(stats);
}
