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

- `C++98`
- TCP/IP Sockets
- `poll()` (for non-blocking I/O operations)
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

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/SirAlabar">
        <img src="https://github.com/SirAlabar.png" width="70" height="70" style="border-radius: 50%;" alt="SirAlabar"><br>
        <sub><b>SirAlabar</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/MrSloth-dev">
        <img src="https://github.com/MrSloth-dev.png" width="70" height="70" style="border-radius: 50%;" alt="MrSloth-dev"><br>
        <sub><b>MrSloth-dev</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/isilva-t">
        <img src="https://github.com/isilva-t.png" width="70" height="70" style="border-radius: 50%;" alt="isilva-t"><br>
        <sub><b>isilva-t</b></sub>
      </a>
    </td>
  </tr>
</table>

## 📚 References

- [RFC 1459](https://tools.ietf.org/html/rfc1459) - Internet Relay Chat Protocol
- [RFC 2812](https://tools.ietf.org/html/rfc2812) - Internet Relay Chat: Client Protocol

## 📊 Project Progress

- [ ] Initial project setup
- [ ] Basic server implementation
- [ ] Client connection management
- [ ] User authentication
- [ ] Channel implementation
- [ ] Basic IRC commands
- [ ] Operator commands
- [ ] Testing and debugging

---

**This project was developed as part of the 42 curriculum.**

💻 Happy coding! 💻
