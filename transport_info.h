//
// Created by Andres Ortiz Osorio on 10/6/26.
//

#ifndef COMMON_H
#define COMMON_H

// Validation MACROS
#define MIN_USERNAME_LENGTH 1
#define MAX_USERNAME_LENGTH 32
#define MAX_PORT_LENGTH 4
#define MIN_MESSAGE_LENGTH 1
#define MAX_MESSAGE_LENGTH 512
#define MAX_USER_LIST_LENGTH 10
#define MAX_PROTOCOL_LENGTH 1024

// Error MACROS (codes from the spec's protocol table)
#define USERNAME_TAKEN 0
#define INVALID_USERNAME 1
#define UNKNOWN_USER_PMSG 2
#define INVALID_MESSAGE 3

// Transport Layer MACROS
#define DOMAIN_PROTOCOL AF_INET
#define MAX_USER_CONNECTIONS 10
#define SERVER_PORT 8080
#define CLIENT_PORT 3000
#define BACKLOG 10

#endif //COMMON_H