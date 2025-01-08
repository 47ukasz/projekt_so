#include "kierownik.h"
#include "kolejka_komunikatow.h"

int main() {
    int id_sem = get_access_semaphore();
    int id_queue = get_access_message_queue();
    int id_shared_manager = get_shared_memory(49, sizeof(Shared_data_manager));
    Shared_data_manager * shared_data_manager = join_shared_memory_manager(id_shared_manager);
    
    pid_t employee_pid = 0;
    int option = 0;
    Message data_message;

    handle_semaphore_p(id_sem, 7);

    employee_pid = shared_data_manager->employee_pid;

    handle_semaphore_v(id_sem, 7);

    while (option != 4) {
        printf("1 - zatrzymaj wpuszczanie na stadion\n");
        printf("2 - wznow wpuszczanie na stadion\n");
        printf("3 - rozkaz opuszczenia stadionu wszystkim kibicom\n");
        printf("4 - KONIEC\n");

        scanf("%d", &option);

        switch (option) {
            case 1: 
                kill(employee_pid, SIGUSR1);
                break;
            case 2: 
                kill(employee_pid, SIGUSR2);
                break;
            case 3:
                kill(employee_pid, SIGINT);
                receive_message(id_queue, &data_message, MANAGER);
                printf("%s\n", data_message.mtext); 
                break;
            case 4:
                break;
            default:
                printf("Niepoprawna opcja!\n");
                break;
        }
    }

    detach_shared_memory_manager(shared_data_manager);

    return 0;
}