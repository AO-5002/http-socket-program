//
// Created by Andres Ortiz Osorio on 10/9/26.
//

#include "../common/protocol.h"
#include <stdio.h>


int compare_msg_protocols(MSG_PROTOCOL a, MSG_PROTOCOL b);
int test_parsing();
void test_case(int test);

void protocol_unit_test() {
    printf("Protocol Unit Test\n");
    test_case(test_parsing());
    printf("\n");
}

// Test-Cases

int test_parsing() {
    const char *sample_msg = "REG Bob";
    const MSG_PROTOCOL successful_msg = { .type = REG, .message = "Bob" };
    
    // Test parse_message function
    return compare_msg_protocols(parse_message(sample_msg) ,successful_msg) == 1;
}