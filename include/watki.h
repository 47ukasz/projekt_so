#ifndef WATKI_H
#define WATKI_H

#include <pthread.h>
#include <unistd.h>

void create_new_thread(pthread_t * thread_id, void*(*handler_function)(void *), void * data);
void * calculate_time(void * _data);
void join_thread(pthread_t thread_id);
void detach_thread(pthread_t thread_id);
void * child_handler(void * data);

typedef struct _Data_thread {
    int running;
    time_t working_time;
} Data_thread; 

#endif