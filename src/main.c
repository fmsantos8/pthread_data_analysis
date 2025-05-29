#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "classes/device/cls_device.h"
#include "classes/linked_list/cls_linked_list.h"

#include "libs/csv/lib_csv.h"
#include "libs/device_mapper/lib_device_mapper.h"

static pthread_mutex_t device_mutex = PTHREAD_MUTEX_INITIALIZER;

static linked_list_t *device_queue = NULL;
static node_t *current_device_node = NULL;

device_t *get_next_device_to_process(void) {
    pthread_mutex_lock(&device_mutex);

    if (device_queue == NULL) {
        pthread_mutex_unlock(&device_mutex);
        return NULL; // No devices to process
    }

    if (current_device_node == NULL) {
        current_device_node = cls_linked_list_get(device_queue, 0);
    } else {
        current_device_node = cls_linked_list_get_next(current_device_node);
    }

    if (current_device_node == NULL) {
        pthread_mutex_unlock(&device_mutex);
        return NULL; // No more devices to process
    }

    device_t *device = (device_t *)cls_linked_list_get_data(current_device_node);
    pthread_mutex_unlock(&device_mutex);
    return device; // Return the next device to process
}

void *processing_thread(void *arg) {
    (void)arg;

    while (1) {
        device_t *device = get_next_device_to_process();
        if (device == NULL) {
            break; // No more devices to process
        }

        cls_device_calculate_average_readings(device);
    }

    printf("Thread %ld finished processing devices.\n", pthread_self());
    pthread_exit(NULL);
}

int main(void) {

    long cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (cores < 1) {
        return 1;
    }

    pthread_mutex_init(&device_mutex, NULL);

    csv_data_t *data = lib_csv_parse_file("devices_clean.csv", '|');
    if (!data) {
        return 1;
    }

    linked_list_t *devices = lib_device_mapper_from_csv(data);
    lib_csv_free(data);

    device_queue = devices;

    pthread_t threads[cores];
    for (long i = 0; i < cores; ++i) {
        if (pthread_create(&threads[i], NULL, processing_thread, NULL) != 0) {
            printf("Failed to create thread %ld.\n", i);
            cls_linked_list_deinit(devices);
            return 1;
        } else {
            printf("Thread %ld created successfully.\n", i);
        }
    }

    for (long i = 0; i < cores; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("Failed to join thread %ld.\n", i);
            cls_linked_list_deinit(devices);
            return 1;
        }
    }

    printf("All threads finished processing.\n");
    data = lib_device_mapper_to_csv(devices);

    // TODO: FREE DEVICES

    if (!data) {
        return 1;
    }

    if (!lib_csv_write_file("devices_processed.csv", data, ';')) {
        printf("Failed to write CSV file.\n");
        lib_csv_free(data);
        return 1;
    }

    return 0;
}