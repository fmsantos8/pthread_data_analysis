#ifndef LIB_CSV_H
#define LIB_CSV_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    char **fields;
    uint32_t field_count;
} csv_row_t;

typedef struct {
    csv_row_t *rows;
    uint32_t number_of_rows;
    uint32_t number_of_columns;
} csv_data_t;

csv_data_t *lib_csv_parse_file(const char *filename, char delimiter);

char *lib_csv_get_field(csv_data_t *data, uint32_t row, uint32_t column);

void lib_csv_free(csv_data_t *data);

bool lib_csv_write_file(const char *filename, const csv_data_t *data, char delimiter);

#endif