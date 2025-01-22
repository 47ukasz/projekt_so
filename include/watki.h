#ifndef WATKI_H
#define WATKI_H

#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void create_new_thread(pthread_t * thread_id, void*(*handler_function)(void *), void * data);
void * calculate_time(void * _data);
void join_thread(pthread_t thread_id);
void detach_thread(pthread_t thread_id);
void * child_handler(void * _data);
void * handle_wait(void * _data);

typedef struct _Data_thread1 {
    int running;
    time_t working_time;
} Data_time_thread; 

#endif