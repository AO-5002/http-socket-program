//
// Created by Andres Ortiz Osorio on 10/14/26.
//

#include "server_handlers.h"
#include <stdio.h>
#include <string.h>
#include <sys/select.h>

#include "../transport_info.h"
#include "../common/protocol.h"
#include "../server/server_validation.h"

int is_registered(const int fd, CLIENT connected_clients[]) {
    if (connected_clients[fd].username[0] == '\0') return 0;
    return 1;
}

void route_message(char *read_buffer, const int fd, CLIENT connected_clients[]) {

    // Parse the message of its msg-type and contents.

    const MSG_PROTOCOL msg = parse_message(read_buffer);
    const int registered = is_registered(fd, connected_clients);

    // Handle user-authorization: only REG is allowed before registering.

    if (msg.type != REG && msg.type != UNKOWN && !registered) {

        // Server-Log
        printf("Connection %d - Attempted to send a message without being registered.\n", fd);
        send_error(fd, INVALID_MESSAGE);
        return;
    }

    // If the user is authorized, then we process their message.

    switch (msg.type) {
        case REG: {
            if (registered) send_error(fd, INVALID_MESSAGE);    // Already registered, keep the old name.
            else handle_register(msg.message, fd, connected_clients);
            break;
        }
        case MESG: { handle_broadcast_message(msg.message, fd, connected_clients); break; }
        case PMSG: { handle_private_message(msg.message, fd, connected_clients); break; }
        case EXIT: {
            if (msg.message[0] != '\0') send_error(fd, INVALID_MESSAGE);   // EXIT takes no fields.
            else handle_exit(fd, connected_clients);
            break;
        }
        default: {

            // Server log
            printf("Connection %d - Sent an invalid message: '%s'\n", fd, read_buffer);
            send_error(fd, INVALID_MESSAGE);
            break;
        }
    }
}

void send_error(const int fd, const int error_code) {
    char message[16];
    snprintf(message, sizeof message, "ERR %d", error_code);
    send_message(message, -1, fd);
}

void send_ack(const int fd, CLIENT connected_clients[]) {

    // ACK <#users> <userlist>

    char user_list[FD_SETSIZE * (MAX_USERNAME_LENGTH + 1) + 1] = {0};
    size_t length = 0;
    int count = 0;

    for (int i = 0; i < FD_SETSIZE; i++) {
        if (connected_clients[i].username[0] != '\0') {
            length += snprintf(user_list + length, sizeof user_list - length, " %s", connected_clients[i].username);
            count++;
        }
    }

    char message[sizeof user_list + 16];
    snprintf(message, sizeof message, "ACK %d%s", count, user_list);
    send_message(message, -1, fd);
}

void handle_broadcast_message(const char *message, const int origin_fd, CLIENT connected_clients[]) {

    // Validate: 1 to 512 characters.

    const size_t length = strlen(message);
    if (length < MIN_MESSAGE_LENGTH || length > MAX_MESSAGE_LENGTH) {
        send_error(origin_fd, INVALID_MESSAGE);
        return;
    }

    // Build message

    char server_message[MAX_PROTOCOL_LENGTH];
    snprintf(server_message, sizeof server_message, "MSG %s %s", connected_clients[origin_fd].username, message);

    // Server log
    printf("(%s -> ALL) : %s\n", connected_clients[origin_fd].username, message);
    broadcast_message(server_message, origin_fd, connected_clients);
}

void handle_exit(const int fd, CLIENT connected_clients[]) {

    // Remove the user first, then ACK with the updated list, then tell everyone else.
    // Only clear the username: the recv_buffer is still being read by the caller.
    // The client closes the connection after receiving the ACK.

    char username[MAX_USERNAME_LENGTH + 1];
    snprintf(username, sizeof username, "%s", connected_clients[fd].username);
    connected_clients[fd].username[0] = '\0';

    send_ack(fd, connected_clients);

    printf("User '%s' left the chat.\n", username);

    char b_message[MAX_PROTOCOL_LENGTH];
    snprintf(b_message, sizeof b_message, "MSG SERVER %s has left the chat", username);
    broadcast_message(b_message, fd, connected_clients);
}

void handle_disconnect(const int fd, CLIENT connected_clients[]) {

    // Connection closed without an EXIT: remove the user and notify everyone else.

    if (is_registered(fd, connected_clients)) {
        char username[MAX_USERNAME_LENGTH + 1];
        snprintf(username, sizeof username, "%s", connected_clients[fd].username);
        connected_clients[fd] = (CLIENT){0};

        printf("User '%s' disconnected.\n", username);

        char b_message[MAX_PROTOCOL_LENGTH];
        snprintf(b_message, sizeof b_message, "MSG SERVER %s has left the chat", username);
        broadcast_message(b_message, fd, connected_clients);
    }
    else {
        connected_clients[fd] = (CLIENT){0};
        printf("Connection %d disconnected.\n", fd);
    }
}

void handle_register(const char *username, const int fd, CLIENT connected_clients[]) {
    const int status = register_new_user(username, connected_clients);
    if (status != -1) {

        // Server logging
        printf("Connection %d - Registration of '%s' rejected (ERR %d).\n", fd, username, status);
        send_error(fd, status);
        return;
    }

    // Add the new user to the client list, then ACK them.

    snprintf(connected_clients[fd].username, sizeof connected_clients[fd].username, "%s", username);
    send_ack(fd, connected_clients);

    // Server logging
    printf("User '%s' registered.\n", connected_clients[fd].username);

    // Broadcast message to all other users

    char b_message[MAX_PROTOCOL_LENGTH];
    snprintf(b_message, sizeof b_message, "MSG SERVER %s has joined the chat", connected_clients[fd].username);
    broadcast_message(b_message, fd, connected_clients);
}

void handle_private_message(const char *message, const int origin_fd, CLIENT connected_clients[]) {

    // Format: <user> <message>

    const char *space = strchr(message, ' ');
    if (space == NULL || space == message || space[1] == '\0' || strlen(space + 1) > MAX_MESSAGE_LENGTH) {
        send_error(origin_fd, INVALID_MESSAGE);
        return;
    }

    // Names longer than the max can't belong to anyone.

    const size_t name_length = space - message;
    if (name_length > MAX_USERNAME_LENGTH) {
        send_error(origin_fd, UNKNOWN_USER_PMSG);
        return;
    }

    char destination_user[MAX_USERNAME_LENGTH + 1];
    memcpy(destination_user, message, name_length);
    destination_user[name_length] = '\0';
    message = space + 1;

    // Search for the target-user and return its fd.

    const int dest_fd = return_user_fd(destination_user, connected_clients);
    if (dest_fd == -1) {
        send_error(origin_fd, UNKNOWN_USER_PMSG);
        return;
    }

    // Send the message to target-user

    char complete_message[MAX_PROTOCOL_LENGTH];
    snprintf(complete_message, sizeof complete_message, "MSG %s %s", connected_clients[origin_fd].username, message);
    send_message(complete_message, -1, dest_fd);

    // Server Logging
    printf("(%s -> %s) : %s\n", connected_clients[origin_fd].username, destination_user, message);
}