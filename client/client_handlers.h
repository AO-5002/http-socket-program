//
// Created by Andres Ortiz Osorio on 10/20/26.
//

#ifndef HTTP_SOCKET_PROGRAM_CLIENT_HANDLERS_H
#define HTTP_SOCKET_PROGRAM_CLIENT_HANDLERS_H

#include <stddef.h>
#include <sys/select.h>
#include "../transport_info.h"

#define INPUT_LENGTH 256
#define RECV_LENGTH (FD_SETSIZE * (MAX_USERNAME_LENGTH + 1) + MAX_PROTOCOL_LENGTH)

extern char recv_buffer[RECV_LENGTH];
extern size_t bytes_received;
extern char users[FD_SETSIZE][MAX_USERNAME_LENGTH + 1];
extern int user_count;

void prompt_user(char *username, char *port, char *server_hostname);
void read_input(const char *prompt, char *buffer, size_t size);
void fill_buffer(int fd);
int pop_line(char *line, size_t size);
void read_line(int fd, char *line, size_t size);
void set_users_from_ack(const char *ack);
void print_users(void);
void print_error(const char *err);
int handle_server_line(const char *line, int exiting);
void handle_user_input(char *input, int client_socket_fd, int *exiting);

#endif