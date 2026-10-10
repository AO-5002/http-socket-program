//
// Created by Andres Ortiz Osorio on 10/9/26.
//

#include "../../common/protocol.h"
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
    const auto text = "foo";

    // I 100% need to improve this

    // Test parse_message function
    return compare_msg_protocols(parse_message("REG foo") , (MSG_PROTOCOL) {.type = REG, .message = text}) == 1 &&
        compare_msg_protocols(parse_message("ACK foo"), (MSG_PROTOCOL) { .type = ACK, .message = text }) == 1 &&
            compare_msg_protocols(parse_message("EXIT foo"), (MSG_PROTOCOL) { .type = EXIT, .message = text }) == 1 &&
                compare_msg_protocols(parse_message("MESG foo"), (MSG_PROTOCOL) { .type = MESG, .message = text }) == 1 &&
                    compare_msg_protocols(parse_message("PMSG foo"), (MSG_PROTOCOL) { .type = PMSG, .message = text }) == 1;
}