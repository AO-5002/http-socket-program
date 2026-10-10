//
// Created by Andres Ortiz Osorio on 10/9/26.
//

#ifndef HTTP_SOCKET_PROGRAM_PROTOCOL_H
#define HTTP_SOCKET_PROGRAM_PROTOCOL_H

typedef enum { REG, ACK, MESG, PMSG, ERR, EXIT, UNKOWN} MSG_TYPE;
typedef struct {
    MSG_TYPE type;
    char *message;
} MSG_PROTOCOL;

void send_message(const char *message, int client_socket_fd);
void receive_message(char* read_buffer, int client_socket_fd);
MSG_PROTOCOL parse_message(char *message);

#endif //HTTP_SOCKET_PROGRAM_PROTOCOL_H