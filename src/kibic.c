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
    int * address = join_shared_memory(id_shared);

    int fan_type_global;
    int fan_number;
    double fan_vip_percentage = 0.005;
    double fan_child_percentage = 0.15;
    pid_t pid;

    handle_semaphore_p(id_sem, 1);
    fan_number = * address;
    handle_semaphore_v(id_sem, 1);

    int probability_vip = round(fan_number / round(fan_vip_percentage * fan_number)); // prawdopodobienstwo stworzenia procesu VIP (K >= 200)
    int probability_child = round(fan_number / round(fan_child_percentage * fan_number)); // prawdopodobienstwo stworzenia procesu z dzieckiem (K >= 200)

    printf("Tworzenie procesow kibicow...\n");

    for (int i = 0; i < fan_number; i++) {
        pid = fork(); 
        
        if (i % probability_vip == 0) {
            fan_type_global = VIP_FAN;
        } else if (i % probability_child == 0) {
            fan_type_global = NORMAL_WITH_CHILD;
        } else {
            fan_type_global = NORMAL_FAN;
        }

        switch (pid) {
        case -1:
            printf("Blad utworzenia nowego procesu!\n");
            perror("fork");
            exit(EXIT_FAILURE);    
            break;
        case 0: {
            pthread_t thread_id;
            Data_thread data;
            int fan_type_local = fan_type_global;

            if (fan_type_local == NORMAL_WITH_CHILD) {
                printf("Kibic %d z dzieckiem\n", getpid());
            } else if (fan_type_local == VIP_FAN) {
                printf("Kibic VIP %d\n", getpid());
            } else {
                printf("Kibic (normalny) %d\n", getpid());
            }

            data.running = 1;

            create_new_thread(&thread_id, &data);

            printf("Kibic %d czeka na wejscie do kolejki!\n", getpid());

            handle_semaphore_p(id_sem, 0);
            printf("Kibic %d wszedł na stadion!\n", getpid());

            sleep(1);

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

    for (int i = 0; i < fan_number; i++) {
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