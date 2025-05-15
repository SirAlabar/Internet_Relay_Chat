#include "Client.hpp"
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>


Client::Client(int fd) : _fd(fd), _authenticated(false)
{
}

Client::~Client()
{
}

int Client::getFd() const
{
    return _fd;
}

const std::string& Client::getNickname() const
{
    return _nickname;
}

void Client::setNickname(const std::string& nickname)
{
    _nickname = nickname;
}

const std::string& Client::getUsername() const
{
    return _username;
}

void Client::setUsername(const std::string& username)
{
    _username = username;
}

bool Client::isAuthenticated() const
{
    return _authenticated;
}

void Client::setAuthenticated(bool auth)
{
    _authenticated = auth;
}

void Client::sendMessage(const std::string& message)
{
    // Simple implementation that sends a message to the client
    if (send(_fd, message.c_str(), message.length(), 0) < 0)
    {
        std::cerr << "Error sending message to client: " << strerror(errno) << std::endl;
    }
}