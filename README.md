# cpp-https-server

A from-scratch HTTPS server in modern C++ on Linux, exploring TCP sockets, HTTP/1.1, TLS, multithreading, and systems-level performance.

The networking and HTTP layers are written by hand on top of the POSIX socket API. Cryptography will come from an established library (OpenSSL), not a custom implementation.

## Current status

**Stage 1 complete: a basic TCP server and client.**

The server:

- resolves a bind address with `getaddrinfo()` (port `8080`, wildcard address, all interfaces)
- creates a socket, binds it, and listens for connections
- accepts clients one at a time in a loop and logs each client's address
- sends each client a greeting, then closes the connection

The client:

- resolves `localhost` with `getaddrinfo()` and tries each address until `connect()` succeeds
- receives the server's greeting with `recv()` and prints it

There is no HTTP, TLS or concurrency yet. Each of these is added in a later stage (see the roadmap).

### Current limitations

These are known and will be fixed in stage 2:

- The server is iterative: it serves one client at a time.
- A single `send()` and a single `recv()` are used, with no handling of partial reads or writes.
- Some error paths exit the server or leak a file descriptor instead of skipping that one client.

## Requirements

- Linux
- A C++23 compiler (developed with Clang)
- CMake 3.20 or newer

## Build

```sh
cmake -S . -B build
cmake --build build
```

This produces two programs: `build/server` and `build/client`.

## Run

Start the server in one terminal:

```sh
./build/server
```

Then connect from a second terminal, with the included client:

```sh
./build/client
```

or with any TCP client:

```sh
nc localhost 8080
telnet localhost 8080
```

Each client prints the server's greeting. The server logs the address of each client that connects.

## Project structure

```
cpp-https-server/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── server.cpp    # TCP stream server
│   └── client.cpp    # TCP stream client
├── include/          # headers (as the code grows)
├── tests/            # automated tests (planned)
└── docs/             # notes and design documentation
```

## Roadmap

- [ ] **0. Tooling**: sanitizers in a Debug build, `strace` and `tcpdump` for debugging, a smoke-test script
- [x] **1. Basic TCP server and client**: `socket`, `bind`, `listen`, `accept`, `connect`, `send`/`recv`, `close`
  - [x] Minimal stream server
  - [x] Minimal stream client
- [ ] **2. Robust I/O**: `send_all` and `recv` loops, an echo server, `SIGPIPE`, per-client error handling
- [ ] **3. C++ socket abstraction**: RAII ownership of file descriptors, an error model based on `std::expected`
- [ ] **4. HTTP/1.1 subset**: a pure, unit-tested and fuzzed request parser with size limits; valid responses
- [ ] **5. Static file serving**: path-traversal protection and timeouts
- [ ] **6. Baseline benchmark**: single-threaded throughput and latency
- [ ] **7. Concurrency**: a thread pool, benchmarked; optionally an `epoll` event loop, benchmarked against it
- [ ] **8. TLS/HTTPS**: OpenSSL, built on the chosen concurrency model
- [ ] **9. Hardening and final results**: CI, security checks, final benchmarks compared with nginx, write-up

Stretch goals: `io_uring`, a reverse proxy, WebSockets.

## References

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)
- [Linux man pages](https://man7.org/linux/man-pages/): `socket(2)`, `bind(2)`, `listen(2)`, `accept(2)`, `connect(2)`, `send(2)`, `recv(2)`, `getaddrinfo(3)`, `tcp(7)`
- Michael Kerrisk, *The Linux Programming Interface*, chapters 56–61 (sockets) and 63 (I/O models)
- [RFC 9110](https://www.rfc-editor.org/rfc/rfc9110) (HTTP semantics) and [RFC 9112](https://www.rfc-editor.org/rfc/rfc9112) (HTTP/1.1)
