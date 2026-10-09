//
// Created by Andres Ortiz Osorio on 10/8/26.
//

#ifndef HTTP_SOCKET_PROGRAM_SERVER_VALIDATION_H
#define HTTP_SOCKET_PROGRAM_SERVER_VALIDATION_H

// Username Validation

// Returns 0 for success, and -1 for invalid username.
int validate_username(const char *username);
int is_username_taken(const char *username, const char *user_list[]);
int contains_invalid_characters(char *username);


#endif //HTTP_SOCKET_PROGRAM_SERVER_VALIDATION_H