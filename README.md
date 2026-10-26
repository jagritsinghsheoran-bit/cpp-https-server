# cpp-https-server

A from-scratch HTTPS server in modern C++ on Linux, exploring TCP sockets, HTTP/1.1, TLS, multithreading, and systems-level performance.

The networking and HTTP layers are written by hand on top of the POSIX socket API. Cryptography will come from an established library (OpenSSL), not a custom implementation.

## Current status

**Stage 1: basic TCP server (in progress)**

The server currently:

- resolves a bind address with `getaddrinfo()` (IPv4, port `8080`, all interfaces)
- creates a socket, binds it, and listens for connections
- accepts clients one at a time in a loop
- sends each client a greeting message, then closes the connection

There is no HTTP, TLS or concurrency yet. Each of these is added in a later stage (see the roadmap).

## Requirements

- Linux
- A C++23 compiler (developed with Clang)
- CMake 3.20 or newer

## Build and run

```sh
cmake -S . -B build
cmake --build build
./build/server
```

## Test it

With the server running, connect from a second terminal:

```sh
nc localhost 8080
# or
telnet localhost 8080
```

You should see the server's greeting. The server terminal logs the client's address.

## Project structure

```
cpp-https-server/
├── CMakeLists.txt
├── README.md
├── src/        # server source
├── include/    # headers (as the code grows)
├── tests/      # automated tests (planned)
└── docs/       # notes and design documentation
```

## Roadmap

- [ ] **1. Basic TCP server and client**: `socket`, `bind`, `listen`, `accept`, `connect`, `send`/`recv`, `close`
  - [x] Minimal stream server
  - [ ] Minimal stream client
- [ ] **2. TCP fundamentals**: socket lifecycle, address resolution, byte order, stream semantics, partial reads/writes
- [ ] **3. C++ socket abstraction**: RAII ownership of file descriptors
- [ ] **4. HTTP/1.1**: request parsing and valid responses
- [ ] **5. Static file serving**: with path-traversal protection
- [ ] **6. TLS/HTTPS**: OpenSSL integration
- [ ] **7. Concurrency**: thread pool or another justified model
- [ ] **8. Robustness**: malformed requests, disconnects, timeouts, resource limits
- [ ] **9. Testing and safety**: automated tests, sanitizers, security checks
- [ ] **10. Benchmarks**: throughput, latency and concurrent connections, with a documented methodology

## References

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)
- Linux man pages: `socket(2)`, `bind(2)`, `listen(2)`, `accept(2)`, `getaddrinfo(3)`
