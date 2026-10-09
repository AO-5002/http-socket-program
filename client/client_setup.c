//
// Created by Andres Ortiz Osorio on 10/8/26.
//

#include "client_setup.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include "../transport_info.h"
#include <unistd.h>

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