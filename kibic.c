#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>

#define K 30

int get_access_semaphore();
int get_semaphore_value(int id_sem, int sem_num);
void handle_semaphore_v(int id_sem, int sem_num);
void handle_semaphore_p(int id_sem, int sem_num);

int main() {
    int id_sem = get_access_semaphore();
    int value = get_semaphore_value(id_sem, 0);
    printf("Wartość na semaforze: %d\n", value);


    for (int i = 0; i < K; i++) {
        pid_t pid = fork();
        handle_semaphore_p(id_sem, 0);
        value = get_semaphore_value(id_sem, 0);

        printf("Wartość na semaforze: %d\n", value);

        if (pid == 0) {
            return 0; 
        } else if (pid > 0) {
            printf("Stworzyłem proces potomny o PID: %d\n", pid);
        } else {
            perror("fork failed");
            return 1;
        }
    }

    for (int i = 0; i < K; i++ ) {
        wait(NULL);
    }


    return 0;
}

int get_access_semaphore() {
    int id_sem = semget(47, 1, 0666);

    if (id_sem == -1) {
        printf("Nie mozna uzyskac dostepu do semafora.\n");
        exit(EXIT_FAILURE);
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

    if (semop(id_sem, &sem_bufor, 1) == -1) {
        perror("Błąd operacji P na semaforze");
        exit(EXIT_FAILURE);
    }
}

int get_semaphore_value(int id_sem, int sem_num) {
    int return_value = 300;

    return_value = semctl(id_sem, sem_num, GETVAL);
    
    // if (return_value == -1) {
    //     printf("Blad ustawiania wartosci semafora.\n");
    //     exit(EXIT_FAILURE);
    // }

    return return_value;
}