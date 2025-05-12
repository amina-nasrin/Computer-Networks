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



This is a simple client-server chat application with basic client management, message parsing, and handling of specific client requests. The client-server communication relies on TCP sockets, where the server can handle multiple clients concurrently through the use of `select()`.


### **Client (Client.c and Client\_1.c)**

The client application communicates with a server over a TCP/IP socket, sending and receiving messages. It has the following components:

1. **Socket Initialization:**

   * The client opens a socket using the `socket()` system call and specifies the socket type as `SOCK_STREAM` (TCP).
   * It then connects to the server using the `connect()` system call, which uses the server’s IP address and port number.

2. **Message Exchange:**

   * The client sends user input messages to the server through the socket, using the `write()` system call.
   * It then waits for the server's response, which is received via the `read()` system call.
   * The client prints the server's response to the console.

3. **Handling Input and Output:**

   * In `Client_1.c`, the client utilizes `select()` to handle multiple I/O operations non-blockingly, allowing it to read messages from both the server and the user’s input simultaneously.

4. **Exit Condition:**

   * In `Client.c`, the client will exit the loop and terminate after receiving a "Bye" message from the server.

### **Server (Server.c and Server\_1.c)**

The server waits for incoming client connections and processes messages. It consists of the following key elements:

1. **Socket Initialization:**

   * The server sets up a socket using the `socket()` system call and binds it to a specific port with `bind()`.
   * It listens for incoming client connections using `listen()` and accepts them with `accept()`.

2. **Client Handling:**

   * Once a client connects, the server reads messages sent from the client and sends back a response based on the content of the message.
   * It checks if the message is a valid integer and responds accordingly:

     * If the message is not a number, it sends back "You didn't send me an integer value!".
     * If the message is an integer, it checks if it is even or odd and responds with "You sent me an even integer" or "You sent me an odd integer."

3. **Select for I/O Multiplexing:**

   * The server uses `select()` to handle multiple clients concurrently, enabling it to wait for incoming data from multiple clients without blocking on a single client.

4. **Client Management:**

   * `Server_1.c` maintains a list of connected clients, including their names and socket descriptors, to manage interactions.
   * It supports client-specific commands like "LIST" to list all clients and "Req" to initiate communication between clients.
   * The server can send data to a specific client by referencing their file descriptor in the client list.

5. **Client Disconnection:**

   * If a client disconnects, the server cleans up and removes the client from its list of connected clients.

### **Communication Flow:**

* **Client to Server:**

  * The client sends messages to the server, such as commands or data (e.g., numbers to check if they are even or odd).
* **Server to Client:**

  * The server responds based on the input, sending back whether the number is odd, even, or an error message for non-integer inputs.

### **Key Functions:**

* **`server_delete_client()`:** Removes a client from the server's client list when they disconnect.
* **`dataFromCli()`:** Reads data from a client and processes it.
* **`dataToCli()`:** Sends data to a client.
* **`ProcessData()`:** Handles specific commands from clients (e.g., LIST, Req) and routes messages accordingly.
* **`cleanup()`:** Closes all open sockets when the server shuts down.

### **Server Select Loop (server\_select.c):**

* The server uses `select()` for multiplexing, waiting for events from multiple file descriptors (including incoming data from clients and standard input for server commands). It handles events asynchronously, ensuring that the server remains responsive while managing multiple client connections.



