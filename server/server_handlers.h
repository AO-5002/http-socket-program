//
// Created by Andres Ortiz Osorio on 10/14/26.
//

#ifndef HTTP_SOCKET_PROGRAM_SERVER_HANDLERS_H
#define HTTP_SOCKET_PROGRAM_SERVER_HANDLERS_H

#include "../common/protocol.h"

void route_message(char *read_buffer, int fd, int server_socket_fd, CLIENT connected_clients[], int client_count);
void create_username(char *src, char *dest);
void handle_register(MSG_PROTOCOL msg, int fd, int server_socket_fd, CLIENT connected_clients[], int client_count);
void handle_exit(int fd, CLIENT connected_clients[]);
int is_registered(int fd, CLIENT connected_clients[]);
void handle_unauthorized_message(int origin_fd, int server_socket_fd);
void handle_default_message(int origin_fd, int server_socket_fd, CLIENT connected_clients[]);
void handle_broadcast_message(char *message, int origin_fd, CLIENT connected_clients[]);
void handle_private_message(char *message, int origin_fd, CLIENT connected_clients[]);


#endif //HTTP_SOCKET_PROGRAM_SERVER_HANDLERS_H