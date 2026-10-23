//
// Created by Andres Ortiz Osorio on 10/9/26.
//

#ifndef HTTP_SOCKET_PROGRAM_PROTOCOL_H
#define HTTP_SOCKET_PROGRAM_PROTOCOL_H
#include <stdio.h>

#include "../transport_info.h"
#include <sys/types.h>

typedef enum { REG, ACK, MESG, PMSG, ERR, EXIT, UNKOWN } MSG_TYPE;
typedef struct {
    MSG_TYPE type;
    char *message;
} MSG_PROTOCOL;

typedef struct {
    char username[MAX_USERNAME_LENGTH + 1];
    char recv_buffer[MAX_PROTOCOL_LENGTH];
    size_t bytes_received;
    int discarding;
} CLIENT;

// Protocol Specific
int return_user_fd(const char *target, CLIENT connected_clients[]);
void send_message(const char *message, int origin_fd, int dest_fd);
ssize_t receive_message(char* read_buffer, int client_socket_fd);
void broadcast_message(const char *message, int origin_fd, CLIENT connected_clients[]);
MSG_PROTOCOL parse_message(char *message);

#endif