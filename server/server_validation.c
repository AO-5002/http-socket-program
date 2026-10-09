//
// Created by Andres Ortiz Osorio on 10/8/26.
//

#include "server_validation.h"

#include <string.h>

int validate_username(const char *username) {

    // Check if the username is 1 to 32 chars.
    if (strlen(username) < 1 || strlen(username) > 32) return 0;

    // Contains no spaces
    for (size_t i = 0; username[i] != '\0'; i++) {
        if (username[i] == ' ' || username[i] == '\n') return 0;
    }

    return 1;
}

int is_username_taken(const char *username, const char *user_list[]) {

    // Check if the list is empty
    if (user_list == NULL) return 0;

    // Check if there is a username match in the list
    for (size_t i = 0; user_list[i] != NULL; i++) {
        if (strcmp(username, user_list[i]) == 0) return 1;
    }

    return 0;
}