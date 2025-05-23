#include "lib_csv.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CSV_MAX_FILES       10
#define MAX_LINE_SIZE       1024
#define MAX_FIELDS_PER_LINE 100

typedef struct {
    char filename[256];
    char separator;
    FILE *file;
    bool in_use;
} CsvFileEntry;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static CsvFileEntry csv_files[CSV_MAX_FILES];
static CsvFileEntry *active_file = NULL;

static CsvFileEntry *find_entry(const char *filename) {
    for (int i = 0; i < CSV_MAX_FILES; ++i) {
        if (csv_files[i].in_use && strcmp(csv_files[i].filename, filename) == 0) {
            return &csv_files[i];
        }
    }
    return NULL;
}

static CsvFileEntry *allocate_entry(const char *filename) {
    for (int i = 0; i < CSV_MAX_FILES; ++i) {
        if (!csv_files[i].in_use) {
            strncpy(csv_files[i].filename, filename, sizeof(csv_files[i].filename) - 1);
            csv_files[i].in_use = true;
            return &csv_files[i];
        }
    }
    return NULL;
}

bool lib_csv_init(const char *filename, const char separator) {
    pthread_mutex_lock(&mutex);

    CsvFileEntry *entry = find_entry(filename);
    if (!entry) {
        entry = allocate_entry(filename);
        if (!entry) {
            pthread_mutex_unlock(&mutex);
            return false;
        }
    }

    if (entry->file) {
        fclose(entry->file);
        entry->file = NULL;
    }

    entry->file = fopen(filename, "a+");
    if (!entry->file) {
        entry->in_use = false;
        pthread_mutex_unlock(&mutex);
        return false;
    }

    entry->separator = separator;
    active_file = entry;

    pthread_mutex_unlock(&mutex);
    return true;
}

static void split_line(char *line, char separator, char *fields[], uint32_t max_fields) {
    uint32_t count = 0;
    char *token = strtok(line, &separator);
    while (token && count < max_fields) {
        fields[count++] = token;
        token = strtok(NULL, &separator);
    }
}

bool lib_csv_read_row(field_data_t *fields, uint32_t field_count) {
    pthread_mutex_lock(&mutex);

    if (!active_file || !active_file->file) {
        pthread_mutex_unlock(&mutex);
        return false;
    }

    // Ensure reading starts at current position
    char line[MAX_LINE_SIZE];
    if (!fgets(line, sizeof(line), active_file->file)) {
        pthread_mutex_unlock(&mutex);
        return false;
    }

    // Remove newline
    line[strcspn(line, "\r\n")] = '\0';

    char *tokens[MAX_FIELDS_PER_LINE] = {0};
    split_line(line, active_file->separator, tokens, field_count);

    for (uint32_t i = 0; i < field_count; ++i) {
        if (!tokens[i])
            break;

        switch (fields[i].type) {
            case TYPE_INT:
                *((int *)fields[i].data) = atoi(tokens[i]);
                break;
            case TYPE_FLOAT:
                *((float *)fields[i].data) = strtof(tokens[i], NULL);
                break;
            case TYPE_STRING:
                strncpy((char *)fields[i].data, tokens[i], fields[i].size - 1);
                ((char *)fields[i].data)[fields[i].size - 1] = '\0';
                break;
            case TYPE_DATE:
                // Not implemented; fallback to string
                strncpy((char *)fields[i].data, tokens[i], fields[i].size - 1);
                ((char *)fields[i].data)[fields[i].size - 1] = '\0';
                break;
            default:
                break;
        }
    }

    pthread_mutex_unlock(&mutex);
    return true;
}

bool lib_csv_write_row(field_data_t *fields, uint32_t field_count) {
    pthread_mutex_lock(&mutex);

    if (!active_file || !active_file->file) {
        pthread_mutex_unlock(&mutex);
        return false;
    }

    for (uint32_t i = 0; i < field_count; ++i) {
        char buffer[256] = {0};

        switch (fields[i].type) {
            case TYPE_INT:
                snprintf(buffer, sizeof(buffer), "%d", *((int *)fields[i].data));
                break;
            case TYPE_FLOAT:
                snprintf(buffer, sizeof(buffer), "%f", *((float *)fields[i].data));
                break;
            case TYPE_STRING:
            case TYPE_DATE:
                snprintf(buffer, sizeof(buffer), "%s", (char *)fields[i].data);
                break;
            default:
                break;
        }

        fprintf(active_file->file, "%s", buffer);
        if (i < field_count - 1) {
            fprintf(active_file->file, "%c", active_file->separator);
        }
    }

    fprintf(active_file->file, "\n");
    fflush(active_file->file);

    pthread_mutex_unlock(&mutex);
    return true;
}

uint32_t lib_csv_get_row_count(void) {
    pthread_mutex_lock(&mutex);

    if (!active_file || !active_file->file) {
        pthread_mutex_unlock(&mutex);
        return 0;
    }

    fseek(active_file->file, 0, SEEK_SET);
    uint32_t count = 0;
    char line[MAX_LINE_SIZE];
    while (fgets(line, sizeof(line), active_file->file)) {
        count++;
    }

    // Reset file pointer for future reads
    fseek(active_file->file, 0, SEEK_SET);

    pthread_mutex_unlock(&mutex);
    return count - 1; // Exclude header
}

bool lib_csv_drop_columns(const char *filename, const char **column_names, uint32_t column_count) {
    if (!filename || !column_names || column_count == 0)
        return false;

    pthread_mutex_lock(&mutex);

    CsvFileEntry *entry = find_entry(filename);
    char separator = '|'; // Default separator

    if (entry && entry->file) {
        separator = entry->separator;
        fclose(entry->file);
        entry->file = NULL;
    }

    FILE *in = fopen(filename, "r");
    if (!in) {
        pthread_mutex_unlock(&mutex);
        return false;
    }

    char tmp_filename[300];
    snprintf(tmp_filename, sizeof(tmp_filename), "%s.tmp", filename);

    FILE *out = fopen(tmp_filename, "w");
    if (!out) {
        fclose(in);
        pthread_mutex_unlock(&mutex);
        return false;
    }

    char line[MAX_LINE_SIZE];

    // Read header
    if (!fgets(line, sizeof(line), in)) {
        fclose(in);
        fclose(out);
        remove(tmp_filename);
        pthread_mutex_unlock(&mutex);
        return false;
    }

    line[strcspn(line, "\r\n")] = '\0';

    char *fields[MAX_FIELDS_PER_LINE];
    uint32_t field_count = 0;
    char *p = line;
    char *start = p;

    while (*p) {
        if (*p == separator) {
            *p = '\0';
            fields[field_count++] = start;
            start = p + 1;
        }
        p++;
    }
    fields[field_count++] = start;

    // Determine which fields to drop
    bool drop_flags[MAX_FIELDS_PER_LINE] = {false};

    for (uint32_t i = 0; i < column_count; i++) {
        for (uint32_t j = 0; j < field_count; j++) {
            if (strcmp(fields[j], column_names[i]) == 0) {
                drop_flags[j] = true;
                break;
            }
        }
    }

    // Write header
    for (uint32_t i = 0, written = 0; i < field_count; ++i) {
        if (drop_flags[i])
            continue;
        if (written++)
            fputc(separator, out);
        fputs(fields[i], out);
    }
    fputc('\n', out);

    // Process data lines
    while (fgets(line, sizeof(line), in)) {
        line[strcspn(line, "\r\n")] = '\0';

        char *values[MAX_FIELDS_PER_LINE];
        uint32_t values_count = 0;
        p = line;
        start = p;

        while (*p) {
            if (*p == separator) {
                *p = '\0';
                values[values_count++] = start;
                start = p + 1;
            }
            p++;
        }
        values[values_count++] = start;

        for (uint32_t i = 0, written = 0; i < values_count; ++i) {
            if (drop_flags[i])
                continue;
            if (written++)
                fputc(separator, out);
            fputs(values[i], out);
        }
        fputc('\n', out);
    }

    fclose(in);
    fclose(out);

    // Replace original file
    remove(filename);
    rename(tmp_filename, filename);

    // Reopen if file was managed
    if (entry) {
        entry->file = fopen(filename, "a+");
        if (!entry->file) {
            entry->in_use = false;
            if (active_file == entry)
                active_file = NULL;
        }
    }

    pthread_mutex_unlock(&mutex);
    return true;
}

bool lib_csv_close(const char *filename) {
    pthread_mutex_lock(&mutex);

    CsvFileEntry *entry = find_entry(filename);
    if (entry && entry->file) {
        fclose(entry->file);
        entry->file = NULL;
        entry->in_use = false;
        if (active_file == entry) {
            active_file = NULL;
        }
        pthread_mutex_unlock(&mutex);
        return true;
    }

    pthread_mutex_unlock(&mutex);
    return false;
}

bool lib_csv_is_open(const char *filename) {
    pthread_mutex_lock(&mutex);
    CsvFileEntry *entry = find_entry(filename);
    bool is_open = (entry && entry->file != NULL);
    pthread_mutex_unlock(&mutex);
    return is_open;
}
