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
#include "pamiec_dzielona.h"

typedef enum {
    NORMAL_FAN,
    NORMAL_WITH_CHILD,    
    VIP_FAN
} Fan_type;

// void handle_gate_control(int id_sem, int id_team);
void handle_gate_control(int id_sem, int id_team, Shared_data * data);

#endif