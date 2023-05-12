# Client-Server-Terminal-Chat

A client-server chat application written in C.

## Setup

In the client application, change the defined `SERVER_IP` from `"XXX.XXX.XXX.XXX"` to your machine's IP ADDRESS.

## Run the application

1. Open two terminals at the directory of the two C files.
2. Use the gcc compiler to compile both the `Client.c` and `Server.c` files as follows:

```bash
gcc -o Client Client.c
gcc -o Server Server.c
```

3. Run the compiled files one on each terminal starting with the server then the client to allow for back and forth communication.

Run the Server with:
```bash
./Server

```

Run the Client with:
```bash
./Client

```

