//
// Created by Andres Ortiz Osorio on 10/8/26.
//

#include "../../server/server_validation.h"
#include <assert.h>
#include <stdio.h>

int test_validate_username();
void test_case(int test);
int test_is_username_taken();

void server_validation_unit_test() {
    printf("Server Validation Unit Test\n");
    test_case(test_validate_username());
    test_case(test_is_username_taken());
    printf("\n");
}

int test_validate_username() {
    
    // Test validate_username
    
    return (validate_username("Bob") == 1)
    && (validate_username("Bob ") == 0)
    && (validate_username("Bob\n") == 0);
}

int test_is_username_taken() {
    
    // Test username taken
    const char *empty_list[] = { NULL };
    const char *user_list[5] = { "Bob", "Rod", "Joe", "Jane", "Jill"};
    
    return (is_username_taken("Bob", empty_list) == 0)
    && (is_username_taken("Bob", user_list) == 1)
    && (is_username_taken("Bob2", user_list) == 0);
}