//
// Created by Andres Ortiz Osorio on 10/6/26.
//


#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include "../common/protocol.h"
#include "../transport_info.h"
#include "server_setup.h"

void handle_message(char *read_buffer);

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
    int client_socket_fd = accept_client(server_socket_fd, addr, &addr_len);
    printf("Client connected\n");
    while (1) {

        char read_buffer[MAX_MESSAGE_LENGTH];
        const ssize_t n = recv(client_socket_fd, read_buffer, 100, 0);

        if (n > 0) handle_message(read_buffer);
        else if (n == 0) {
            printf("Client disconnected\n");
            break;
        }
        else {
            perror("recv");
            exit(EXIT_FAILURE);
        }
    }

    close_server(server_socket_fd);
}

void handle_message(char *read_buffer) {

    // Parse the message of its msg-type and contents.
    const MSG_PROTOCOL msg = parse_message(read_buffer);
    printf("Message received: %s\n", msg.message);

    if (msg.type == EXIT) {
        printf("Client disconnected\n");
        exit(EXIT_SUCCESS);
    }
}