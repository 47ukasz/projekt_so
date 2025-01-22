#ifndef SEMAFORY_H
#define SEMAFORY_H

int create_new_semaphore();
void set_semaphore(int id_sem, int sem_num, int value);
void handle_semaphore_v(int id_sem, int sem_num);
void handle_semaphore_p(int id_sem, int sem_num);
void handle_semaphore_pn(int id_sem, int sem_num, int value);
void handle_semaphore_vn(int id_sem, int sem_num, int value);
void delete_semaphore(int id_sem);
int get_access_semaphore();
int get_semaphore_value(int id_sem, int sem_num);
int get_process_waiting_before_p(int id_sem, int sem_num);

#endif