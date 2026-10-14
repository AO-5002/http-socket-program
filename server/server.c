//
// Created by Andres Ortiz Osorio on 10/6/26.
//


#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/_select.h>

#include "../common/protocol.h"
#include "server_validation.h"
#include "../transport_info.h"
#include "server_setup.h"

void route_message(char *read_buffer, int fd);
void create_username(char *src, char *dest);
void handle_register(MSG_PROTOCOL msg, int fd);
void handle_exit(int fd);
int is_registered(int fd);
void handle_unauthorized_message(int origin_fd);
void handle_default_message(int origin_fd);
void handle_broadcast_message(char *message, int origin_fd);
void handle_private_message(char *message, int origin_fd);

// char *connected_clients[FD_SETSIZE];
CLIENT connected_clients[FD_SETSIZE] = {0};

int client_count = 0;
int server_socket_fd, MAX_SOCKET_SIZE;

// Main

int main() {

    // (Source) Server Address
    struct sockaddr_in addr;
    constexpr socklen_t addr_len = sizeof(addr);
    addr.sin_family = DOMAIN_PROTOCOL;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Initial-TCP Setup
    server_socket_fd = create_server_socket();
    bind_server(server_socket_fd, addr);
    start_listening(server_socket_fd);

    fd_set current_sockets;
    FD_ZERO (&current_sockets);
    FD_SET (server_socket_fd, &current_sockets);
    MAX_SOCKET_SIZE = server_socket_fd;

    while (1) {

        fd_set ready_sockets = current_sockets;
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (int fd = 0; fd <= MAX_SOCKET_SIZE + 1; fd++) {
            if (FD_ISSET(fd, &ready_sockets)) {
                if (fd == server_socket_fd) {

                    // New connection request on the server socket

                    int connected_client_fd = accept_client(server_socket_fd, addr, &addr_len);
                    printf("Client %d connected.\n", connected_client_fd);
                    FD_SET(connected_client_fd, &current_sockets);

                    if (connected_client_fd > MAX_SOCKET_SIZE) MAX_SOCKET_SIZE = connected_client_fd;
                }
                else {

                    // New data from an already-connected client-socket

                    CLIENT *client_data = &connected_clients[fd];
                    const ssize_t n = recv(fd, client_data->recv_buffer + client_data->bytes_received, sizeof client_data->recv_buffer - client_data->bytes_received - 1, 0);

                    // We can route the message and send an appropriate response.

                    if (n > 0) {
                        client_data->bytes_received += n;
                        client_data->recv_buffer[client_data->bytes_received] = '\0';

                        // Route every complete line, keep any partial line for the next recv
                        char *newline;
                        while ((newline = strchr(client_data->recv_buffer, '\n')) != NULL) {
                            *newline = '\0';
                            route_message(client_data->recv_buffer, fd);

                            const size_t consumed = newline - client_data->recv_buffer + 1;
                            client_data->bytes_received -= consumed;
                            memmove(client_data->recv_buffer, newline + 1, client_data->bytes_received + 1);
                        }
                    }
                    else if (n == 0) {

                        // If the client disconnects, remove fd from the socket-set, clear the client structure, and decrement client count.

                        FD_CLR(fd, &current_sockets);
                        if (is_registered(fd)) {
                            --client_count;

                            // Server-Log
                            printf("User %s disconnected.\n", connected_clients[fd].username);
                        }
                        else printf("Connection %d disconnected.\n", fd);

                        connected_clients[fd] = (CLIENT){0};
                        close(fd);
                    }
                    else {
                        FD_CLR(fd, &current_sockets);
                        close(fd);
                        perror("recv");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }
}

int is_registered(const int fd) {
    if (connected_clients[fd].username[0] == '\0') return 0;
    return 1;
}

void route_message(char *read_buffer, const int fd) {

    // Parse the message of its msg-type and contents.

    const MSG_PROTOCOL msg = parse_message(read_buffer);

    // Handle user-authorization

    if (msg.type == EXIT) { handle_exit(fd); }
    if (msg.type == REG && is_registered(fd) == 0) { handle_register(msg, fd); }
    else if (msg.type != REG && is_registered(fd) == 0) { handle_unauthorized_message(fd); }
    else {

        // If the user is authorized, then we process their message.

        switch (msg.type) {
            case MESG: { handle_broadcast_message(msg.message, fd); break; }
            case PMSG: { handle_private_message(msg.message, fd); break;}
            default: { handle_default_message(fd); break; }
        }
    }
}

void handle_unauthorized_message(const int origin_fd) {

    // Server-Log
    printf("Connection %d - Attempted to send a message without being registered.\n", origin_fd);

    // Message to origin_fd
    const char *message = "Please register by writing 'REG <username>'.\n";
    send_message(message, server_socket_fd, origin_fd);
}

void handle_default_message(const int origin_fd) {

    // Server log
    printf("(%s) - Attempted to send an Invalid message type.\n", connected_clients[origin_fd].username);

    // Private message to origin_fd
    char message[MAX_MESSAGE_LENGTH];
    snprintf(message, MAX_MESSAGE_LENGTH, "%s Messages require {REG, MESG, PMSG, EXIT} prefix.\n", connected_clients[origin_fd].username);
    handle_private_message(message, server_socket_fd);
}

void handle_broadcast_message(char *message, const int origin_fd) {
    // Server log
    printf("(%s -> ALL) : %s\n", connected_clients[origin_fd].username, message);
    broadcast_message(message, origin_fd, connected_clients);
}

void handle_exit(const int fd) {
    printf("User '%s' disconnected.\n", connected_clients[fd].username);
    exit(EXIT_SUCCESS);
}

void handle_register(const MSG_PROTOCOL msg, const int fd) {
    const int status = register_new_user(msg.message, connected_clients);
    if (status == 0) {

        // Add the new user to the client list
        snprintf(connected_clients[fd].username, sizeof connected_clients[fd].username, "%s", msg.message);
        ++client_count;

        // Start building an ACK message for the client

        char message[MAX_MESSAGE_LENGTH];
        snprintf(message, MAX_MESSAGE_LENGTH, "ACK %d ", client_count);

        // List all the connected users

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (connected_clients[i].username[0] != '\0') {
                snprintf(message + strlen(message), sizeof message - strlen(message) , "%s ", connected_clients[i].username);
            }
        }

        send_message(message, server_socket_fd, fd);

        // Server logging
        printf("User '%s' registered.\n", connected_clients[fd].username);
    }
    else if (status == INVALID_USERNAME) {
        send_message("ERR INVALID_USERNAME", server_socket_fd, fd);
    }
    else if (status == USERNAME_TAKEN) {
        send_message("ERR USERNAME_TAKEN", server_socket_fd, fd);
    }
}

void handle_private_message(char *message, const int origin_fd) {

    // Parse for the destination user (assumes the message is formatted like this ___ ___)

    int i = 0;
    char destination_user[MAX_USERNAME_LENGTH];
    while (message[i] != ' ') {
        destination_user[i] = message[i];
        i++;
    }
    destination_user[i] = '\0';
    message = message + i + 1;

    // Search for the target-user and return its fd.
    const int dest_fd = return_user_fd(destination_user, connected_clients);

    // Send the message to target-user
    send_message(message, origin_fd, dest_fd);

    // Server Logging
    printf("(%s -> %s) : %s\n", connected_clients[origin_fd].username, destination_user, message);
}

