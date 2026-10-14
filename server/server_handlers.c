//
// Created by Andres Ortiz Osorio on 10/14/26.
//

#include "server_handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../transport_info.h"
#include "../common/protocol.h"
#include "../server/server_validation.h"
#include "../server/server_setup.h"

int is_registered(const int fd, CLIENT connected_clients[]) {
    if (connected_clients[fd].username[0] == '\0') return 0;
    return 1;
}

void route_message(char *read_buffer, const int fd, const int server_socket_fd, CLIENT connected_clients[], const int client_count) {

    // Parse the message of its msg-type and contents.

    const MSG_PROTOCOL msg = parse_message(read_buffer);

    // Handle user-authorization

    if (msg.type == EXIT) { handle_exit(fd, connected_clients); }
    if (msg.type == REG && is_registered(fd, connected_clients) == 0) { handle_register(msg, fd, server_socket_fd, connected_clients, client_count); }
    else if (msg.type != REG && is_registered(fd, connected_clients) == 0) { handle_unauthorized_message(fd, server_socket_fd); }
    else {

        // If the user is authorized, then we process their message.

        switch (msg.type) {
            case MESG: { handle_broadcast_message(msg.message, fd, connected_clients); break; }
            case PMSG: { handle_private_message(msg.message, fd, connected_clients); break;}
            default: { handle_default_message(fd, server_socket_fd, connected_clients); break; }
        }
    }
}

void handle_unauthorized_message(const int origin_fd, const int server_socket_fd) {

    // Server-Log
    printf("Connection %d - Attempted to send a message without being registered.\n", origin_fd);

    // Message to origin_fd
    const char *message = "Please register by writing 'REG <username>'.\n";
    send_message(message, server_socket_fd, origin_fd);
}

void handle_default_message(const int origin_fd, const int server_socket_fd, CLIENT connected_clients[]) {

    // Server log
    printf("(%s) - Attempted to send an Invalid message type.\n", connected_clients[origin_fd].username);

    // Private message to origin_fd
    char message[MAX_MESSAGE_LENGTH];
    snprintf(message, MAX_MESSAGE_LENGTH, "%s Messages require {REG, MESG, PMSG, EXIT} prefix.\n", connected_clients[origin_fd].username);
    handle_private_message(message, server_socket_fd, connected_clients);
}

void handle_broadcast_message(char *message, const int origin_fd, CLIENT connected_clients[]) {
    // Server log
    printf("(%s -> ALL) : %s\n", connected_clients[origin_fd].username, message);
    broadcast_message(message, origin_fd, connected_clients);
}

void handle_exit(const int fd, CLIENT connected_clients[]) {
    printf("User '%s' disconnected.\n", connected_clients[fd].username);
    connected_clients[fd] = (CLIENT){0};
    close(fd);
}

void handle_register(const MSG_PROTOCOL msg, const int fd, const int server_socket_fd, CLIENT connected_clients[], int client_count) {
    const int status = register_new_user(msg.message, connected_clients);
    if (status == 0) {

        // Add the new user to the client list
        snprintf(connected_clients[fd].username, sizeof connected_clients[fd].username, "%s", msg.message);
        ++client_count;

        // Start building an ACK message for the client

        char message[MAX_MESSAGE_LENGTH];
        snprintf(message, MAX_MESSAGE_LENGTH, "ACK %d ", client_count);

        // List all the connected users

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (connected_clients[i].username[0] != '\0') {
                snprintf(message + strlen(message), sizeof message - strlen(message) , "%s ", connected_clients[i].username);
            }
        }

        send_message(message, server_socket_fd, fd);

        // Server logging
        printf("User '%s' registered.\n", connected_clients[fd].username);
    }
    else if (status == INVALID_USERNAME) {
        send_message("ERR INVALID_USERNAME", server_socket_fd, fd);
    }
    else if (status == USERNAME_TAKEN) {
        send_message("ERR USERNAME_TAKEN", server_socket_fd, fd);
    }
}

void handle_private_message(char *message, const int origin_fd, CLIENT connected_clients[]) {

    // Parse for the destination user (assumes the message is formatted like this ___ ___)

    int i = 0;
    char destination_user[MAX_USERNAME_LENGTH];
    while (message[i] != ' ') {
        destination_user[i] = message[i];
        i++;
    }
    destination_user[i] = '\0';
    message = message + i + 1;

    // Search for the target-user and return its fd.
    const int dest_fd = return_user_fd(destination_user, connected_clients);

    // Send the message to target-user
    send_message(message, origin_fd, dest_fd);

    // Server Logging
    printf("(%s -> %s) : %s\n", connected_clients[origin_fd].username, destination_user, message);
}