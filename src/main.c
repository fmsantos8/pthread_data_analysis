#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "test/test.h"

void* my_thread(void* arg) {
    while(1) {
        printf("Thread is running...\n");
        sleep(1);
    }
    return NULL;
}

int main() {
    printf("Main: %s\n", get_string()); 
    pthread_t thread;
    pthread_create(&thread, NULL, my_thread, NULL);
    sleep(5);
    //Requesting to cancel the thread after 5 seconds. 
    pthread_cancel(thread);
    // Wait for the thread to terminate
    pthread_join(thread, NULL);  

    printf("Main thread finished.\n");
    return 0;
}