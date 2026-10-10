//
// Created by Andres Ortiz Osorio on 10/6/26.
//

/*
* Client can send private messages to other users
* Client can broadcast messages to all users
* Simple protocol to register users, and exchange messages
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include "../transport_info.h"
#include <unistd.h>
#include "client_setup.h"

void send_message(const char *message, int client_socket_fd);
void receive_message(char *read_buffer, int client_socket_fd);
void register_user(char *username, char *port, int client_socket_fd);
void prompt_user(char *username, char *port);

int main() {

    char username[MAX_USERNAME_LENGTH];
    char port[MAX_PORT_LENGTH];

    // (Destination) Server Address
    struct sockaddr_in dest_addr = {0};
    dest_addr.sin_family = DOMAIN_PROTOCOL;
    dest_addr.sin_port = htons(SERVER_PORT);
    dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Prompt User
    // prompt_user(username, port);

    // Establish TCP-Client

    int client_socket_fd = create_client_socket();
    connect_to_server(client_socket_fd, dest_addr, sizeof(dest_addr));

    // Register User
    // register_user(username, port, client_socket_fd);

    // Chatroom
    while (1) {
        char read_buffer[MAX_MESSAGE_LENGTH];
        char write_buffer[MAX_MESSAGE_LENGTH];

        // Sending

        printf("You: ");
        fgets(write_buffer, MAX_MESSAGE_LENGTH, stdin);
        write_buffer[strcspn(write_buffer, "\n")] = '\0';
        send_message(write_buffer, client_socket_fd);

        // Receiving

        receive_message(read_buffer, client_socket_fd);
    }


    close_client(client_socket_fd);
}

void prompt_user(char *username, char *port) {

    printf("Enter username: ");
    fgets(username, MAX_USERNAME_LENGTH, stdin);

    printf("Enter port: ");
    fgets(port, MAX_PORT_LENGTH, stdin);

    username[strcspn(username, "\n")] = '\0';
    port[strcspn(port, "\n")] = '\0';
}

// void register_user(char *username, char *port, int client_socket_fd) {
//     while (1) {
//
//         // Send user-registration information to the server to process.
//
//         send_message(username, client_socket_fd);
//     }
// }


