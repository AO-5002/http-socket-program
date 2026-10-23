//
// Created by Andres Ortiz Osorio on 10/8/26.
//

#include "server_validation.h"
#include "../transport_info.h"
#include "../common/protocol.h"
#include <stdio.h>
#include <string.h>
#include <sys/select.h>

int register_new_user(const char *username, CLIENT user_list[]) {

    // Validate the username
    if (validate_username(username) == 0) return INVALID_USERNAME;

    // Check if username is taken
    if (is_username_taken(username, user_list) == 1) return USERNAME_TAKEN;

    return -1;
}

int validate_username(const char *username) {

    // Check if the username is 1 to 32 chars.
    if (strlen(username) < MIN_USERNAME_LENGTH || strlen(username) > MAX_USERNAME_LENGTH) return 0;

    // Contains no spaces
    for (size_t i = 0; username[i] != '\0'; i++) {
        if (username[i] == ' ' || username[i] == '\n') return 0;
    }

    return 1;
}

int is_username_taken(const char *username, CLIENT user_list[]) {

    // Check if the list is empty
    if (user_list == NULL) return 0;

    // Check every slot: the list is indexed by fd, so empty slots can sit between users.
    for (size_t i = 0; i < FD_SETSIZE; i++) {
        if (user_list[i].username[0] != '\0' && strcmp(username, user_list[i].username) == 0) return 1;
    }

    return 0;
}