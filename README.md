Of course! Based on the files you shared (`Client.c`, `Client_1.c`, `Server.c`, `Server_1.c`), here’s a **README.md** you can put in your Git repository:

---

# Socket Programming Project

This project contains basic and advanced client-server communication programs in C using TCP sockets.

## Overview

There are two sets of clients and servers:

* **Basic Client-Server**

  * `Client.c`
  * `Server.c`
* **Advanced Client-Server with Multiple Clients**

  * `Client_1.c`
  * `Server_1.c`

The basic client-server setup allows a single client to connect to a server, send integers, and receive information about whether the number is odd, even, or invalid.

The advanced setup allows multiple clients to connect to the server, list active clients, send direct messages to other clients by name, and handle more complex communication patterns.

---

## Files

| File         | Description                                                                                                                             |
| :----------- | :-------------------------------------------------------------------------------------------------------------------------------------- |
| `Client.c`   | Basic TCP client that sends messages to a server and receives responses.                                                                |
| `Server.c`   | Basic TCP server that receives messages and replies based on whether an integer is even or odd.                                         |
| `Client_1.c` | Advanced TCP client that supports non-blocking input and communication with other clients via the server.                               |
| `Server_1.c` | Advanced TCP server that manages multiple clients, supports listing connected clients, handling messaging requests, and error handling. |

---

## How to Compile

```bash
# Basic server and client
gcc Server.c -o server
gcc Client.c -o client

# Advanced server and client
gcc Server_1.c -o server_1
gcc Client_1.c -o client_1
```

---

## How to Run

### Basic Setup

First, start the server:

```bash
./server <port_number>
```

Then, start the client:

```bash
./client <hostname> <port_number>
```

Example:

```bash
./server 8080
./client localhost 8080
```

### Advanced Setup (Multiple Clients)

Start the advanced server:

```bash
./server_1 <port_number>
```

Start each client with a unique name:

```bash
./client_1 <client_name> <port_number>
```

Example:

```bash
./server_1 8081
./client_1 Alice 8081
./client_1 Bob 8081
```

In the client, you can:

* Send `LIST` to see all connected clients.
* Send `REQ: <client_name>` to request a connection to another client.
* Chat directly with another client after a request.

---

## Features

* **Basic server-client:**

  * Send and receive simple integer messages.
  * Server identifies if the message is a valid integer and if it’s even or odd.
* **Advanced server-client:**

  * Handle multiple clients.
  * Support commands like `LIST` and `REQ`.
  * Enable direct client-to-client messaging via the server.
  * Basic error handling for incomplete messages.
