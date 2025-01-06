#include <stdio.h>
#include "pracownik_techniczny.h"
#include "semafory.h"
#include "pamiec_dzielona.h"

int main() {
    int id_sem = create_new_semaphore();
    int id_shm = create_new_shared_memory();
    Shared_data * shared_data = join_shared_memory(id_shm);
    int K;

    printf("Podaj liczbe kibicow majacych wejsc na stadion: ");
    scanf("%d", &K);

    if (K <= 0) {
        printf("Podana liczba kibicow jest nieprawidlowa!\n");
        return 1;
    }


    set_semaphore(id_sem, 0, K); // liczba kibicow
    set_semaphore(id_sem, 1, 1); // pamiec
    set_semaphore(id_sem, 2, 3);
    set_semaphore(id_sem, 3, 3);
    set_semaphore(id_sem, 4, 3);

    handle_semaphore_p(id_sem, 1);
    
    shared_data->K = K;
    
    for (int i = 0; i < 3; i++) {
        shared_data->gateTeam[i] = -1;
        shared_data->gateCount[i] = 0;
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

    detach_shared_memory(shared_data);
    delete_shared_memory(id_shm);
    delete_semaphore(id_sem);
    return 0;
}
