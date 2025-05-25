#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "libs/csv/lib_csv.h"

int main(void) {

    // START
    time_t start_time = time(NULL);

    csv_data_t *data = lib_csv_parse_file("devices_clean.csv", '|');

    if (!data) {
        printf("Erro ao abrir arquivo.\n");
        return 1;
    } else {
        printf("Arquivo aberto com sucesso.\n");
    }

    // for (uint32_t i = 0; i < data->number_of_rows; ++i) {
    //     for (uint32_t j = 0; j < data->number_of_columns; ++j) {
    //         printf("[%s] ", lib_csv_get_field(data, i, j));
    //     }
    //     printf("\n");
    // }

    // Remove the devices_clean.csv file
    if (remove("devices_clean.csv") == 0) {
        printf("Arquivo removido com sucesso.\n");
    } else {
        printf("Erro ao remover arquivo.\n");
        return 1;
    }

    lib_csv_write_file("devices_clean.csv", data, '|');

    lib_csv_free(data);

    time_t end_time = time(NULL);
    double elapsed_time = difftime(end_time, start_time);
    printf("Elapsed time: %.6f seconds\n", elapsed_time);
    // END

    return 0;
}