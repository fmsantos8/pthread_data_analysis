#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "test/test.h"

#include "libs/csv/lib_csv.h"

typedef struct {
    char *filename;
    char separator;
} csv_config_t;

void *my_thread(void *arg) {

    // csv_config_t *config = (csv_config_t *)arg;

    // while (1) {

    //     if (lib_csv_init(config->filename, config->separator)) {
    //         printf("Thread: %s\n", get_string());
    //         char buffer[256];
    //         if (lib_csv_read_line(buffer, sizeof(buffer))) {
    //             printf("Thread: Read line: %s\n", buffer);
    //         }
    //         if (lib_csv_write_line("Sample data")) {
    //             printf("Thread: Wrote line: Sample data\n");
    //         }
    //         lib_csv_close();
    //     } else {
    //         printf("Thread: Failed to initialize CSV.\n");
    //     }
    //     sleep(1); // Simulate some work
    // }
    // printf("Thread finished.\n");
    return NULL;
}

int main(void) {

    // START
    time_t start_time = time(NULL);

    if (lib_csv_init("devices.csv", '|')) {
        printf("Main: CSV initialized successfully.\n");

        char *column_names[] = {"latitude", "longitude", "contagem"};
        uint32_t column_count = sizeof(column_names) / sizeof(column_names[0]);
        if (lib_csv_drop_columns("devices.csv", (const char **)column_names, column_count)) {
            printf("Main: Columns dropped successfully.\n");
        } else {
            printf("Main: Failed to drop column.\n");
        }

        lib_csv_close("devices.csv");

    } else {
        printf("Main: Failed to initialize CSV.\n");
        return 1;
    }

    time_t end_time = time(NULL);
    double elapsed_time = difftime(end_time, start_time);
    printf("Elapsed time: %.6f seconds\n", elapsed_time);
    // END

    return 0;
}