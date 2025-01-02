#include "kibic.h"
#include "semafory.h"
#include "pamiec_dzielona.h"   
#include "watki.h"

void * calculate_time(void * _data);

int main() {
    int id_sem = get_access_semaphore();
    int id_shared = get_shared_memory();
    SharedData * shared_data = join_shared_memory(id_shared);

    int fan_type_global;
    int fan_number;
    double fan_vip_percentage = 0.005;
    double fan_child_percentage = 0.15;
    pid_t pid;

    handle_semaphore_p(id_sem, 1);
    fan_number = shared_data->K;
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
            srand(time(NULL) ^ getpid());
            pthread_t thread_id_time, thread_id_child;
            Data_thread data;
            int fan_type_local = fan_type_global;
            int team = rand() % 2;

            if (fan_type_local == NORMAL_WITH_CHILD) {
                create_new_thread(&thread_id_child, child_handler, NULL);
            }

            data.running = 1;

            create_new_thread(&thread_id_time, calculate_time, (void *) &data);

            handle_semaphore_p(id_sem, 0); // ustawienie w kolejce
            
            printf("Kibic (PID=%d) wchodzi na stadion\n", getpid());
            printf("Kibic (PID=%d) wychodzi ze stadionu\n", getpid());
            if (fan_type_local == NORMAL_WITH_CHILD) {
                join_thread(thread_id_child);
            }
            
            data.running = 0; // oznaczenia konca dzialania procesu
            join_thread(thread_id_time);

            printf("Czas dzialania programu (PID=%d): %ld\n", getpid(), (long)data.working_time);            

            detach_thread(thread_id_time);

            if (fan_type_local == NORMAL_WITH_CHILD) {
                detach_thread(thread_id_child);
            }

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

    detach_shared_memory(shared_data);

    return 0;
}