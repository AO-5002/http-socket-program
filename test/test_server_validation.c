//
// Created by Andres Ortiz Osorio on 10/8/26.
//

#include "../server/server_validation.h"
#include <assert.h>
#include <stdio.h>

int main() {

    // Test validate_username
    assert(validate_username("Bob") == 1);
    assert(validate_username("Bob ") == 0);
    assert(validate_username("Bob\n") == 0);

    // Test username taken
    const char *user_list[5] = { "Bob", "Rod", "Joe", "Jane", "Jill"};
    assert(is_username_taken("Bob", NULL) == 0);
    assert(is_username_taken("Bob", user_list) == 1);
    assert(is_username_taken("Bob2", user_list) == 0);
}
