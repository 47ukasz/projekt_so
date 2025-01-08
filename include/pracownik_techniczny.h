#ifndef PRACOWNIK_TECHNICZNY_H
#define PRACOWNIK_TECHNICZNY_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <string.h>

void handle_pause_signal(int signal);
void handle_resume_signal(int signal);
void handle_exit_signal(int signal);

#endif