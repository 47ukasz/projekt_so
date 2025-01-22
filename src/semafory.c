#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>  

int create_new_semaphore() {
    int id_sem = semget(47, 11, 0666 | IPC_CREAT);

    if (id_sem == -1) {
        perror("Blad utworzenia nowego semafora (semget)");
        exit(EXIT_FAILURE);
    } else {
        printf("Semafor zostal utworzony: %d\n", id_sem);
    }

    return id_sem;
}

void set_semaphore(int id_sem, int sem_num, int value) {
    int return_value = semctl(id_sem, sem_num, SETVAL, value);

    if (return_value == -1) {
        perror("Blad ustawiania wartosci semafora (semctl SETVAL)");
        exit(EXIT_FAILURE);
    }
}

void delete_semaphore(int id_sem) {
    int return_value = semctl(id_sem, 0, IPC_RMID);

    if (return_value == -1) {
        perror("Blad usuniecia semafora (semctl IPC_RMID)");
        exit(EXIT_FAILURE);
    } else {
        printf("Semafor zostal usuniety: %d\n", id_sem);
    }
}

int get_access_semaphore() {
    int id_sem = semget(47, 11, 0666);

    if (id_sem == -1) {
        perror("Nie mozna uzyskac dostepu do semafora (semget)");
        exit(EXIT_FAILURE);
    } else {
        printf("Dostep do semafora zostal uzyskany: %d\n", id_sem);
    }

    return id_sem;
}

void handle_semaphore_v(int id_sem, int sem_num) {
    struct sembuf sem_bufor;
    sem_bufor.sem_num = sem_num;
    sem_bufor.sem_op  = 1;  // V
    sem_bufor.sem_flg = 0;

    if (semop(id_sem, &sem_bufor, 1) == -1) {
        perror("Blad operacji V na semaforze (semop)");
        exit(EXIT_FAILURE);
    }
}

void handle_semaphore_p(int id_sem, int sem_num) {
    struct sembuf sem_bufor;
    sem_bufor.sem_num = sem_num;
    sem_bufor.sem_op  = -1; // P
    sem_bufor.sem_flg = 0;

    if (semop(id_sem, &sem_bufor, 1) == -1) {
        perror("Blad operacji P na semaforze (semop)");
        exit(EXIT_FAILURE);
    }
}

int get_semaphore_value(int id_sem, int sem_num) {
    int return_value = semctl(id_sem, sem_num, GETVAL);

    if (return_value == -1) {
        perror("Blad pobierania wartosci semafora (semctl GETVAL)");
        exit(EXIT_FAILURE);
    }

    return return_value;
}

void handle_semaphore_pn(int id_sem, int sem_num, int value) {
    struct sembuf sem_bufor;
    sem_bufor.sem_num = sem_num;
    sem_bufor.sem_op  = -value; 
    sem_bufor.sem_flg = 0;

    if (semop(id_sem, &sem_bufor, 1) == -1) {
        perror("Blad operacji P(n) na semaforze (semop)");
        exit(EXIT_FAILURE);
    }
}

void handle_semaphore_vn(int id_sem, int sem_num, int value) {
    struct sembuf sem_bufor;
    sem_bufor.sem_num = sem_num;
    sem_bufor.sem_op  = value; 
    sem_bufor.sem_flg = 0;

    if (semop(id_sem, &sem_bufor, 1) == -1) {
        perror("Blad operacji V(n) na semaforze (semop)");
        exit(EXIT_FAILURE);
    }
}

int get_process_waiting_before_p(int id_sem, int sem_num) {
    int return_value = semctl(id_sem, sem_num, GETNCNT);

    if (return_value == -1) {
        perror("Blad pobierania liczby oczekujacych procesow (semctl GETNCNT)");
        exit(EXIT_FAILURE);
    }

    return return_value;
}
