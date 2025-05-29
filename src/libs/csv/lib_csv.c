#include "lib_csv.h"
#include "lib_csv_private.h"

csv_data_t *lib_csv_parse_file(const char *filename, char delimiter) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    char line[MAX_LINE_LENGTH] = {};
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return NULL;
    }

    csv_data_t *data = calloc(1, sizeof(csv_data_t));
    data->number_of_columns = get_number_of_columns(line, delimiter);

    fseek(file, 0, SEEK_SET); // This is necessary to include the headers in the data

    while (fgets(line, sizeof(line), file)) {

        uint32_t length = strlen(line);
        while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r')) { // Remove newlines
            line[--length] = '\0';
        }

        csv_row_t row;
        row.fields = get_fields(line, delimiter, data->number_of_columns, &row.field_count);

        data->rows = realloc(data->rows, sizeof(csv_row_t) * (data->number_of_rows + 1));
        data->rows[data->number_of_rows++] = row;
    }
    fclose(file);
    return data;
}

char *lib_csv_get_field(csv_data_t *data, uint32_t row, uint32_t column) {
    if (row >= data->number_of_rows || column >= data->number_of_columns) {
        return NULL;
    }
    return data->rows[row].fields[column];
}

void lib_csv_free(csv_data_t *data) {
    for (uint32_t i = 0; i < data->number_of_rows; ++i) {
        for (uint32_t j = 0; j < data->rows[i].field_count; ++j)
            free(data->rows[i].fields[j]);
        free(data->rows[i].fields);
    }
    free(data->rows);
    free(data);
}

bool lib_csv_write_file(const char *filename, const csv_data_t *data, char delimiter) {

    FILE *file = fopen(filename, "w");
    if (!file) {
        return false;
    }

    for (uint32_t i = 0; i < data->number_of_rows; ++i) {

        csv_row_t row = data->rows[i];

        for (uint32_t column = 0; column < data->number_of_columns; ++column) {
            if (column < row.field_count && row.fields[column]) {
                fprintf(file, "%s", row.fields[column]);
            }
            if (column < data->number_of_columns - 1) {
                fputc(delimiter, file);
            }
        }
        fputc('\n', file);
    }

    fclose(file);
    return true;
}
