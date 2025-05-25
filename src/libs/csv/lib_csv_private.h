#ifndef __LIB_CSV_PRIVATE_H__
#define __LIB_CSV_PRIVATE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024 /** Maximum line length (bytes) */

static char *copy_field(const char *start, uint32_t size);

static char **get_fields(const char *line, char delimiter, uint32_t expected_fields, uint32_t *count);

static uint32_t get_number_of_columns(const char *line, char delimiter);

#endif