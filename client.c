//
// Created by Andres Ortiz Osorio on 10/6/26.
//

/*
* Client can send private messages to other users
* Client can broadcast messages to all users
* Simple protocol to register users, and exchange messages
*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include "transport_info.h"

void register_user(char **username);
bool validate_username(char *username);
bool validate_password(char *password);

int main() {

    int client_socket_fd;
    if ((client_socket_fd = socket(DOMAIN_PROTOCOL, SOCK_STREAM, 0)) != -1) {
        printf("Successfully created client socket. \n");

        // (Source) Client Address
        struct sockaddr_in addr;
        addr.sin_family = DOMAIN_PROTOCOL;
        addr.sin_port = htons(CLIENT_PORT);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        // (Destination) Server Address
        struct sockaddr_in dest_addr = {0};
        dest_addr.sin_family = DOMAIN_PROTOCOL;
        dest_addr.sin_port = htons(SERVER_PORT);
        dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        // Connect to the server socket
        int dest_addr_len = sizeof(dest_addr);
        if (connect(client_socket_fd, (struct sockaddr*) &dest_addr, dest_addr_len) != -1) {
            printf("Successfully connected to server socket.\n");
        }
        else printf("Error connecting to server socket. (%s)", strerror(errno));
    }
    else printf("Error creating client socket. (%s)", strerror(errno));
    return 0;
}

// User Validation
void register_user(char **username) {
    char username_input[MAX_USERNAME_LENGTH], password_input[MAX_PASSWORD_LENGTH];

    // Get user inputs & validate

    do {
        printf("Username: ");
        fgets(username_input, MAX_USERNAME_LENGTH, stdin);
        printf("Password: ");
        fgets(password_input, MAX_PASSWORD_LENGTH, stdin);
    } while (!validate_username(username_input) || !validate_password(password_input));

    // Register user

    *username = username_input;
    printf("User registered successfully!\n");
}

bool validate_username(char *username) {
    return true;
}

bool validate_password(char *password) {
    return false;
}