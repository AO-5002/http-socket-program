//
// Created by Andres Ortiz Osorio on 10/9/26.
//

#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "../transport_info.h"

// Generic function for sending messages
void send_message(const char *message, const int origin_fd, const int dest_fd) {
    if (origin_fd == dest_fd || dest_fd < 0) return;

    // One failed send shouldn't take the whole program down; just log it.
    if (send(dest_fd, message, strlen(message), 0) == -1 || send(dest_fd, "\n", 1, 0) == -1) {
        perror("send_message");
    }
}

// Return fd based on username
int return_user_fd(const char *target, CLIENT connected_clients[]) {
    if (target == NULL) return -1;
    for (int i = 0; i < FD_SETSIZE; i++) {
        if (connected_clients[i].username[0] != '\0' && strcmp(connected_clients[i].username, target) == 0) return i;
    }

    return -1;
}

// Generic function for sending global-messages
void broadcast_message(const char *message, const int origin_fd, CLIENT connected_clients[]) {

    // Send to every registered user except the sender (send_message adds the '\n').
    for (int fd = 0; fd < FD_SETSIZE; fd++) {
        if (connected_clients[fd].username[0] != '\0') send_message(message, origin_fd, fd);
    }
}

// Generic function for receiving messages
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

// Parse message and identify message-type & message contents
MSG_PROTOCOL parse_message(char *message) {

    // Parse the type and content from the message.
    char type[5] = {0};
    int i = 0;
    for (; i < 4; i++) {
        if (message[i] == ' ' || message[i] == '\0') break;
        type[i] = message[i];
    }

    // Type must be followed by a space or \0.
    if (message[i] != ' ' && message[i] != '\0') return (MSG_PROTOCOL) { .type = UNKOWN, .message = message + i };

    message += message[i] == ' ' ? i + 1 : i;
    return (MSG_PROTOCOL) { .type = get_type(type), .message = message };
}