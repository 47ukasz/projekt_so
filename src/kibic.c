#include "kibic.h"
#include "semafory.h"
#include "pamiec_dzielona.h"   
#include "watki.h"

int main() {
    int id_sem = get_access_semaphore();
    int id_shared = get_shared_memory(48, sizeof(Shared_data_fan));
    Shared_data_fan * shared_data = join_shared_memory_fan(id_shared);
    pthread_t thread_id_wait;
    int fan_number;
    int generation_status;
    int i = 0;
    
    double fan_vip_percentage = 0.005;
    double fan_child_percentage = 0.15;
    pid_t pid;

    handle_semaphore_p(id_sem, 1);
    fan_number = shared_data->K;
    generation_status = !shared_data->match_ended;
    handle_semaphore_v(id_sem, 1);

    int probability_vip = round(fan_number / round(fan_vip_percentage * fan_number)); // prawdopodobienstwo stworzenia procesu VIP (K >= 200)
    int probability_child = round(fan_number / round(fan_child_percentage * fan_number)); // prawdopodobienstwo stworzenia procesu z dzieckiem (K >= 200)

    while(generation_status) {
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
            Data_time_thread thread_time_data;
            Fan fan_data;
            fan_data.fan_pid = getpid();
            fan_data.fan_team = rand() % 2; 
            fan_data.is_dangerous = (i % 100 == 0) ? 1 : 0;
            // fan_data.is_dangerous = 1;

            // fan_data.fan_type = VIP_FAN;

            if (i % probability_vip == 0) {
                fan_data.fan_type = VIP_FAN;
            } else if (i % probability_child == 0) {
                fan_data.fan_type = NORMAL_WITH_CHILD;
            } else {
                fan_data.fan_type = NORMAL_FAN;
            }

            if (i % 2 == 0) {
                fan_data.fan_type = NORMAL_FAN_PRIORITY;
            }

            printf("Utworzono nowego Kibica | PID: %d | Druzyna: %d | Typ: %d\n", fan_data.fan_pid, fan_data.fan_team, fan_data.fan_type);

            if (fan_data.fan_type == NORMAL_WITH_CHILD) {
                create_new_thread(&thread_id_child, child_handler, NULL);
            }

            thread_time_data.running = 1;

            create_new_thread(&thread_id_time, calculate_time, (void *) &thread_time_data);

            handle_semaphore_p(id_sem, 1);
            shared_data->waiting_before_queue = shared_data->waiting_before_queue + 1;
            handle_semaphore_v(id_sem, 1);

            handle_semaphore_p(id_sem, 0); // ustawienie w kolejce
            
            handle_semaphore_p(id_sem, 1);

            if (shared_data->match_ended) {
                printf("Kibic (PID=%d) opuszczar obszar przed stadionem\n", fan_data.fan_pid);
                handle_semaphore_v(id_sem,1);
                exit(EXIT_SUCCESS);
                break;
            } else {
                shared_data->waiting_before_queue = shared_data->waiting_before_queue - 1;
            }

            handle_semaphore_v(id_sem, 1);

            handle_semaphore_p(id_sem, 9);

            if (fan_data.fan_type == NORMAL_FAN || fan_data.fan_type == NORMAL_WITH_CHILD) {
                handle_normal_fan(id_sem, shared_data, &fan_data, fan_number);
            } else if (fan_data.fan_type == NORMAL_FAN_PRIORITY) {
                handle_priority_fan(id_sem, shared_data, &fan_data, fan_number);
            }

            if (fan_data.is_dangerous) {
                printf("Kibic (PID=%d) jest niebezpieczny i nie moze wejsc na stadion\n", fan_data.fan_pid);
                thread_time_data.running = 0; // oznaczenia konca dzialania procesu
                join_thread(thread_id_time);
            
                if (fan_data.fan_type == NORMAL_WITH_CHILD) {
                    join_thread(thread_id_child);
                }

                detach_thread(thread_id_time);

                if (fan_data.fan_type == NORMAL_WITH_CHILD) {
                    detach_thread(thread_id_child);
                }
            
                exit(EXIT_SUCCESS);
                break;
            }

            if (fan_data.fan_type == VIP_FAN) {
                handle_semaphore_p(id_sem, 1);
                shared_data->entered_VIP = shared_data->entered_VIP + 1;
                handle_semaphore_v(id_sem, 1);                

                handle_semaphore_p(id_sem, 10);
            } else {
                handle_semaphore_p(id_sem, 1);
                shared_data->entered_normal = shared_data->entered_normal + 1;
                handle_semaphore_v(id_sem, 1);

                handle_semaphore_p(id_sem, 8);
            }

            printf("Kibic (PID=%d) wychodzi ze stadionu\n", fan_data.fan_pid);

            thread_time_data.running = 0; // oznaczenia konca dzialania procesu
            join_thread(thread_id_time);
            
            if (fan_data.fan_type == NORMAL_WITH_CHILD) {
                join_thread(thread_id_child);
            }

            //printf("Czas dzialania programu (PID=%d): %ld\n", getpid(), data.working_time);            

            detach_thread(thread_id_time);

            if (fan_data.fan_type == NORMAL_WITH_CHILD) {
                detach_thread(thread_id_child);
            }
            
            exit(EXIT_SUCCESS);
            break;
        }
        default:
            break;
        }
        
        i = i + 1;

        if (i > fan_number) {
            sleep(1);

            handle_semaphore_p(id_sem, 1);
            generation_status = !shared_data->match_ended;
            handle_semaphore_v(id_sem, 1);
        }

        if (i == 1) {
            create_new_thread(&thread_id_wait, handle_wait, NULL);
        }

    }

    join_thread(thread_id_wait);
    detach_thread(thread_id_wait);
    detach_shared_memory_fan(shared_data);
    printf("Kibic zakonczyl swoje dzialanie.\n");

    return 0;
}

void handle_gate_control(int id_sem, Shared_data_fan * data, Fan * fan_data) {
    int id_gate = -1;

    while (id_gate == -1) {
        
        handle_semaphore_p(id_sem, 1);
        for (int i = 0; i < 3; i++)  {

            if (data->gateTeam[i] == -1) {
                id_gate = i;

                if (fan_data->fan_type == NORMAL_WITH_CHILD) {
                    data->gateCount[i] = data->gateCount[i] + 2;
                } else {
                    data->gateCount[i] = data->gateCount[i] + 1;
                }

                data->gateTeam[i] = fan_data->fan_team;
                break;
            } else if (data->gateTeam[i] == fan_data->fan_team && data->gateCount[i] < 3 && fan_data->fan_type != NORMAL_WITH_CHILD) {
                id_gate = i;
                data->gateCount[i] = data->gateCount[i] + 1;
                break;
            } else if(data->gateTeam[i] == fan_data->fan_team && data->gateCount[i] < 2 && fan_data->fan_type == NORMAL_WITH_CHILD) {
                id_gate = i;
                data->gateCount[i] = data->gateCount[i] + 2;
                break;
            }
        }
        handle_semaphore_v(id_sem, 1);

    }

    printf("Kibic (PID=%d) wybral bramke | Bramka: %d | Druzyna %d | Liczba osob na bramce: %d | Bramka zajeta przez: %d\n", fan_data->fan_pid, id_gate, fan_data->fan_team, data->gateCount[id_gate], data->gateTeam[id_gate]);

    if (fan_data->fan_type == NORMAL_WITH_CHILD) {
        handle_semaphore_pn(id_sem, id_gate + 2, 2); // uwzglednienie dziecka przy wejsciu
    } else {
        handle_semaphore_p(id_sem, id_gate + 2);
    }

    printf("Kibic (PID=%d) przechodzi kontrole bezpieczenstwa na bramce %d\n", fan_data->fan_pid, id_gate);    
    usleep(rand() % 500);

    if (fan_data->is_dangerous) {
        printf("Kibic (PID=%d) nie przeszedl kontroli bezpieczenstwa\n", fan_data->fan_pid);
    } else {
        printf("Kibic (PID=%d) przeszedl kontrole bezpieczenstwa\n", fan_data->fan_pid);
    }

    if (fan_data->fan_type == NORMAL_WITH_CHILD) {
        handle_semaphore_vn(id_sem, id_gate + 2, 2); // uwzglednienie dziekca przy wyjsciu
    } else {
        handle_semaphore_v(id_sem, id_gate + 2);
    }

    handle_semaphore_p(id_sem, 1); // pisanie

    if (data->gateCount[id_gate] == 1 && fan_data->fan_type != NORMAL_WITH_CHILD) {
        data->gateTeam[id_gate] = -1;
        data->gateCount[id_gate] = data->gateCount[id_gate] - 1;
    } else if (data->gateCount[id_gate] == 2 && fan_data->fan_type == NORMAL_WITH_CHILD) {
        data->gateTeam[id_gate] = -1;
        data->gateCount[id_gate] = data->gateCount[id_gate] - 2;
    } else if (fan_data->fan_type == NORMAL_WITH_CHILD) {
        data->gateCount[id_gate] = data->gateCount[id_gate] - 2;
    } else {
        data->gateCount[id_gate] = data->gateCount[id_gate] - 1;
    }

    handle_semaphore_v(id_sem, 1);

    if (!fan_data->is_dangerous) {
        printf("Kibic (PID=%d) wchodzi na stadion | Bramka: %d | Druzyna %d | Liczba osob na bramce: %d\n", getpid(), id_gate, fan_data->fan_team, data->gateCount[id_gate]);
    }
}

// 5 => r | 6 => w
void handle_normal_fan(int id_sem, Shared_data_fan * data, Fan * fan_data, int K) {
    printf("Kibic czeka dla normalnych | PID: %d | Druzyna: %d\n", fan_data->fan_pid, fan_data->fan_team);
    handle_semaphore_pn(id_sem, 5, K);
    handle_semaphore_pn(id_sem, 6, 1);

    handle_semaphore_vn(id_sem, 5, K-1);
    handle_semaphore_vn(id_sem, 5, 1);

    printf("Kibic wchodzi dla normalnych | PID: %d | Druzyna: %d\n", fan_data->fan_pid, fan_data->fan_team);
    handle_gate_control(id_sem, data, fan_data);

    handle_semaphore_vn(id_sem, 6, 1);
}

void handle_priority_fan(int id_sem, Shared_data_fan * data, Fan * fan_data, int K) {
    printf("Kibic czeka dla vip | PID: %d | Druzyna: %d\n", fan_data->fan_pid, fan_data->fan_team);
    handle_semaphore_pn(id_sem, 5, 1);
    handle_semaphore_pn(id_sem, 6, K);

    printf("Kibic wchodzi dla vip | PID: %d | Druzyna: %d\n", fan_data->fan_pid, fan_data->fan_team);
    handle_gate_control(id_sem, data, fan_data);

    handle_semaphore_vn(id_sem, 6, K);
    handle_semaphore_vn(id_sem, 5, 1);
}