cmake_minimum_required(VERSION 4.0)
project(http_socket_program C)

set(CMAKE_C_STANDARD 23)

add_executable(client
        client/client.c
        client/client_setup.c
        client/client_setup.h
        common/protocol.c
        common/protocol.h
)

add_executable(server
        server/server.c
        server/server_setup.c
        server/server_setup.h
        server/server_validation.c
        server/server_validation.h
        common/protocol.c
        common/protocol.h
)

add_executable(test test/test_server_validation.c server/server_validation.c
        common/protocol.c
        common/protocol.h
        test/test_protocol.c
        test/test_main.c
        test/test_utils.c
)