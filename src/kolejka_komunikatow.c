#include "kolejka_komunikatow.h"

int create_new_message_queue() {
    int id_queue = msgget(50, IPC_CREAT | 0666);   

    if (id_queue == -1) {
        printf("Blad utworzenia nowej kolejki.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Kolejka zostala utworzona : %d\n", id_queue);
    }

    return id_queue;
}

int get_access_message_queue() {
    int id_queue = msgget(50, 0666);

    if (id_queue == -1) {
        printf("Nie mozna uzyskac dostepu do kolejki.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Kolejka zostala dolaczona : %d\n", id_queue);
    }

    return id_queue;
}

void send_message(int id_queue, Message * data_message) {
    int return_value;

    return_value = msgsnd(id_queue, data_message, strlen(data_message->mtext) + 1, 0);

    if (return_value == -1) {
        printf("Blad dodania komunikatu do kolejki\n");
        exit(EXIT_FAILURE);
    } 
}

void receive_message(int id_queue, Message * data_message, long int type_message) {
    int return_value;

    return_value = msgrcv(id_queue, data_message, MAX, type_message, 0);

    if (return_value == -1) {
        printf("Blad pobrania komunikatu z kolejki\n");
        exit(EXIT_FAILURE);
    }
}

void delete_message_queue(int id_queue) {
    int return_value;

    return_value = msgctl(id_queue, IPC_RMID, NULL);

    if (return_value == -1) {
        printf("Blad usuniecia kolejki.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Kolejka zostala usunieta: %d\n", id_queue);
    }
}