# Chat Client and Server

CS/CE 4390 Programming Assignment: a TCP chat server and client in C.

## Where to run

Build and run on the CS lab machines `csa-net01.utdallas.edu` to `csa-net45.utdallas.edu`.

## Compile
Unzip the submission (or clone https://github.com/AO-5002/http-socket-program), then from the project root:

```
gcc -o chat_server server/*.c common/protocol.c
gcc -o chat_client client/*.c common/protocol.c
```

### Run

1. Start the server (listens on port 8080):

   ```
   ./chat_server
   ```

2. In another terminal, start a client:

   ```
   ./chat_client
   ```

   When prompted, enter:
   - Server hostname: `localhost` if on the same machine as the server, otherwise the server's hostname (e.g. `csa-net01.utdallas.edu`)
   - Port: `8080`
   - Username: 1-32 characters, no spaces

3. Start more clients the same way to chat between users.

## Client commands

Commands are case-sensitive.

| Command | What it does |
|---|---|
| `MESG <message>` | Send a message to everyone in the chatroom |
| `PMSG <user> <message>` | Send a private message to `<user>` |
| `LIST` | Show the users currently in the chatroom |
| `EXIT` | Leave the chatroom and quit |

Messages from the server are shown as received (`ACK`, `MSG`, `ERR`), and errors also print a short explanation.

## Notes

- Error codes follow the spec's table: `0` username taken, `1` invalid username, `2` unknown user for private message, `3` invalid or unexpected message.
- `LIST` is handled by the client: it keeps the user list from the `ACK` received at registration and the `SERVER` join/leave notifications.
- To use a different port, change `SERVER_PORT` in `transport_info.h` and recompile.
