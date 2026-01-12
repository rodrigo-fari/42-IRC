![ft_irc Banner](./assets/IRC-ProjectBanner.png)

## Introduction
Internet Relay Chat (IRC) is a text-based communication protocol that enables real-time messaging over the Internet. It supports both public and private communication through channels and direct messages. IRC clients connect to an IRC server, which may be part of a larger IRC network.

## Project Overview
The **ft_irc** project consists of developing an IRC server in **C++98**, following the specifications defined by the IRC protocol. The server must be capable of handling multiple clients simultaneously using non-blocking I/O and a single I/O multiplexing mechanism.

This project emphasizes low-level network programming, event-driven design, and strict compliance with the C++98 standard.

## Technical Focus Areas
- **Network Programming:** TCP/IP server implementation (IPv4 or IPv6).
- **I/O Multiplexing:** Use of a single `poll()` call (or equivalent such as `select`, `kqueue`, or `epoll`) to manage all client connections.
- **Non-blocking I/O:** All sockets must operate in non-blocking mode. The use of `fork()` is strictly prohibited.
- **Data Integrity:** Incoming data must be buffered and reassembled to ensure complete IRC commands before processing.
- **C++98 Compliance:** The project must compile with `-Wall -Wextra -Werror` and adhere strictly to the C++98 standard.

## Project Structure
```text
.
├── Makefile              # Build automation (all, clean, fclean, re)
├── inc/                  # Header files (.hpp, .tpp, .ipp)
│   ├── commands/         # IRC command headers
│   ├── core/             # Core IRC logic headers
│   └── network/          # Networking and socket headers
├── src/                  # Source files (.cpp)
│   ├── main.cpp          # Entry point and argument parsing
│   ├── commands/         # IRC command implementations (KICK, INVITE, etc.)
│   ├── core/             # Client and channel management
│   └── network/          # Socket handling and poll() loop
└── obj/                  # Compiled object files (not tracked)
```
## Mandatory Features

### Authentication
- A connection password is required for all incoming clients.

### Client Registration
- Clients must be able to set a **nickname** and a **username**.

### Channel Operations
- **KICK** – Remove a client from a channel.
- **INVITE** – Invite a client to join a channel.
- **TOPIC** – View or change the channel topic.
- **MODE** – Manage channel modes:
  - `i` – Invite-only channel
  - `t` – Topic change restricted to operators
  - `k` – Channel password (key)
  - `o` – Operator privilege
  - `l` – User limit

### Communication
- Broadcast messages to all clients in a channel.
- Support private messages between clients.

## Usage
Compile the project and run the server as follows:

```bash
./ircserv <port> <password>
```
### Arguments
- **port** – Port number used for incoming IRC connections.
- **password** – Password required for client authentication.

## Notes
- The server must never block or hang when handling multiple clients.
- Only one polling mechanism is allowed.
- The implementation must remain compatible with standard IRC clients (e.g., irssi, WeeChat).

## Author

![Rodrigo Faria.](https://github.com/rodrigo-fari)  |  ![Reinaldo Rodrigues.](https://github.com/reinatch)  |  ![Christofer Mota.](https://github.com/cde-paiv)
