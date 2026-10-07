//
// Created by Andres Ortiz Osorio on 10/6/26.
//

/*
 * Server maintains a list of connected users
 * Receives messages from client, and resends messages to other clients
 * Simple protocol to register users
*/

#include <sys/socket.h>
#include <sys/errno.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "transport_info.h"
#include <unistd.h>

// Forward-Functions Defined

// Server Validation
void server_validate_username(char *username);
void server_validate_password(char *password);

// Server Related Setup
int create_server_socket();
void bind_server(int server_socket_fd, struct sockaddr_in addr);
void start_listening(int server_socket_fd);
int accept_client(int server_socket_fd, struct sockaddr_in addr, const socklen_t *addr_len);
void close_server(int fd);

// Main

int main() {

    // (Source) Server Address
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    addr.sin_family = DOMAIN_PROTOCOL;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Initial-TCP Setup
    const int server_socket_fd = create_server_socket();
    bind_server(server_socket_fd, addr);
    start_listening(server_socket_fd);


    // Receive Client Messages
    int client_socket_fd = accept_client(server_socket_fd, addr, &addr_len);
    close_server(server_socket_fd);
}

int create_server_socket() {
    int fd = socket(DOMAIN_PROTOCOL, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    return fd;
}

void bind_server(const int server_socket_fd, const struct sockaddr_in addr) {
    if (bind(server_socket_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
}

void start_listening(const int server_socket_fd) {
    if (listen(server_socket_fd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

int accept_client(int server_socket_fd, const struct sockaddr_in addr, const socklen_t *addr_len) {
    const int fd = accept(server_socket_fd, (struct sockaddr *) &addr, (socklen_t*) &addr_len);
    if (fd == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Client connected\n");
    return fd;
}

void close_server(const int fd) {
    int status = close(fd);
    if (status == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
}