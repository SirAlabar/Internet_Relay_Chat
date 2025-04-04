# 🌐 ft_irc - Internet Relay Chat Server 🌐

## 📝 About the Project

This project involves implementing an IRC (Internet Relay Chat) server in C++98, enabling real-time communication between multiple clients. The server supports authentication, channel creation, private messaging, and operator commands.

## 🚀 Features

- ✅ User authentication (nickname, username)
- ✅ Channel creation and management
- ✅ Private messaging between users
- ✅ Operator commands:
  - 👢 KICK - Eject a client from the channel
  - 📨 INVITE - Invite a client to a channel
  - 📌 TOPIC - Change or view the channel topic
  - 🔧 MODE - Change the channel's mode:
    - i: Invite-only channel
    - t: Restrict TOPIC to operators
    - k: Channel password
    - o: Operator privilege
    - l: User limit to channel

## 🛠️ Technologies Used

- C++98
- TCP/IP Sockets
- Poll (for non-blocking I/O operations)
- Makefile

## 🏗️ Build and Execution

```bash
# Compile the project
make

# Run the server
./ircserv <port> <password>
```

## 📋 Requirements

- Multiple simultaneous clients
- All non-blocking I/O operations
- Communication via TCP/IP (v4 or v6)
- Compatibility with standard IRC clients

## 👨‍💻 Team

| Member | GitHub |
|--------|--------|
| Alabar | [SirAlabar](https://github.com/SirAlabar) |
| MrSloth | [MrSloth-dev](https://github.com/MrSloth-dev) |
| isilva-t | [isilva-t](https://github.com/isilva-t) |

## 📚 References

- [RFC 1459](https://tools.ietf.org/html/rfc1459) - Internet Relay Chat Protocol
- [RFC 2812](https://tools.ietf.org/html/rfc2812) - Internet Relay Chat: Client Protocol

## 📊 Project Progress

- [x] Initial project setup
- [x] Basic server implementation
- [x] Client connection management
- [x] User authentication
- [x] Channel implementation
- [x] Basic IRC commands
- [x] Operator commands
- [x] Testing and debugging

---

*This project was developed as part of the 42 curriculum.*

💻 Happy coding! 💻
