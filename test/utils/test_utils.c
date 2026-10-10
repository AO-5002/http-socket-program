//
// Created by Andres Ortiz Osorio on 10/9/26.
//

#include <stdio.h>
#include <string.h>
#include "../../common/protocol.h"

// Helper methods

int compare_msg_protocols(MSG_PROTOCOL a, MSG_PROTOCOL b) {
    return a.type == b.type && strcmp(a.message, b.message) == 0;
}

void test_case(int test) {
    if (test == 1) printf("Test passed!\n");
    else printf("Test failed!\n");
}