#include <stdio.h>
#include <stdlib.h>
#include <errno.h>    
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "pamiec_dzielona.h"

int create_new_shared_memory(int key, int size) {
    int id_shared = shmget(key, size, 0666 | IPC_CREAT);

    if (id_shared == -1) {
        perror("Blad utworzenia pamieci wspoldzielonej");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec wspoldzielona zostala utworzona: %d\n", id_shared);
    }

    return id_shared;
}

int get_shared_memory(int key, int size) {
    int id_shared = shmget(key, size, 0666);

    if (id_shared == -1) {
        perror("Blad dostepu do pamieci wspoldzielonej");
        exit(EXIT_FAILURE);
    } else {
        printf("Dostep do pamieci wspoldzielonej uzyskany: %d\n", id_shared);
    }

    return id_shared;
}

Shared_data_fan *join_shared_memory_fan(int id_shared) {
    Shared_data_fan *address = (Shared_data_fan*)shmat(id_shared, NULL, 0);

    if (address == (void*)-1) {
        perror("Blad dolaczenia pamieci wspoldzielonej (fan)");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec wspoldzielona zostala dolaczona (fan)\n");
    }

    return address;
}

Shared_data_manager *join_shared_memory_manager(int id_shared) {
    Shared_data_manager *address = (Shared_data_manager*)shmat(id_shared, NULL, 0);

    if (address == (void*)-1) {
        perror("Blad dolaczenia pamieci wspoldzielonej (manager)");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec wspoldzielona zostala dolaczona (manager)\n");
    }

    return address;
}

void detach_shared_memory_fan(Shared_data_fan *address) {
    if (shmdt(address) == -1) {
        perror("Problemy z odlaczeniem pamieci dzielonej (fan)");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec dzielona zostala odlaczona (fan).\n");
    }
}

void detach_shared_memory_manager(Shared_data_manager *address) {
    if (shmdt(address) == -1) {
        perror("Problemy z odlaczeniem pamieci dzielonej (manager)");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec dzielona zostala odlaczona (manager).\n");
    }
}

void delete_shared_memory(int id_shared) {
    if (shmctl(id_shared, IPC_RMID, NULL) == -1) {
        perror("Blad usuniecia pamieci wspoldzielonej");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec wspoldzielona zostala usunieta: %d\n", id_shared);
    }
}
