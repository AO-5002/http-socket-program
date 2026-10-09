//
// Created by Andres Ortiz Osorio on 10/6/26.
//


#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "../transport_info.h"
#include "server_setup.h"

// Main

int main() {

    // (Source) Server Address
    struct sockaddr_in addr;
    constexpr socklen_t addr_len = sizeof(addr);
    addr.sin_family = DOMAIN_PROTOCOL;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Initial-TCP Setup
    const int server_socket_fd = create_server_socket();
    bind_server(server_socket_fd, addr);
    start_listening(server_socket_fd);

    // Receive Client Messages
    int count = 0;
    while (count < MAX_USER_CONNECTIONS) {
        int client_socket_fd = accept_client(server_socket_fd, addr, &addr_len);
        printf("User %d connected\n", client_socket_fd);
        count++;
    }

    close_server(server_socket_fd);
}