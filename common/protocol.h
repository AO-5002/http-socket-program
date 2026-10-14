//
// Created by Andres Ortiz Osorio on 10/9/26.
//

#ifndef HTTP_SOCKET_PROGRAM_PROTOCOL_H
#define HTTP_SOCKET_PROGRAM_PROTOCOL_H
#include <stdio.h>

#include "../transport_info.h"
#include <sys/_types/_ssize_t.h>

typedef enum { REG, ACK, MESG, PMSG, ERR, EXIT, UNKOWN } MSG_TYPE;
typedef struct {
    MSG_TYPE type;
    char *message;
} MSG_PROTOCOL;

typedef struct {
    int fd;
    char username[MAX_USERNAME_LENGTH];
    char recv_buffer[MAX_MESSAGE_LENGTH];
    size_t bytes_received;
} CLIENT;


// Protocol specific
int return_user_fd(const char *target, char *connected_clients[]);
void send_message(const char *message, int origin_fd, int dest_fd);
ssize_t receive_message(char* read_buffer, int client_socket_fd);
void broadcast_message(const char *message, int origin_fd, char *connected_clients[]);
MSG_PROTOCOL parse_message(char *message);

#endif //HTTP_SOCKET_PROGRAM_PROTOCOL_H