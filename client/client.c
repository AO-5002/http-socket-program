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
#include "../common/protocol.h"

void server_registration(char *username, int client_socket_fd);
void prompt_user(char *username, char *port, char *server_hostname);

int main() {

    char username[MAX_USERNAME_LENGTH];
    char port[MAX_PORT_LENGTH];
    char server_hostname[250];

    // Prompt User
    // prompt_user(username, port, server_hostname);

    // (Destination) Server Address
    struct sockaddr_in dest_addr = {0};
    dest_addr.sin_family = DOMAIN_PROTOCOL;
    // const long port_int = strtol(port, NULL, 10);
    dest_addr.sin_port = htons(SERVER_PORT);
    dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);


    // Establish TCP-Client

    int client_socket_fd = create_client_socket();
    connect_to_server(client_socket_fd, dest_addr, sizeof(dest_addr));

    // Chatroom

    while (1) {
        char read_buffer[MAX_MESSAGE_LENGTH];
        char write_buffer[MAX_MESSAGE_LENGTH];

        // Sending

        printf("You: ");
        fgets(write_buffer, MAX_MESSAGE_LENGTH, stdin);
        write_buffer[strcspn(write_buffer, "\n")] = '\0';
        send_message(write_buffer, -1, client_socket_fd);

        // Receiving

        const ssize_t n = receive_message(read_buffer, client_socket_fd);
        if (n > 0) {
            printf("Server: %s\n", read_buffer);
        }
        else {
            printf("Server disconnected\n");
            break;
        }
    }


    close_client(client_socket_fd);
}

void prompt_user(char *username, char *port, char *server_hostname) {

    printf("Welcome to the HTTP-Socket Client!\n");
    printf("Please enter the server-hostname: ");
    fgets(server_hostname, 250, stdin);

    printf("Enter the server-port: ");
    fgets(port, MAX_PORT_LENGTH, stdin);

    printf("Enter your username: ");
    fgets(username, MAX_USERNAME_LENGTH, stdin);

    username[strcspn(username, "\n")] = '\0';
    port[strcspn(port, "\n")] = '\0';
    server_hostname[strcspn(server_hostname, "\n")] = '\0';
}

