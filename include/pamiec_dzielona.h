#ifndef PAMIEC_DZIELONA_H
#define PAMIEC_DZIELONA_H

typedef struct _Shared_data_fan {
    int K;
    int gateTeam[3]; // -1, 0, 1 
    int gateCount[3]; // 0, 1, 2, 3
    int entered_VIP;
    int entered_normal;
    int waiting_before_queue;
    int match_ended; // 0 | 1
} Shared_data_fan;

typedef struct _Shared_data_manager {
    pid_t employee_pid;
} Shared_data_manager;

int create_new_shared_memory(int key, int size);
int get_shared_memory(int key, int size);

Shared_data_fan * join_shared_memory_fan(int id_shared);
Shared_data_manager * join_shared_memory_manager(int id_shared);

void detach_shared_memory_fan(Shared_data_fan * address);
void detach_shared_memory_manager(Shared_data_manager * address);

void delete_shared_memory(int id_shared);

#endif