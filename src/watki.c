#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "watki.h"

void create_new_thread(pthread_t * thread_id, void*(*handler_function)(void *), void * data) {
    int return_value;

    return_value = pthread_create(thread_id, NULL, handler_function, (void *)data);

    if (return_value == -1) {
        printf("Blad utworzenia nowego watku!\n");
        perror("blad - watek (utworzenie)");
        exit(EXIT_FAILURE);
    }
}

void join_thread(pthread_t thread_id) {
    int return_value;

    return_value = pthread_join(thread_id, NULL);

    if (return_value == -1) {
        printf("Blad dolaczania watku!\n");
        perror("blad - watek (dolaczenie)");
        exit(EXIT_FAILURE);
    }
}

void detach_thread(pthread_t thread_id) {
    int return_value;

    return_value = pthread_detach(thread_id);

    if (return_value == -1) {
        printf("Blad odolaczania (usuwania) watku!\n");
        perror("blad - watek (odlaczanie)");
        exit(EXIT_FAILURE);
    }
}

void * child_handler(void * data) {
    printf("Kibic %d z dzieckiem\n", getpid());

    pthread_exit(0);
}

void * calculate_time(void * _data) {
    Data_thread * data = (Data_thread *)_data;
    time_t start_time;
    time_t end_time = 10;

    time(&start_time);

    while (data->running) {
        time(&end_time);
        sleep(1);        
    }

    data->working_time = end_time - start_time;

    pthread_exit(0);
}