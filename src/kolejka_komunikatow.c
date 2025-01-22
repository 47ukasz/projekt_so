#include "kolejka_komunikatow.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>      
#include <string.h>     
#include <sys/msg.h>   

int create_new_message_queue() {
    int id_queue = msgget(50, IPC_CREAT | 0666);   

    if (id_queue == -1) {
        perror("Blad utworzenia nowej kolejki");
        exit(EXIT_FAILURE);
    } else {
        printf("Kolejka zostala utworzona: %d\n", id_queue);
    }

    return id_queue;
}

int get_access_message_queue() {
    int id_queue = msgget(50, 0666);

    if (id_queue == -1) {
        perror("Nie mozna uzyskac dostepu do kolejki");
        exit(EXIT_FAILURE);
    } else {
        printf("Kolejka zostala dolaczona: %d\n", id_queue);
    }

    return id_queue;
}

void send_message(int id_queue, Message *data_message) {
    int return_value;

    size_t msg_size = strlen(data_message->mtext) + 1; 

    return_value = msgsnd(id_queue, data_message, msg_size, 0);

    if (return_value == -1) {
        perror("Blad dodania komunikatu do kolejki");
        exit(EXIT_FAILURE);
    }
}

void receive_message(int id_queue, Message *data_message, long int type_message) {
    int return_value;

    return_value = msgrcv(id_queue, data_message, MAX, type_message, 0);

    if (return_value == -1) {
        perror("Blad pobrania komunikatu z kolejki");
        exit(EXIT_FAILURE);
    }
}

void delete_message_queue(int id_queue) {
    int return_value;

    return_value = msgctl(id_queue, IPC_RMID, NULL);

    if (return_value == -1) {
        perror("Blad usuniecia kolejki");
        exit(EXIT_FAILURE);
    } else {
        printf("Kolejka zostala usunieta: %d\n", id_queue);
    }
}
