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
#include "transport_info.h"
#include <unistd.h>

int create_client_socket();
void connect_to_server(int client_socket_fd, struct sockaddr_in dest_addr, socklen_t dest_addr_len);
void close_client(int fd);


int main() {

    // User gets registered

    char username[100];
    char password[100];
    char port[4];

    printf("Enter username: ");
    fgets(username, 100, stdin);

    printf("Enter password: ");
    fgets(password, 100, stdin);

    printf("Enter server port: ");
    fgets(port, 4, stdin);


    // Establish TCP-Client & send username & password

    // (Destination) Server Address
    struct sockaddr_in dest_addr = {0};
    dest_addr.sin_family = DOMAIN_PROTOCOL;
    dest_addr.sin_port = htons(SERVER_PORT);
    dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int client_socket_fd = create_client_socket();
    connect_to_server(client_socket_fd, dest_addr, sizeof(dest_addr));

    close_client(client_socket_fd);
}

void send_message(char *message, int client_socket_fd) {
    ssize_t status = send(client_socket_fd, message, strlen(message), 0);
    if (status == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }
}

int create_client_socket() {
    int fd = socket(DOMAIN_PROTOCOL, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    return fd;
}

void connect_to_server(int client_socket_fd, struct sockaddr_in dest_addr, const socklen_t dest_addr_len) {
    int status = connect(client_socket_fd, (struct sockaddr*) &dest_addr, dest_addr_len);
    if (status == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");
}

void close_client(int fd) {
    int status = close(fd);
    if (status == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
}