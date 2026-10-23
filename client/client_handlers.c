//
// Created by Andres Ortiz Osorio on 10/20/26.
//

#include "client_handlers.h"
#include "../transport_info.h"
#include "../common/protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

char recv_buffer[RECV_LENGTH];
size_t bytes_received = 0;
char users[FD_SETSIZE][MAX_USERNAME_LENGTH + 1];
int user_count = 0;


void handle_user_input(char *input, const int client_socket_fd, int *exiting) {

    if (input[0] == '\0') return;

    // LIST is local: the protocol has no list request, so print the list we track.
    if (strcmp(input, "LIST") == 0) {
        print_users();
        return;
    }

    // Everything else is sent as typed (MESG, PMSG, EXIT); the server validates it.
    if (strcmp(input, "EXIT") == 0) *exiting = 1;
    send_message(input, -1, client_socket_fd);
}

// Returns 1 when the client should quit.
int handle_server_line(const char *line, const int exiting) {

    // Show every server message as received (ACK, MSG, ERR).
    printf("%s\n", line);

    if (strncmp(line, "MSG ", 4) == 0) {

        // MSG <username> <message>
        const char *sender = line + 4;
        const char *space = strchr(sender, ' ');
        if (space == NULL) return 0;
        const char *text = space + 1;

        // Keep the local user list current from SERVER join/leave notices.
        if (strncmp(sender, "SERVER ", 7) == 0) {
            const char *joined = " has joined the chat";
            const char *left = " has left the chat";
            const size_t len = strlen(text);
            char name[MAX_USERNAME_LENGTH + 1];

            if (len > strlen(joined) && strcmp(text + len - strlen(joined), joined) == 0 && user_count < FD_SETSIZE) {
                snprintf(name, sizeof name, "%.*s", (int)(len - strlen(joined)), text);
                snprintf(users[user_count++], sizeof users[0], "%s", name);
            }
            else if (len > strlen(left) && strcmp(text + len - strlen(left), left) == 0) {
                snprintf(name, sizeof name, "%.*s", (int)(len - strlen(left)), text);
                for (int i = 0; i < user_count; i++) {
                    if (strcmp(users[i], name) == 0) {
                        if (i != --user_count) memcpy(users[i], users[user_count], sizeof users[0]);
                        break;
                    }
                }
            }
        }
    }
    else if (strncmp(line, "ERR ", 4) == 0) {
        print_error(line);
    }
    else if (strncmp(line, "ACK ", 4) == 0) {
        set_users_from_ack(line);
        if (exiting) {
            printf("You left the chat.\n");
            return 1;
        }
    }
    return 0;
}

void print_error(const char *err) {
    switch (atoi(err + 4)) {
        case USERNAME_TAKEN: printf("Error: that username is already taken.\n"); break;
        case INVALID_USERNAME: printf("Error: invalid username (1-32 characters, no spaces).\n"); break;
        case UNKNOWN_USER_PMSG: printf("Error: no user with that name is in the chat.\n"); break;
        case INVALID_MESSAGE: printf("Error: invalid or unexpected message.\n"); break;
        default: printf("Error from server: %s\n", err); break;
    }
}

void set_users_from_ack(const char *ack) {

    // ACK <#users> <userlist>
    user_count = 0;
    const char *p = strchr(ack + 4, ' ');
    while (p != NULL && user_count < FD_SETSIZE) {
        p++;
        const size_t len = strcspn(p, " ");
        if (len > 0) snprintf(users[user_count++], sizeof users[0], "%.*s", (int) len, p);
        p = strchr(p, ' ');
    }
}

void print_users(void) {
    printf("%d user(s) in the chatroom:", user_count);
    for (int i = 0; i < user_count; i++) printf(" %s", users[i]);
    printf("\n");
}

// One recv into the buffer; exits if the server is gone.
void fill_buffer(const int fd) {
    if (bytes_received >= sizeof recv_buffer - 1) bytes_received = 0;    // Line too long, drop it.

    const ssize_t n = recv(fd, recv_buffer + bytes_received, sizeof recv_buffer - bytes_received - 1, 0);
    if (n <= 0) {
        printf("Server disconnected.\n");
        exit(n == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
    }
    bytes_received += n;
    recv_buffer[bytes_received] = '\0';
}

// Moves one complete line (without '\n') into line. Returns 0 if no full line is buffered yet.
int pop_line(char *line, const size_t size) {
    char *newline = memchr(recv_buffer, '\n', bytes_received);
    if (newline == NULL) return 0;

    *newline = '\0';
    if (newline > recv_buffer && newline[-1] == '\r') newline[-1] = '\0';
    snprintf(line, size, "%s", recv_buffer);

    const size_t consumed = newline - recv_buffer + 1;
    bytes_received -= consumed;
    memmove(recv_buffer, newline + 1, bytes_received + 1);
    return 1;
}

// Blocks until a full line arrives.
void read_line(const int fd, char *line, const size_t size) {
    while (!pop_line(line, size)) fill_buffer(fd);
}

void read_input(const char *prompt, char *buffer, const size_t size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buffer, (int) size, stdin) == NULL) exit(EXIT_SUCCESS);

    // Drop anything past the buffer so it doesn't spill into the next prompt.
    if (strchr(buffer, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

void prompt_user(char *username, char *port, char *server_hostname) {

    printf("Welcome to the HTTP-Socket Client!\n");
    read_input("Please enter the server-hostname: ", server_hostname, INPUT_LENGTH);
    read_input("Enter the server-port: ", port, INPUT_LENGTH);
    read_input("Enter your username: ", username, INPUT_LENGTH);
}