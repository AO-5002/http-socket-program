//
// Created by Andres Ortiz Osorio on 10/9/26.
//

#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "../transport_info.h"

void send_message(const char *message, const int origin_fd, const int dest_fd) {
    if (origin_fd == dest_fd) return;
    const ssize_t n = send(dest_fd, message, strlen(message), 0);
    if (n == -1) {
        perror("send_message");
        exit(EXIT_FAILURE);
    }
}

int return_user_fd(const char *target, char *connected_clients[]) {
    if (target == NULL) return -1;
    for (int i = 0; i < FD_SETSIZE; i++) {
        if (connected_clients[i] != NULL && strcmp(connected_clients[i], target) == 0) return i;
    }

    return -1;
}


void broadcast_message(const char *message, const int origin_fd, char *connected_clients[]) {

    // Client broadcast to all users.
    for (int fd = 0; fd < FD_SETSIZE; fd++) {
        if (connected_clients[fd] != NULL && fd != origin_fd) {
            const ssize_t n = send(fd, message, strlen(message), 0);
            if (n == -1) {
                perror("send");
                exit(EXIT_FAILURE);
            }
        }
    }
}

ssize_t receive_message(char* read_buffer, const int fd) {
    const ssize_t n = recv(fd, read_buffer, MAX_MESSAGE_LENGTH - 1, 0);
    if (n < 0) {
        perror("receive_message");
        exit(EXIT_FAILURE);
    }

    if (n == 0) {
        printf("Server disconnected.\n");
        exit(EXIT_SUCCESS);
    }

    read_buffer[n] = '\0';
    return n;
}

MSG_TYPE get_type(const char *message) {
    if (strcmp(message, "REG") == 0) return REG;
    if (strcmp(message, "ACK") == 0) return ACK;
    if (strcmp(message, "MESG") == 0) return MESG;
    if (strcmp(message, "PMSG") == 0) return PMSG;
    if (strcmp(message, "ERR") == 0) return ERR;
    if (strcmp(message, "EXIT") == 0) return EXIT;
    return UNKOWN;
}

MSG_PROTOCOL parse_message(char *message) {

    // Parse the type and content from the message.
    char type[5] = {0};
    int i = 0;
    for (; i < 4; i++) {
        if (message[i] == ' ' || message[i] == '\0') break;
        type[i] = message[i];
    }

    message += message[i] == ' ' ? i + 1 : i;
    return (MSG_PROTOCOL) { .type = get_type(type), .message = message };
}