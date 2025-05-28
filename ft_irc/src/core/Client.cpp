#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "Client.hpp"

Client::Client(int fd) : _fd(fd), _authenticated(false), _isBot(false) {}

Client::~Client() {}

int Client::getFd() const { return _fd; }

std::string Client::getFdString() const
{
    std::stringstream ss;
    ss << _fd;
    return ss.str();
}

const std::string& Client::getNickname() const { return _nickname; }

void Client::setNickname(const std::string& nickname) { _nickname = nickname; }

const std::string& Client::getUsername() const { return _username; }

void Client::setUsername(const std::string& username) { _username = username; }

bool Client::isAuthenticated() const { return _authenticated; }

void Client::setAuthenticated(bool auth) { _authenticated = auth; }

bool Client::isBot() { return _isBot; }
void Client::setBot(bool status) { _isBot = status; }
bool Client::sendMessage(const std::string& message)
{
    Print::Debug("Attempting to send to client FD: " + getFdString());
    Print::Debug(message);

    ssize_t sentBytes = send(_fd, message.c_str(), message.length(), MSG_NOSIGNAL);

    if (sentBytes < 0)
    {
        Print::StdErr("sending message: " + toString(strerror(errno)) +
                      " (errno: " + toString(errno));
        return false;
    }
    else if (sentBytes < (ssize_t)message.length())
    {
        Print::Log("WARNING: Partial send - only " + toString(sentBytes) + " of " +
                   toString(message.length()) + " bytes sent");
        return false;
    }
    else
    {
        Print::Debug("Successfully sent " + toString(sentBytes) + " bytes");
        return true;
    }
}
