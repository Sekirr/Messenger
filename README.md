# Messenger

A C++20 TCP echo-server and command-line client for exploring Linux network programming, message framing, resource management, and concurrency.

**Project status:** Work in progress. The `master` branch contains the original blocking, thread-per-client implementation. A `poll()`-based event loop is being developed separately on `feature/poll` and is not yet complete.

## Features

- IPv4 TCP server and command-line client using POSIX sockets.
- Multiple client connections, handled by one `std::thread` per client in the current working implementation.
- Length-prefixed messages: a 4-byte unsigned length in network byte order, followed by the message payload.
- Loops to handle partial `send()` and `recv()` operations in blocking mode.
- RAII socket wrapper with move-only ownership and automatic descriptor cleanup.
- Error reporting via `std::error_code` and a custom `Result<T, E>` type for accepting connections.
- Echo behavior: the server sends each received message back to the same client.

## Requirements

- Linux or WSL2 (Ubuntu or another Linux distribution)
- A C++20 compiler (GCC or Clang)
- CMake 3.20 or newer
- Make or Ninja

## Build

```bash
git clone https://github.com/Sekirr/Messenger.git
cd Messenger
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

## Run

Start the server in one terminal:

```bash
./build/Server
```

Start the client in another terminal:

```bash
./build/Client
```

The server listens on TCP port **8080** on all IPv4 interfaces (`0.0.0.0`). The client connects to `127.0.0.1:8080`.

Enter a line of text in the client and press Enter. The server receives it and sends it back.

**Current limitation:** the client reads the echoed response but does not yet display it in the terminal.

## Wire protocol

Each message consists of:

- **Header:** 4-byte unsigned integer (`uint32_t`) in network byte order, specifying the payload length.
- **Payload:** the specified number of bytes.

The maximum payload size is **64 KiB**.

TCP is a byte stream: a single message may arrive across several `recv()` calls, and one read does not imply one complete application-level message.

## Code organization

- `Socket` — owns a socket file descriptor and closes it automatically.
- `TCPListener` — binds, listens, and accepts incoming connections.
- `TCPConnection` — connects, sends, and receives length-prefixed messages in blocking mode.
- `Result<T, E>` — represents a value or an error.
- `src/serverMain.cpp` — runs the echo server.
- `src/clientMain.cpp` — runs the command-line client.

## In progress

- Nonblocking sockets and event-driven I/O with `poll()`.
- Per-client input/output buffers and handling of incomplete messages.
- Automated tests and integration tests.
- More predictable connection shutdown and cleanup.
- Displaying echoed responses in the client.

## Scope

This is an educational systems-programming project, not a production messaging service. It does not currently provide authentication, encryption, message persistence, or delivery guarantees across reconnects.
