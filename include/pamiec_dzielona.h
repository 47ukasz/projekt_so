#ifndef PAMIEC_DZIELONA_H
#define PAMIEC_DZIELONA_H

int create_new_shared_memory();
int get_shared_memory();
char * join_shared_memory(int id_shared);
void detach_shared_memory(char * address);
void delete_shared_memory(int id_shared);

#endif

