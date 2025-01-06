#include "kibic.h"
#include "semafory.h"
#include "pamiec_dzielona.h"   
#include "watki.h"

int main() {
    int id_sem = get_access_semaphore();
    int id_shared = get_shared_memory();
    Shared_data * shared_data = join_shared_memory(id_shared);

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

            printf("Wartosc sem1: %d | PID: %d\n", get_semaphore_value(id_sem, 1), getpid());

            printf("Utworzony proces PID(%d) Druzyna: %d \n", getpid(), team);

            if (fan_type_local == NORMAL_WITH_CHILD) {
                create_new_thread(&thread_id_child, child_handler, NULL);
            }

            data.running = 1;

            create_new_thread(&thread_id_time, calculate_time, (void *) &data);

            handle_semaphore_p(id_sem, 0); // ustawienie w kolejce
            
            handle_gate_control(id_sem, team, shared_data);

            data.running = 0; // oznaczenia konca dzialania procesu
            join_thread(thread_id_time);
            
            if (fan_type_local == NORMAL_WITH_CHILD) {
                join_thread(thread_id_child);
            }

            //printf("Czas dzialania programu (PID=%d): %ld\n", getpid(), data.working_time);            

            detach_thread(thread_id_time);

            if (fan_type_local == NORMAL_WITH_CHILD) {
                detach_thread(thread_id_child);
            }
            
            handle_semaphore_v(id_sem, 0);

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
    printf("Kibic zakonczyl swoje dzialanie.\n");

    return 0;
}

void handle_gate_control(int id_sem, int id_team, Shared_data * data) {
    int id_gate = -1;

    while (id_gate == -1) {
        
        handle_semaphore_p(id_sem, 1);
        for (int i = 0; i < 3; i++)  {

            if (data->gateTeam[i] == -1) {
                id_gate = i;
                data->gateCount[i] = data->gateCount[i] + 1;
                data->gateTeam[i] = id_team;
                break;
            } else if (data->gateTeam[i] == id_team && data->gateCount[i] < 3) {
                id_gate = i;
                data->gateCount[i] = data->gateCount[i] + 1;
                break;
            }
        }
        handle_semaphore_v(id_sem, 1);

    }

    printf("Kibic %d wybral bramke | Bramka: %d | Druzyna %d | Liczba osob na bramce: %d | Bramka zajeta przez: %d\n", getpid(), id_gate, id_team, data->gateCount[id_gate], data->gateTeam[id_gate]);

    handle_semaphore_p(id_sem, id_gate + 2);

    printf("Kibic (PID=%d) przechodzi kontrole bezpieczenstwa na bramce %d\n", getpid(), id_gate);    
    sleep(2);

    handle_semaphore_v(id_sem, id_gate + 2);

    handle_semaphore_p(id_sem, 1); // pisanie

    if (data->gateCount[id_gate] == 1) {
        data->gateTeam[id_gate] = -1;
        data->gateCount[id_gate] = data->gateCount[id_gate] - 1;
    } else {
        data->gateCount[id_gate] = data->gateCount[id_gate] - 1;
    }

    handle_semaphore_v(id_sem, 1);

    printf("Kibic %d wchodzi na stadion | Bramka: %d | Druzyna %d | Status bramki: %d | wartosc sem1: %d \n", getpid(), id_gate, id_team, data->gateCount[id_gate], get_semaphore_value(id_sem, 1));
}