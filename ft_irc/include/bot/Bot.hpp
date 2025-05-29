#ifndef BOT_HPP
#define BOT_HPP
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Socket.hpp"

class Bot
{
private:
    Socket _socket;
    std::string _nickname;
    std::string _serverHost;
    int _serverPort;
    std::string _password;
    bool _connected;
    bool _authenticated;
    std::string _messageBuffer;

public:
    Bot(std::string host, int port, std::string pass);
    ~Bot();

    bool connect();
    void disconnect();
    bool isConnected() const;

    void run();
    void sendMessage(const std::string& message);

    bool authenticate();
};
#endif  // !BOT
