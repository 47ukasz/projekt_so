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
#include <signal.h>
#include "pamiec_dzielona.h"

typedef enum {
    NORMAL_FAN,
    NORMAL_FAN_PRIORITY,
    NORMAL_WITH_CHILD,
    VIP_FAN
} Fan_type;

typedef struct _Fan {
    int fan_pid;
    int fan_team;
    int fan_type;
    int is_dangerous;
} Fan;

void handle_normal_fan(int id_sem, Shared_data_fan * data, Fan * fan_data, int K);
void handle_priority_fan(int id_sem, Shared_data_fan * data, Fan * fan_data, int K);
void handle_gate_control(int id_sem, Shared_data_fan * data, Fan * fan_data);

#endif