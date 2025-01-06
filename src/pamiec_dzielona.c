#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "pamiec_dzielona.h"

int create_new_shared_memory() {
    int id_shared = shmget(48, sizeof(Shared_data), 0666|IPC_CREAT);

    if (id_shared == -1) {
        printf("Blad utworzenia pamieci wspoldzielonej.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec wspoldzielona zostala utworzona : %d\n", id_shared);
    }

    return id_shared;
}

int get_shared_memory() {
    int id_shared = shmget(48, sizeof(Shared_data), 0666);

    if (id_shared == -1) {
        printf("Blad dostepu do pamieci wspoldzielonej.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Dostep do pamieci wspoldzielonej uzyskany: %d\n", id_shared);
    }

    return id_shared;
}

Shared_data* join_shared_memory(int id_shared) {
    Shared_data* address = (Shared_data*)shmat(id_shared, NULL, 0);

    if (address == -1) {
        perror("Blad dolaczenia pamieci wspoldzielonej.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec wspoldzielona zostala dolaczona\n");
    }
    
    return address;
}

void detach_shared_memory(Shared_data * address) {
    int return_value;

    return_value = shmdt(address);

    if (return_value == -1) {
        printf("Problemy z odlaczeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec dzielona zostala odlaczona.\n");
    }
}

void delete_shared_memory(int id_shared) {
    int return_value;

    return_value = shmctl(id_shared, IPC_RMID, NULL);

    if (return_value == -1) {
        printf("Blad usuniecia pamieci wspoldzielonej.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec wspoldzielona zostala usunieta: %d\n", id_shared);
    }
}