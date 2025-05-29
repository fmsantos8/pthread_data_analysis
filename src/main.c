#include <pthread.h>
#include <unistd.h>

#include "classes/device/cls_device.h"
#include "classes/linked_list/cls_linked_list.h"

#include "libs/csv/lib_csv.h"
#include "libs/device_mapper/lib_device_mapper.h"

static pthread_mutex_t device_mutex = PTHREAD_MUTEX_INITIALIZER;

static linked_list_t *device_list = NULL;
static node_t *current_device_node = NULL;

static void free_device_data(linked_list_t *devices) {
    if (!devices) {
        return;
    }

    node_t *current = cls_linked_list_get(devices, 0);
    while (current != NULL) {
        device_t *device = (device_t *)cls_linked_list_get_data(current);
        cls_device_deinit(device);
        current = cls_linked_list_get_next(current);
    }
    cls_linked_list_deinit(devices);
}

static device_t *get_next_device_to_process(void) {
    pthread_mutex_lock(&device_mutex);

    if (device_list == NULL) {
        pthread_mutex_unlock(&device_mutex);
        return NULL; // No devices to process
    }

    if (current_device_node == NULL) {
        current_device_node = cls_linked_list_get(device_list, 0);
    } else {
        current_device_node = cls_linked_list_get_next(current_device_node);
    }

    if (current_device_node == NULL) {
        pthread_mutex_unlock(&device_mutex);
        device_list = NULL;
        return NULL; // No more devices to process
    }

    device_t *device = (device_t *)cls_linked_list_get_data(current_device_node);
    pthread_mutex_unlock(&device_mutex);
    return device; // Return the next device to process
}

void *processing_thread(void *arg) {
    uint32_t thread_id = (*(uint32_t *)arg);

    while (1) {
        device_t *device = get_next_device_to_process();
        if (device == NULL) {
            break; // No more devices to process
        }

        printf("Thread %u: Processando dispositivo: %s\n", thread_id, device->name);
        cls_device_process_readings(device);
    }

    pthread_exit(NULL);
}

int main(void) {

    uint32_t cores = (uint32_t)sysconf(_SC_NPROCESSORS_ONLN);
    if (cores < 1) {
        return 1;
    }

    pthread_mutex_init(&device_mutex, NULL);

    printf("Lendo o arquivo CSV...\n");
    csv_data_t *data = lib_csv_parse_file("devices_clean.csv", '|');
    if (!data) {
        return 1;
    }

    printf("Convertendo CSV para lista de dispositivos...\n");
    linked_list_t *devices = lib_device_mapper_from_csv(data);
    lib_csv_free(data);

    device_list = devices;

    printf("O número de cores disponíveis é: %u, criando %u threads...\n", cores, cores);
    pthread_t threads[cores];
    for (uint32_t i = 0; i < cores; ++i) {
        if (pthread_create(&threads[i], NULL, processing_thread, (void *)&i) != 0) {
            free_device_data(devices);
            return 1;
        }
    }

    for (uint32_t i = 0; i < cores; ++i) {
        printf("Aguardando thread %u...\n", i);
        if (pthread_join(threads[i], NULL) != 0) {
            free_device_data(devices);
            return 1;
        }
    }

    printf("Processamento concluído. Convertendo lista de dispositivos para csv_data_t...\n");
    data = lib_device_mapper_to_csv(devices);

    free_device_data(devices);

    if (!data) {
        return 1;
    }

    printf("Escrevendo dados processados no arquivo CSV...\n");
    if (!lib_csv_write_file("devices_output.csv", data, ';')) {
        return 1;
    }

    lib_csv_free(data);
    return 0;
}