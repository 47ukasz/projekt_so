#ifndef KIBIC_H
#define KIBIC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

typedef struct _Data_thread {
    int running;
    long int working_time;
} Data_thread; 

#endif