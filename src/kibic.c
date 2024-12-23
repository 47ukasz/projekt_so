#include <kibic.h>
#include <semafory.h>
#include <pamiec_dzielona.h>   

void create_new_thread(pthread_t * thread_id, Data_thread * data);
void join_thread(pthread_t thread_id);
void detach_thread(pthread_t thread_id);
void * calculate_time(void * _data);

int main() {
    int id_sem = get_access_semaphore();
    int id_shared = get_shared_memory();
    char * address = join_shared_memory(id_shared);

    int fanNumber;
    double p = 0.005;
    pid_t pid;

    handle_semaphore_p(id_sem, 1);
    fanNumber = * address;
    handle_semaphore_v(id_sem, 1);


    int probability = round(fanNumber / round(p * fanNumber)); // prawdopodobienstwo stworzenia procesu VIP (K >= 200)

    for (int i = 0; i < fanNumber; i++) {
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

            printf("Czas dzialania programu (PID=%d): %ld\n", getpid(), data.working_time);            

            detach_thread(thread_id);

            exit(EXIT_SUCCESS);
            break;
        }
        default:
            break;
        }
    }

    for (int i = 0; i < fanNumber; i++) {
        wait(NULL);
    }

    detach_shared_memory(address);

    return 0;
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