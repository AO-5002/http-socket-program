//
// Created by Andres Ortiz Osorio on 10/8/26.
//
#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#ifndef HTTP_SOCKET_PROGRAM_CLIENT_SETUP_H
#define HTTP_SOCKET_PROGRAM_CLIENT_SETUP_H



int create_client_socket();
void connect_to_server(int client_socket_fd, struct sockaddr_in dest_addr, socklen_t dest_addr_len);
void close_client(int fd);

#endif //HTTP_SOCKET_PROGRAM_CLIENT_SETUP_H