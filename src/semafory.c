#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int create_new_semaphore() {
    int id_sem = semget(47, 5, 0666|IPC_CREAT);

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

int get_access_semaphore() {
    int id_sem = semget(47, 5, 0666);

    if (id_sem == -1) {
        printf("Nie mozna uzyskac dostepu do semafora.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Dostep do semafora zostal uzyskany: %d\n", id_sem);
    }

    return id_sem;
}

void handle_semaphore_v(int id_sem, int sem_num) {
    int return_value;
    struct sembuf sem_bufor;
    sem_bufor.sem_num = sem_num;
    sem_bufor.sem_op = 1;
    sem_bufor.sem_flg = SEM_UNDO;

    return_value = semop(id_sem, &sem_bufor, 1);

    if (return_value == -1) {
        printf("Blad otwarcia semafora.\n");
        exit(EXIT_FAILURE);
    }
}

void handle_semaphore_p(int id_sem, int sem_num) {
    int return_value;
    struct sembuf sem_bufor;
    sem_bufor.sem_num = sem_num;
    sem_bufor.sem_op = -1;
    sem_bufor.sem_flg = 0;

    return_value = semop(id_sem, &sem_bufor, 1);

    if (return_value == -1) {
        perror("Blad operacji P na semaforze");
        exit(EXIT_FAILURE);
    }
}

int get_semaphore_value(int id_sem, int sem_num) {
    int return_value = 300;

    return_value = semctl(id_sem, sem_num, GETVAL);

    return return_value;
}

void PN(int id_sem, int sem_num, int value) {
    int return_value;
    struct sembuf sem_bufor;
    sem_bufor.sem_num = sem_num;
    sem_bufor.sem_op = -value;
    sem_bufor.sem_flg = 0;

    return_value = semop(id_sem, &sem_bufor, 1);

    if (return_value == -1) {
        perror("Blad operacji P na semaforze");
        exit(EXIT_FAILURE);
    }
}

void VN(int id_sem, int sem_num, int value) {
    int return_value;
    struct sembuf sem_bufor;
    sem_bufor.sem_num = sem_num;
    sem_bufor.sem_op = value;
    sem_bufor.sem_flg = 0;

    return_value = semop(id_sem, &sem_bufor, 1);

    if (return_value == -1) {
        perror("Blad operacji V na semaforze");
        exit(EXIT_FAILURE);
    }
}