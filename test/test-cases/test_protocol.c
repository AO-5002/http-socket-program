//
// Created by Andres Ortiz Osorio on 10/9/26.
//

#include "../../common/protocol.h"
#include <stdio.h>


int compare_msg_protocols(MSG_PROTOCOL a, MSG_PROTOCOL b);
int test_parsing();
int test_return_user_fd();
void test_case(int test);

void protocol_unit_test() {
    printf("Protocol Unit Test\n");
    test_case(test_parsing());
    test_case(test_return_user_fd());
    printf("\n");
}

// Test-Cases

int test_return_user_fd() {
    char *list[5] = { "Bob", "Rod", "Joe", "Jane", "Jill" };
    return return_user_fd("Rod", list) == 1 &&
        return_user_fd("Dude", list) == -1;
}

int test_parsing() {
    const auto text = "foo";

    // I 100% need to improve this

    // Test parse_message function
    return compare_msg_protocols(parse_message("REG foo") , (MSG_PROTOCOL) {.type = REG, .message = text}) == 1 &&
        compare_msg_protocols(parse_message("ACK foo"), (MSG_PROTOCOL) { .type = ACK, .message = text }) == 1 &&
            compare_msg_protocols(parse_message("EXIT foo"), (MSG_PROTOCOL) { .type = EXIT, .message = text }) == 1 &&
                compare_msg_protocols(parse_message("MESG foo"), (MSG_PROTOCOL) { .type = MESG, .message = text }) == 1 &&
                    compare_msg_protocols(parse_message("PMSG foo"), (MSG_PROTOCOL) { .type = PMSG, .message = text }) == 1;
}