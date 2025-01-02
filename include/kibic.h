#ifndef KIBIC_H
#define KIBIC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>

typedef enum {
    NORMAL_FAN,
    NORMAL_WITH_CHILD,    
    VIP_FAN
} Fan_type;

#endif