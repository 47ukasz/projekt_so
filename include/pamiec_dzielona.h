#ifndef PAMIEC_DZIELONA_H
#define PAMIEC_DZIELONA_H

typedef struct _Shared_data{
    int K;            
    int gateTeam[3]; // -1, 0, 1 
    int gateCount[3]; // 0, 1, 2, 3
} Shared_data;

int create_new_shared_memory();
int get_shared_memory();
Shared_data * join_shared_memory(int id_shared);
void detach_shared_memory(Shared_data * address);
void delete_shared_memory(int id_shared);

#endif

