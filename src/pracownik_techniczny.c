#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int create_new_semaphore();
void set_semaphore(int id_sem, int sem_num, int value);
void handle_semaphore_v(int id_sem, int sem_num);
void handle_semaphore_p(int id_sem, int sem_num);
void delete_semaphore(int id_sem);

int main() {
    int K;

    printf("Podaj liczbe kibicow majacych wejsc na stadion: ");
    scanf("%d", &K);

    if (K <= 0) {
        printf("Podana liczba kibicow jest nieprawidlowa!\n");
        return 1;
    }

    int id_sem = create_new_semaphore();
    set_semaphore(id_sem, 0, K);

    return 0;
}

int create_new_semaphore() {
    int id_sem = semget(47, 1, 0666|IPC_CREAT);

    if (id_sem == -1) {
        printf("Blad utworzenia nowego semafora.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Semafor zostal utworzony : %d\n", id_sem);
    }

    return id_sem;
}

void set_semaphore(int id_sem, int sem_num, int value) {
    int return_value = 0;

    return_value = semctl(id_sem, sem_num, SETVAL, value);
    
    if (return_value == -1) {
        printf("Blad ustawiania wartosci semafora.\n");
        exit(EXIT_FAILURE);
    }
}

void delete_semaphore(int id_sem) {
    int return_value;

    return_value = semctl(id_sem, 0, IPC_RMID);

    if (return_value == -1) {
        printf("Blad usuniecia semafora.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Semafor zostal usuniety: %d\n", id_sem);
    }
}