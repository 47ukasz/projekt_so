#ifndef KOLEJKA_KOMUNIKATOW_H
#define KOLEJKA_KOMUNIKATOW_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>

#define MAX 255
#define MANAGER 1

typedef struct _Message {
    long mtype;
    char mtext[MAX];
} Message;

int create_new_message_queue();
int get_access_message_queue();
void send_message(int id_queue, Message * data_message);
void receive_message(int id_queue, Message * data_message, long int type_message);
void delete_message_queue(int id_queue);

#endif