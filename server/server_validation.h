//
// Created by Andres Ortiz Osorio on 10/8/26.
//

#ifndef HTTP_SOCKET_PROGRAM_SERVER_VALIDATION_H
#define HTTP_SOCKET_PROGRAM_SERVER_VALIDATION_H
#include "../common/protocol.h"


// Username Validation + helper functions
int validate_username(const char *username);
int is_username_taken(const char *username, CLIENT user_list[]);
int register_new_user(const char *username, CLIENT user_list[]);
int contains_invalid_characters(char *username);

#endif