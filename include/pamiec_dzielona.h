#ifndef PAMIEC_DZIELONA_H
#define PAMIEC_DZIELONA_H

typedef struct {
    int K;            
    int gateTeam[3];  
    int gateCount[3]; 
} SharedData;

int create_new_shared_memory();
int get_shared_memory();
SharedData * join_shared_memory(int id_shared);
void detach_shared_memory(SharedData * address);
void delete_shared_memory(int id_shared);

#endif

