#include "pracownik_techniczny.h"
#include "semafory.h"
#include "pamiec_dzielona.h"
#include "kolejka_komunikatow.h"

int id_sem;
int id_queue;
int K;

int main() {
    id_sem = create_new_semaphore();
    int id_shm_fan = create_new_shared_memory(48, sizeof(Shared_data_fan));
    int id_shm_manager = create_new_shared_memory(49, sizeof(Shared_data_manager));
    id_queue = create_new_message_queue();
    Shared_data_fan * shared_data_fan = join_shared_memory_fan(id_shm_fan);
    Shared_data_manager * shared_data_manager = join_shared_memory_manager(id_shm_manager);

    signal(SIGUSR1, handle_pause_signal);
    signal(SIGUSR2, handle_resume_signal);
    signal(SIGINT, handle_exit_signal);

    printf("Podaj liczbe kibicow majacych wejsc na stadion: ");
    scanf("%d", &K);

    if (K <= 0) {
        printf("Podana liczba kibicow jest nieprawidlowa!\n");
        return 1;
    }

    set_semaphore(id_sem, 0, K); // liczba kibicow, kolejka
    set_semaphore(id_sem, 1, 1); // pamiec
    set_semaphore(id_sem, 2, 3);
    set_semaphore(id_sem, 3, 3);
    set_semaphore(id_sem, 4, 3);
    set_semaphore(id_sem, 5, K); // sekcja krytyczna dla procesow czytajacych (kibicow bez prio) | r
    set_semaphore(id_sem, 6, K); // sekcja krytyczna dla procesow piszacych (kibicow z prio) | w
    set_semaphore(id_sem, 7, 1); // pamiec dla kierownika
    set_semaphore(id_sem, 8, 0); // wejscie na stadion, normalni | 0 sa na stadionie | K wychodza ze stadionu
    set_semaphore(id_sem, 9, K); // wstrzymywanie/wznawianie kontroli
    set_semaphore(id_sem, 10, 0); // wejscie na stadion, VIP | 0 sa na stadionie | K * 0.005 wychodza ze stadionu

    handle_semaphore_p(id_sem, 7);

    shared_data_manager->employee_pid = getpid();

    handle_semaphore_v(id_sem, 7);

    handle_semaphore_p(id_sem, 1);

    shared_data_fan->K = K;

    for (int i = 0; i < 3; i++) {
        shared_data_fan->gateTeam[i] = -1;
        shared_data_fan->gateCount[i] = 0;
    }

    handle_semaphore_v(id_sem, 1);

    pid_t pid = fork();

    switch(pid) {
        case -1:
            printf("Blad utworzenia nowego procesu!\n");
            perror("fork");
            exit(EXIT_FAILURE);
            break;
        case 0:
            execl("./bin/kibic", "kibic", NULL);
            break;
    }

    wait(NULL);

    detach_shared_memory_fan(shared_data_fan);
    delete_shared_memory(id_shm_fan);
    delete_shared_memory(id_shm_manager);
    delete_semaphore(id_sem);
    return 0;
}

void handle_pause_signal(int signal) {
    printf("Otrzymano sygnal: %d, Wstrzymywanie wpuszczania do kontroli.\n", signal);
    set_semaphore(id_sem, 9, 0);
}

void handle_resume_signal(int signal) {
    printf("Otrzymano sygnal: %d, Wznawianie wpuszczania do kontroli.\n", signal);
    set_semaphore(id_sem, 9, K);
}

void handle_exit_signal(int signal) {
    Message data_message;

    data_message.mtype = MANAGER;
    strcpy(data_message.mtext, "Wszyscy kibice opuscili stadion.");

    printf("Otrzymano sygnal: %d, Rozpoczeto opuszczanie stadionu przez kibicow.\n", signal);
    set_semaphore(id_sem, 8, K);
    set_semaphore(id_sem, 10, K);
    send_message(id_queue, &data_message);
}