#include "lib_csv_private.h"

char *copy_field(const char *start, uint32_t size) {
    char *field = malloc(size + 1);
    if (field) {
        memcpy(field, start, size);
        field[size] = '\0';
    }
    return field;
}

char **get_fields(const char *line, char delimiter, uint32_t expected_fields, uint32_t *count) {
    char **fields = malloc(sizeof(char *) * expected_fields);
    uint32_t field_index = 0;
    const char *start = line;
    const char *pointer = start;

    while (*pointer && field_index < expected_fields - 1) {
        if (*pointer == delimiter) {
            fields[field_index++] = copy_field(start, pointer - start);
            start = pointer + 1;
        }
        pointer++;
    }

    if (field_index < expected_fields) { // Last field
        fields[field_index++] = copy_field(start, strlen(start));
    }

    *count = expected_fields;
    return fields;
}

uint32_t get_number_of_columns(const char *line, char delimiter) {
    uint32_t delimiters = 0;
    const char *pointer = line;

    while (*pointer) {
        if (*pointer == delimiter) {
            delimiters++;
        }
        pointer++;
    }
    return delimiters + 1; // The number of fields is the number of delimiters + 1
}