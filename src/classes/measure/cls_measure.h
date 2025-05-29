#ifndef __CLS_MEASURE_H__
#define __CLS_MEASURE_H__

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t month;
    uint16_t year;
} date_t;

typedef enum {
    SENSOR_TEMPERATURE = 0,
    SENSOR_HUMIDITY,
    SENSOR_LIGHT,
    SENSOR_NOISE,
    SENSOR_ECO2,
    SENSOR_ETVOC,
    SENSOR_MAX,
} sensor_type_t;

typedef struct {
    float values[SENSOR_MAX];
} sensor_readings_t;

typedef struct {
    date_t date;
    sensor_readings_t readings;
} measure_t;

measure_t *cls_measure_init(date_t *date, sensor_readings_t *data);

float cls_measure_reading_from_string(const char *string);

void cls_measure_date_from_string(date_t *date, const char *string);

void cls_measure_deinit(measure_t *measure);

static inline date_t *cls_measure_get_date(measure_t *measure) {
    if (!measure) {
        return NULL;
    }
    return &measure->date;
}

static inline sensor_readings_t *cls_measure_get_readings(measure_t *measure) {
    if (!measure) {
        return NULL;
    }
    return &measure->readings;
}

#endif