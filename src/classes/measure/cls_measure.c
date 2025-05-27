#include "cls_measure.h"

#include <assert.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

measure_t *cls_measure_init(date_t *date, sensor_readings_t *readings) {

    if (!date || !readings) {
        return NULL; // Handle null pointers
    }

    measure_t *measure = (measure_t *)malloc(sizeof(measure_t));
    if (!measure) {
        return NULL; // Memory allocation failed
    }

    measure->date = *date;
    measure->readings = *readings;

    return measure;
}

void cls_measure_deinit(measure_t *measure) {

    if (!measure) {
        return; // Handle null pointer
    }

    free(measure); // Free the allocated memory for measure
}

float cls_measure_reading_from_string(const char *string) {

    if (!string) {
        return FLT_MAX; // Handle null string case
    }

    char *endptr;
    float value = strtof(string, &endptr);

    if (endptr == string || *endptr != '\0') {
        return FLT_MAX; // Invalid conversion, return 0.0
    }

    return value;
}

void cls_measure_date_from_string(date_t *date, const char *string) {

    if (!string || !date) {
        return; // Handle null string case
    }

    // Get only the year and month from the string
    char year_str[5] = {0};
    char month_str[3] = {0};
    if (sscanf(string, "%4s-%2s", year_str, month_str) != 2) {
        return; // Invalid format, do nothing
    }
    date->year = (uint16_t)atoi(year_str);
    date->month = (uint8_t)atoi(month_str);

    assert(date->year >= 2024 && date->month >= 3); // TODO: REMOVE
}