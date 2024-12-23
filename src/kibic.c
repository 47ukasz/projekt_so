#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define K 20

typedef struct _Data_thread {
    int running;
    long int working_time;
} Data_thread; 

int get_access_semaphore();
int get_semaphore_value(int id_sem, int sem_num);
void handle_semaphore_v(int id_sem, int sem_num);
void handle_semaphore_p(int id_sem, int sem_num);

void create_new_thread(pthread_t * thread_id, Data_thread * data);
void join_thread(pthread_t thread_id);
void detach_thread(pthread_t thread_id);
void * calculate_time(void * _data);

int main() {
    int id_sem = get_access_semaphore();
    // int value = get_semaphore_value(id_sem, 0);
    double p = 0.005;
    pid_t pid;

    int probability = round(K / round(p * K)); // prawdopodobienstwo stworzenia procesu VIP (K >= 200)
    //printf("Wartość na semaforze: %d\n", value);

    for (int i = 0; i < K; i++) {
        pid = fork(); // Dodać losowe tworzenie procesow z dzieckiem, VIP'ow
        
        // if (i % probability == 0) {
        //     printf("Proces VIP");
        // }

        switch (pid) {
        case -1:
            printf("Blad utworzenia nowego procesu!\n");
            perror("fork");
            exit(EXIT_FAILURE);    
            break;
        case 0: {
            pthread_t thread_id;
            Data_thread data;

            data.running = 1;

            create_new_thread(&thread_id, &data);

            printf("Kibic %d czeka na wejscie do kolejki!\n", getpid());

            handle_semaphore_p(id_sem, 0);
            printf("Kibic %d wszedł na stadion!\n", getpid());

            sleep(5);
            data.running = 0; // oznaczenia konca dzialania procesu

            join_thread(thread_id);

            printf("Czas dzialania programu (PID=%d): %ld", getpid(), data.working_time);            

            detach_thread(thread_id);

            exit(EXIT_SUCCESS);
            break;
        }
        default:
            break;
        }
    }

    for (int i = 0; i < K; i++) {
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
        perror("Blad operacji P na semaforze");
        exit(EXIT_FAILURE);
    }
}

int get_semaphore_value(int id_sem, int sem_num) {
    int return_value = 300;

    return_value = semctl(id_sem, sem_num, GETVAL);

    return return_value;
}

void create_new_thread(pthread_t * thread_id, Data_thread * data) {
    int return_value;

    return_value = pthread_create(thread_id, NULL, calculate_time, (void *)data);

    if (return_value == -1) {
        printf("Blad utworzenia nowego watku!\n");
        perror("blad - watek (utworzenie)");
        exit(EXIT_FAILURE);
    }
}

void * calculate_time(void * _data) {
    Data_thread * data = (Data_thread *)_data;
    time_t start_time;
    time_t end_time;

    time(&start_time);

    while (data->running) {
        time(&end_time);
        sleep(1);        
    }

    data->working_time = end_time - start_time;

    pthread_exit(0);
}

void join_thread(pthread_t thread_id) {
    int return_value;

    return_value = pthread_join(thread_id, NULL);

    if (return_value == -1) {
        printf("Blad dolaczania watku!\n");
        perror("blad - watek (dolaczenie)");
        exit(EXIT_FAILURE);
    }
}

void detach_thread(pthread_t thread_id) {
    int return_value;

    return_value = pthread_detach(thread_id);

    if (return_value == -1) {
        printf("Blad odolaczania (usuwania) watku!\n");
        perror("blad - watek (odlaczanie)");
        exit(EXIT_FAILURE);
    }
}