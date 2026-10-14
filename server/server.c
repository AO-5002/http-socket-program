//
// Created by Andres Ortiz Osorio on 10/6/26.
//
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/_select.h>
#include "server_handlers.h"
#include "../common/protocol.h"
#include "../transport_info.h"
#include "server_setup.h"


// char *connected_clients[FD_SETSIZE];
CLIENT connected_clients[FD_SETSIZE] = {0};

int client_count = 0;
int server_socket_fd, MAX_SOCKET_SIZE;

// Main

int main() {

    // (Source) Server Address
    struct sockaddr_in addr;
    constexpr socklen_t addr_len = sizeof(addr);
    addr.sin_family = DOMAIN_PROTOCOL;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Initial-TCP Setup
    server_socket_fd = create_server_socket();
    bind_server(server_socket_fd, addr);
    start_listening(server_socket_fd);

    fd_set current_sockets;
    FD_ZERO (&current_sockets);
    FD_SET (server_socket_fd, &current_sockets);
    MAX_SOCKET_SIZE = server_socket_fd;

    while (1) {

        fd_set ready_sockets = current_sockets;
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (int fd = 0; fd <= MAX_SOCKET_SIZE + 1; fd++) {
            if (FD_ISSET(fd, &ready_sockets)) {
                if (fd == server_socket_fd) {

                    // New connection request on the server socket

                    int connected_client_fd = accept_client(server_socket_fd, addr, &addr_len);
                    printf("Client %d connected.\n", connected_client_fd);
                    FD_SET(connected_client_fd, &current_sockets);

                    if (connected_client_fd > MAX_SOCKET_SIZE) MAX_SOCKET_SIZE = connected_client_fd;
                }
                else {

                    // New data from an already-connected client-socket

                    CLIENT *client_data = &connected_clients[fd];
                    const ssize_t n = recv(fd, client_data->recv_buffer + client_data->bytes_received, sizeof client_data->recv_buffer - client_data->bytes_received - 1, 0);

                    // We can route the message and send an appropriate response.

                    if (n > 0) {
                        client_data->bytes_received += n;
                        client_data->recv_buffer[client_data->bytes_received] = '\0';

                        // Route every complete line, keep any partial line for the next recv
                        char *newline;
                        while ((newline = strchr(client_data->recv_buffer, '\n')) != NULL) {
                            *newline = '\0';
                            route_message(client_data->recv_buffer, fd, server_socket_fd, connected_clients, client_count);

                            const size_t consumed = newline - client_data->recv_buffer + 1;
                            client_data->bytes_received -= consumed;
                            memmove(client_data->recv_buffer, newline + 1, client_data->bytes_received + 1);
                        }
                    }
                    else if (n == 0) {

                        // If the client disconnects, remove fd from the socket-set, clear the client structure, and decrement client count.

                        FD_CLR(fd, &current_sockets);
                        if (is_registered(fd, connected_clients)) {
                            --client_count;

                            // Server-Log
                            printf("User %s disconnected.\n", connected_clients[fd].username);
                        }
                        else printf("Connection %d disconnected.\n", fd);

                        connected_clients[fd] = (CLIENT){0};
                        close(fd);
                    }
                    else {
                        FD_CLR(fd, &current_sockets);
                        close(fd);
                        perror("recv");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }
}