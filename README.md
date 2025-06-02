<div align="center">

# 🌐 ft_irc - Internet Relay Chat Server 🌐

</div>

<div align="center">

## 📝 About the Project

</div>

This project is a complete IRC (Internet Relay Chat) server implementation in C++98, designed to handle real-time communication between multiple clients. The server fully supports IRC protocol standards, authentication, channel management, private messaging, operator commands, and includes a bonus IRC bot with advanced features.

<div align="center">

## 🚀 Core Features

</div>

### 🔐 Authentication & Connection Management
- **User Authentication**: Complete PASS/NICK/USER registration flow
- **Connection Handling**: Non-blocking I/O with poll() for multiple simultaneous clients
- **CAP Negotiation**: Client capability negotiation support
- **PING/PONG**: Keep-alive mechanism to maintain connections
- **QUIT**: Graceful disconnection with custom messages

### 📡 Channel Operations
- **JOIN/PART**: Create, join, and leave channels with optional keys
- **LIST**: Browse available channels with user counts and topics
- **TOPIC**: View and set channel topics (operator-restricted when +t mode)
- **Channel Modes**: Complete mode system implementation:
  - **+i/-i**: Invite-only channel
  - **+t/-t**: Topic restriction to operators
  - **+k/-k**: Channel password protection
  - **+o/-o**: Operator privilege management
  - **+l/-l**: User limit enforcement

### 👥 User Management & Messaging
- **PRIVMSG**: Private messages between users and channel broadcasting
- **NOTICE**: Notice system (non-reply generating messages)
- **WHO/WHOIS**: User information and channel membership queries
- **Multiple Channels**: Users can join and manage multiple channels simultaneously

### 🛡️ Operator Commands
- **KICK**: Remove users from channels with optional reason
- **INVITE**: Invite users to channels (respects +i mode)
- **MODE**: Complete channel mode management system
- **Operator Privileges**: Automatic operator assignment to channel creators

### 📋 Additional Features
- **MOTD**: Message of the Day system with file-based configuration
- **Error Handling**: Complete IRC error code implementation (401, 403, 461, etc.)
- **Debug System**: Comprehensive logging with colored output
- **Configuration**: File-based configuration system

<div align="center">

## 🤖 Bonus: IRC Bot

</div>

### 🌤️ Weather Command
- **API Integration**: Real-time weather data via wttr.in
- **Multiple Formats**: 
  - Simple: `!weather porto`
  - Detailed: `!weather london detailed`
  - Full: `!weather paris full`
- **Global Support**: Weather for cities worldwide
- **Smart Parsing**: Handles multi-word city names

### 😄 Entertainment Commands
- **Dad Jokes**: `!dadjokes` - Random dad jokes from 24+ joke collection
- **Game**: `!game` - Surprise interactive feature
- **Help System**: `!help [command]` - Comprehensive help with examples

### 🔧 Bot Architecture
- **HTTP Client**: Custom HTTP client for external API calls
- **Command Factory**: Modular command system
- **Auto-Join**: Automatically joins all channels
- **Case-Insensitive**: All commands work regardless of case

<div align="center">

## 🛠️ Technologies & Architecture

</div>

### Core Technologies
- **C++98**: Full compliance with C++98 standard
- **TCP/IP Sockets**: IPv4/IPv6 support with non-blocking I/O
- **Poll System**: Single poll() call handles all I/O operations

### Design Patterns Implemented
- **Factory Pattern**: `CommandFactory` & `CommandBotFactory` for dynamic command creation
- **Command Pattern**: `ACommand` hierarchy with `execute()` method for all IRC commands
- **Observer Pattern**: Channel broadcasting system notifying all subscribed clients
- **Strategy Pattern**: Different command execution strategies via polymorphic `ACommand`
- **Template Method Pattern**: `ACommand` base class defining execution template
- **Facade Pattern**: `Server` class as unified interface to IRC subsystems
- **Adapter Pattern**: `Socket` wrapper adapting system calls to C++ interface

### Development Tools
- **Comprehensive Testing**: 
  - Connection tests
  - Channel operation tests
  - Messaging tests
  - Stress tests with multiple clients
  - Memory leak detection with Valgrind
- **Makefile**: Advanced build system with progress bars and parallel compilation
- **Debug System**: Colored logging with timestamp and severity levels

## 🏗️ Build and Execution

```bash
# Compile the main server
make

# Compile with bot (bonus)
make bonus

# Run the server
./ircserv <port> <password>

# Run the bot (connects to server)
./ircbot <port> <bot_password>

# Run comprehensive tests
make test
make test-all          # Include stress and valgrind tests
make test-valgrind     # Memory leak testing
```

## 🧪 Testing Suite

```bash
# Quick connectivity test
make test-quick

# Specific test categories
make test-connection   # Authentication and connection tests
make test-channels     # Channel operations (JOIN, PART, KICK, etc.)
make test-messaging    # PRIVMSG functionality
make test-stress       # Load testing with multiple clients

# Memory testing
make test-valgrind     # Comprehensive memory leak detection
```

## 📋 Configuration

### Server Configuration (`config.txt`)
```bash
# Bot authentication password
botpass=your_bot_password
```

### MOTD Configuration (`motd.txt`)
Custom Message of the Day displayed to connecting users.

## 🔧 Technical Specifications

### Protocol Compliance
- **RFC 1459/2812**: Full IRC protocol implementation
- **Error Codes**: Complete IRC error response system
- **Message Format**: Proper IRC message parsing and formatting
- **Channel Modes**: All standard channel modes implemented

### Performance Features
- **Non-blocking I/O**: Handles hundreds of concurrent connections
- **Memory Efficient**: Comprehensive memory management
- **Resource Cleanup**: Automatic cleanup of disconnected clients
- **Signal Handling**: Graceful shutdown on SIGINT/SIGTERM

### Security Features
- **Password Authentication**: Server and bot password protection
- **Channel Security**: Invite-only, password-protected channels
- **Operator System**: Hierarchical permission system
- **Input Validation**: Comprehensive input sanitization

## 📚 Supported IRC Commands

### Connection Commands
`PASS`, `NICK`, `USER`, `QUIT`, `PING`, `PONG`, `CAP`

### Channel Commands
`JOIN`, `PART`, `LIST`, `TOPIC`, `MODE`, `KICK`, `INVITE`

### Messaging Commands
`PRIVMSG`, `NOTICE`, `WHO`, `WHOIS`, `MOTD`

### Bot Commands
`!weather`, `!dadjokes`, `!game`, `!help`

## 🧑‍💻 Team

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/SirAlabar">
        <img src="https://github.com/SirAlabar.png" width="80" height="80" style="border-radius: 50%;" alt="hluiz"><br>
        <sub><b>hluiz</b></sub><br>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/isilva-t">
        <img src="https://github.com/isilva-t.png" width="80" height="80" style="border-radius: 50%;" alt="isilva-t"><br>
        <sub><b>isilva-t</b></sub><br>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/MrSloth-dev">
        <img src="https://github.com/MrSloth-dev.png" width="80" height="80" style="border-radius: 50%;" alt="joao-pol"><br>
        <sub><b>joao-pol</b></sub><br>
      </a>
    </td>
  </tr>
</table>

## 📈 Project Statistics

- **Lines of Code**: 10,000+ lines of C++
- **Files**: 90+ source and header files
- **Commands**: 20+ IRC commands implemented
- **Tests**: 50+ automated test scenarios
- **Features**: Complete IRC server + advanced bot

## 🎯 Usage Examples

### Basic Server Usage
```bash
# Start server on port 6667 with password "mypass"
./ircserv 6667 mypass

# Connect with any IRC client (e.g., irssi, HexChat)
/connect localhost 6667
/pass mypass
/nick mynick
/join #general
```

### Bot Integration
```bash
# Start bot (after server is running)
./ircbot 6667 bot_password

# Use bot commands in channels
!weather london
!help weather
!dadjokes
```

## 📚 References

- [RFC 1459](https://tools.ietf.org/html/rfc1459) - Internet Relay Chat Protocol
- [RFC 2812](https://tools.ietf.org/html/rfc2812) - Internet Relay Chat: Client Protocol
- [IRC Numeric Replies](https://tools.ietf.org/html/rfc2812#section-5) - Standard IRC response codes

---

**42 School Project** | *Developed with passion for networking and real-time communication* 

🔗 **Compatible with all standard IRC clients** | 🚀 **Production-ready architecture** | 🧪 **Thoroughly tested**
