//
// Created by Andres Ortiz Osorio on 10/8/26.
//

#include <sys/socket.h>
#include "server_setup.h"
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "../transport_info.h"
#include <unistd.h>

// Server-Connection Functions

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