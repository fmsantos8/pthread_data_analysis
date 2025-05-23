#ifndef LIB_CSV_H
#define LIB_CSV_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    TYPE_INT = 0x00,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_DATE,
    TYPE_MAX,
} data_type_t;

typedef struct {
    data_type_t type;
    void *data;
    uint32_t size;
} field_data_t;

bool lib_csv_init(const char *filename, const char separator);
bool lib_csv_read_row(field_data_t *fields, uint32_t field_count);
bool lib_csv_write_row(field_data_t *fields, uint32_t field_count);
uint32_t lib_csv_get_row_count(void);
bool lib_csv_drop_columns(const char *filename, const char **column_names, uint32_t column_count);
bool lib_csv_close(const char *filename);
bool lib_csv_is_open(const char *filename);

#endif