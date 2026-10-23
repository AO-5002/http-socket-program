//
// Created by Andres Ortiz Osorio on 10/14/26.
//

#ifndef HTTP_SOCKET_PROGRAM_SERVER_HANDLERS_H
#define HTTP_SOCKET_PROGRAM_SERVER_HANDLERS_H

#include "../common/protocol.h"

void route_message(char *read_buffer, int fd, CLIENT connected_clients[]);
void handle_register(const char *username, int fd, CLIENT connected_clients[]);
void handle_exit(int fd, CLIENT connected_clients[]);
void handle_disconnect(int fd, CLIENT connected_clients[]);
int is_registered(int fd, CLIENT connected_clients[]);
void send_error(int fd, int error_code);
void send_ack(int fd, CLIENT connected_clients[]);
void handle_broadcast_message(const char *message, int origin_fd, CLIENT connected_clients[]);
void handle_private_message(const char *message, int origin_fd, CLIENT connected_clients[]);


#endif