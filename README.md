# cpp-https-server

A from-scratch HTTPS server in modern C++ on Linux, exploring TCP sockets, HTTP/1.1, TLS, multithreading, and systems-level performance.

The networking and HTTP layers are written by hand on top of the POSIX socket API. Cryptography will come from an established library (OpenSSL), not a custom implementation.

## Current status

**Stage 2 in progress: robust I/O.** The server is now an echo server.

The server:

- resolves a bind address with `getaddrinfo()` (port `8080`, wildcard address, all interfaces)
- creates a socket, sets `SO_REUSEADDR` so it can restart immediately, binds it, and listens
- accepts clients one at a time and logs each client's address
- reads from the client in a loop and sends every byte back until the client disconnects

The client (from stage 1):

- resolves `localhost` with `getaddrinfo()` and tries each address until `connect()` succeeds
- waits for data with `recv()` and prints it

The client does not send anything yet, so use `nc` to test the echo server. Updating the client to send a message and read the echo is part of stage 2.

There is no HTTP, TLS or concurrency yet. Each of these is added in a later stage (see the roadmap).

### Current limitations

- The server is iterative: it serves one client at a time. A second client can connect, but gets no echo until the first one disconnects.
- Partial sends and some error paths are not yet handled correctly (in progress, see stage 2 below).

## Requirements

- Linux
- A C++23 compiler (developed with Clang)
- CMake 3.20 or newer
- `nc` (OpenBSD netcat) for testing: `sudo pacman -S openbsd-netcat` on Arch-based systems

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

Then connect from a second terminal and type a few lines. Each line comes back:

```sh
nc localhost 8080
```

Quit with `Ctrl+C`. The server logs the client's address when it connects and a message when it disconnects.

For a one-shot test that exits on its own:

```sh
echo "hello" | nc -N localhost 8080
```

Stop the server with `Ctrl+C`. (`Ctrl+Z` only suspends it, and a suspended server keeps port 8080 busy.)

## Project structure

```
cpp-https-server/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── server.cpp    # TCP echo server
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
- [ ] **2. Robust I/O**
  - [x] Echo server: `recv` loop that handles data, disconnect (`0`) and errors (`-1`)
  - [x] `SO_REUSEADDR` for immediate restarts
  - [ ] `send_all()` that handles partial sends and errors
  - [ ] `SIGPIPE` handling (`MSG_NOSIGNAL`)
  - [ ] Per-client errors skip the client instead of stopping the server; error messages include the reason (`strerror`)
  - [ ] Client sends a message and reads the full echo
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
