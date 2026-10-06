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

// Forward-Functions Defined

bool server_validate_username(char *username);
bool server_validate_password(char *password);

int main() {

    int opt = 1;
    int server_socket_fd;

    if ((server_socket_fd = socket(DOMAIN_PROTOCOL, SOCK_STREAM, 0)) != -1) {

        // Restart the porting
        if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt,
               sizeof(opt))) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
               }

        // (Source) Server Address
        struct sockaddr_in addr;
        addr.sin_family = DOMAIN_PROTOCOL;
        addr.sin_port = htons(SERVER_PORT);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(server_socket_fd, (struct sockaddr *) &addr, sizeof(addr)) != -1) {

            // Listen for incoming connections

            if (listen(server_socket_fd, BACKLOG) != -1) {
                printf("Server listening on port %d\n", SERVER_PORT);

                // Accept client connections

                int client_socket_fd;
                socklen_t addr_len = sizeof(addr);
                if ((client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&addr, &addr_len)) != -1) {
                    printf("Client connected: %d\n", client_socket_fd);
                }
                else printf("Error accepting client connection. (%s)", strerror(errno));
            } else printf("Error listening on server socket. (%s)", strerror(errno));
        } else printf("Error binding server socket. (%s)", strerror(errno));
    } else printf("Error creating server socket. (%s)", strerror(errno));

    return 0;
}
