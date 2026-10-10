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

void send_message(const char *message, int client_socket_fd) {
    const ssize_t n = send(client_socket_fd, message, MAX_MESSAGE_LENGTH, 0);
    if (n == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }
}

void receive_message(char* read_buffer, const int client_socket_fd) {
    const ssize_t n = recv(client_socket_fd, read_buffer, sizeof(read_buffer) - 1, 0);
    if (n > 0) {
        printf("Server Response: %s\n", read_buffer);
        if (strcmp(read_buffer, "exit") == 0) {
            exit(EXIT_SUCCESS);
        }
    }
    else if (n == 0) {
        printf("Server disconnected\n");
        exit(EXIT_SUCCESS);
    }
    else {
        perror("recv");
        exit(EXIT_FAILURE);
    }
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

    char type[4];
    for (int i = 0; i < 4; i++) {
        if (message[i] == ' ') { message = message + i + 1; break; }
        type[i] = message[i];
    }

    return (MSG_PROTOCOL) { .type = get_type(type), .message = message };
}