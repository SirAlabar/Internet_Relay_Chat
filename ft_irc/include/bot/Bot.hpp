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
    std::set<std::string> _joinedChannels;

public:
    Bot(std::string host, int port, std::string pass);
    ~Bot();

    bool connect();
    void disconnect();
    bool isConnected() const;

    void run();

    void processMessage(const std::string& rawMessage);
    void sendMessage(const std::string& message);

    bool authenticate();
    void joinChannel(const std::string& channel);
    void partChannel(const std::string& channel);
    void handleBotCommand(const std::string& channel, const std::string& user,
                          const std::string& command);
};
#endif  // !BOT
