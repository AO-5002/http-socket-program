//
// Created by Andres Ortiz Osorio on 10/8/26.
//

#ifndef HTTP_SOCKET_PROGRAM_SERVER_SETUP_H
#define HTTP_SOCKET_PROGRAM_SERVER_SETUP_H

#include <sys/socket.h>
#include <netinet/in.h>

// Server-Related Setup (TCP)

/*
 *  1. We first create a socket.
 *  2. Then, we bind that socket to our server, and
 *      we provide server addressing-information
 *      such as domain, port, allowed incoming IP Addresses.
 *  3. Then, we start listening for any incoming client-requests.
 *  4. Once a request comes through, we connect our server socket to the client socket.
 */

int create_server_socket();
void bind_server(int server_socket_fd, struct sockaddr_in addr);
void start_listening(int server_socket_fd);
int accept_client(int server_socket_fd, struct sockaddr_in addr, const socklen_t *addr_len);
void close_server(int fd);

#endif //HTTP_SOCKET_PROGRAM_SERVER_SETUP_H