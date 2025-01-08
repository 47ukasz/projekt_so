#include "kibic.h"
#include "semafory.h"
#include "pamiec_dzielona.h"   
#include "watki.h"

int main() {
    int id_sem = get_access_semaphore();
    int id_shared = get_shared_memory(48, sizeof(Shared_data_fan));
    Shared_data_fan * shared_data = join_shared_memory_fan(id_shared);
    int fan_number;

    double fan_vip_percentage = 0.005;
    double fan_child_percentage = 0.15;
    pid_t pid;

    handle_semaphore_p(id_sem, 1);
    fan_number = shared_data->K;
    handle_semaphore_v(id_sem, 1);

    int probability_vip = round(fan_number / round(fan_vip_percentage * fan_number)); // prawdopodobienstwo stworzenia procesu VIP (K >= 200)
    int probability_child = round(fan_number / round(fan_child_percentage * fan_number)); // prawdopodobienstwo stworzenia procesu z dzieckiem (K >= 200)

    sleep(5);

    for (int i = 0; i < fan_number; i++) {
        pid = fork(); 

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
            int fan_type;
            int team = rand() % 2;

            if (i % probability_vip == 0) {
                fan_type = VIP_FAN;
            } else if (i % probability_child == 0) {
                fan_type = NORMAL_WITH_CHILD;
            } else {
                fan_type = NORMAL_FAN;
            }

            if (i == 6 || i == 8) {
                fan_type = NORMAL_FAN_PRIORITY;
            }

            printf("Utworzono nowego Kibica | PID: %d | Druzyna: %d | Typ: %d\n", getpid(), team, fan_type);

            if (fan_type == NORMAL_WITH_CHILD) {
                create_new_thread(&thread_id_child, child_handler, NULL);
            }

            data.running = 1;

            create_new_thread(&thread_id_time, calculate_time, (void *) &data);

            handle_semaphore_p(id_sem, 0); // ustawienie w kolejce
            
            sleep(rand() % 5 + 2);

            handle_semaphore_p(id_sem, 9);

            if (fan_type == NORMAL_FAN || fan_type == NORMAL_WITH_CHILD) {
                handle_normal_fan(id_sem, team, shared_data, fan_number, fan_type);
            } else if (fan_type == NORMAL_FAN_PRIORITY) {
                handle_priority_fan(id_sem, team, shared_data, fan_number, fan_type);
            }

            if (fan_type == VIP_FAN) {
                handle_semaphore_p(id_sem, 10);
            } else {
                handle_semaphore_p(id_sem, 8);
            }

            printf("Kibic %d wychodzi ze stadionu\n", getpid());

            data.running = 0; // oznaczenia konca dzialania procesu
            join_thread(thread_id_time);
            
            if (fan_type == NORMAL_WITH_CHILD) {
                join_thread(thread_id_child);
            }

            //printf("Czas dzialania programu (PID=%d): %ld\n", getpid(), data.working_time);            

            detach_thread(thread_id_time);

            if (fan_type == NORMAL_WITH_CHILD) {
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

    detach_shared_memory_fan(shared_data);
    printf("Kibic zakonczyl swoje dzialanie.\n");

    return 0;
}

void handle_gate_control(int id_sem, int id_team, Shared_data_fan * data, int fan_type) {
    int id_gate = -1;

    while (id_gate == -1) {
        
        handle_semaphore_p(id_sem, 1);
        for (int i = 0; i < 3; i++)  {

            if (data->gateTeam[i] == -1) {
                id_gate = i;

                if (fan_type == NORMAL_WITH_CHILD) {
                    data->gateCount[i] = data->gateCount[i] + 2;
                } else {
                    data->gateCount[i] = data->gateCount[i] + 1;
                }

                data->gateTeam[i] = id_team;
                break;
            } else if (data->gateTeam[i] == id_team && data->gateCount[i] < 3 && fan_type != NORMAL_WITH_CHILD) {
                id_gate = i;
                data->gateCount[i] = data->gateCount[i] + 1;
                break;
            } else if(data->gateTeam[i] == id_team && data->gateCount[i] < 2 && fan_type == NORMAL_WITH_CHILD) {
                id_gate = i;
                data->gateCount[i] = data->gateCount[i] + 2;
                break;
            }
        }
        handle_semaphore_v(id_sem, 1);

    }

    printf("Kibic %d wybral bramke | Bramka: %d | Druzyna %d | Liczba osob na bramce: %d | Bramka zajeta przez: %d\n", getpid(), id_gate, id_team, data->gateCount[id_gate], data->gateTeam[id_gate]);

    if (fan_type == NORMAL_WITH_CHILD) {
        handle_semaphore_pn(id_sem, id_gate + 2, 2); // uwzglednienie dziecka przy wejsciu
    } else {
        handle_semaphore_p(id_sem, id_gate + 2);
    }

    printf("Kibic (PID=%d) przechodzi kontrole bezpieczenstwa na bramce %d\n", getpid(), id_gate);    
    sleep(rand() % 3 + 1);

    if (fan_type == NORMAL_WITH_CHILD) {
        handle_semaphore_vn(id_sem, id_gate + 2, 2); // uwzglednienie dziekca przy wyjsciu
    } else {
        handle_semaphore_v(id_sem, id_gate + 2);
    }

    handle_semaphore_p(id_sem, 1); // pisanie

    if (data->gateCount[id_gate] == 1 && fan_type != NORMAL_WITH_CHILD) {
        data->gateTeam[id_gate] = -1;
        data->gateCount[id_gate] = data->gateCount[id_gate] - 1;
    } else if (data->gateCount[id_gate] == 2 && fan_type == NORMAL_WITH_CHILD) {
        data->gateTeam[id_gate] = -1;
        data->gateCount[id_gate] = data->gateCount[id_gate] - 2;
    } else if (fan_type == NORMAL_WITH_CHILD) {
        data->gateCount[id_gate] = data->gateCount[id_gate] - 2;
    } else {
        data->gateCount[id_gate] = data->gateCount[id_gate] - 1;
    }

    handle_semaphore_v(id_sem, 1);

    printf("Kibic %d wchodzi na stadion | Bramka: %d | Druzyna %d | Liczba osob na bramce: %d | wartosc sem1: %d \n", getpid(), id_gate, id_team, data->gateCount[id_gate], get_semaphore_value(id_sem, 1));
}

// 5 => r | 6 => w
void handle_normal_fan(int id_sem, int id_team, Shared_data_fan * data, int K, int fan_type) {
    printf("Kibic czeka dla normalnych | PID: %d | Druzyna: %d\n", getpid(), id_team);
    handle_semaphore_pn(id_sem, 5, K);
    handle_semaphore_pn(id_sem, 6, 1);

    handle_semaphore_vn(id_sem, 5, K-1);
    handle_semaphore_vn(id_sem, 5, 1);

    printf("Kibic wchodzi dla normalnych | PID: %d | Druzyna: %d\n", getpid(), id_team);
    handle_gate_control(id_sem, id_team, data, fan_type);

    handle_semaphore_vn(id_sem, 6, 1);
}

void handle_priority_fan(int id_sem, int id_team, Shared_data_fan * data, int K, int fan_type) {
    printf("Kibic czeka dla vip | PID: %d | Druzyna: %d\n", getpid(), id_team);
    handle_semaphore_pn(id_sem, 5, 1);
    handle_semaphore_pn(id_sem, 6, K);

    printf("Kibic wchodzi dla vip | PID: %d | Druzyna: %d\n", getpid(), id_team);
    handle_gate_control(id_sem, id_team, data, fan_type);
    
    handle_semaphore_vn(id_sem, 6, K);
    handle_semaphore_vn(id_sem, 5, 1);
}