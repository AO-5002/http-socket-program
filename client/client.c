//
// Created by Andres Ortiz Osorio on 10/6/26.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../transport_info.h"
#include <unistd.h>
#include "client_setup.h"
#include "client_handlers.h"
#include "../common/protocol.h"

int main() {

    char username[INPUT_LENGTH];
    char port[INPUT_LENGTH];
    char server_hostname[INPUT_LENGTH];

    // Prompt User
    prompt_user(username, port, server_hostname);

    // (Destination) Server Address: resolve the hostname or IP.
    struct addrinfo hints = {0}, *result;
    hints.ai_family = DOMAIN_PROTOCOL;
    hints.ai_socktype = SOCK_STREAM;
    const int status = getaddrinfo(server_hostname, port, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "Could not resolve %s:%s (%s)\n", server_hostname, port, gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    const struct sockaddr_in dest_addr = *(struct sockaddr_in *) result->ai_addr;
    freeaddrinfo(result);

    // Establish TCP-Client

    int client_socket_fd = create_client_socket();
    connect_to_server(client_socket_fd, dest_addr, sizeof(dest_addr));

    // Registration: retry until the server ACKs a username.

    char line[RECV_LENGTH];
    while (1) {
        char reg[INPUT_LENGTH + 8];
        snprintf(reg, sizeof reg, "REG %s", username);
        send_message(reg, -1, client_socket_fd);

        read_line(client_socket_fd, line, sizeof line);
        printf("%s\n", line);
        if (strncmp(line, "ACK ", 4) == 0) break;

        if (strncmp(line, "ERR ", 4) == 0) print_error(line);
        else printf("Unexpected reply from server: %s\n", line);
        read_input("Enter a different username: ", username, sizeof username);
    }

    set_users_from_ack(line);
    printf("Registration successful.\n");
    printf("Commands: 'MESG <message>' to broadcast, 'PMSG <user> <message>' to message privately, 'LIST' for users, 'EXIT' to leave.\n");

    // Chatroom
    int exiting = 0;
    int done = 0;

    // Anything that arrived together with the ACK.
    while (!done && pop_line(line, sizeof line)) done = handle_server_line(line, exiting);
    while (!done) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        FD_SET(client_socket_fd, &fds);

        if (select(client_socket_fd + 1, &fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // Sending

        if (FD_ISSET(STDIN_FILENO, &fds) && !exiting) {
            char input[MAX_PROTOCOL_LENGTH];
            if (fgets(input, sizeof input, stdin) == NULL) {

                // EXIT to leave the chat
                send_message("EXIT", -1, client_socket_fd);
                exiting = 1;
            }
            else if (strchr(input, '\n') == NULL && !feof(stdin)) {

                // If message too long, drop it.
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {}
                printf("Message too long (max %d characters).\n", MAX_MESSAGE_LENGTH);
            }
            else {
                input[strcspn(input, "\n")] = '\0';
                handle_user_input(input, client_socket_fd, &exiting);
            }
        }

        // Receiving

        if (FD_ISSET(client_socket_fd, &fds)) {
            fill_buffer(client_socket_fd);
            while (!done && pop_line(line, sizeof line)) done = handle_server_line(line, exiting);
        }
    }

    close_client(client_socket_fd);
}