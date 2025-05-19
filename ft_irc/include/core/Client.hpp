#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

#include "UtilsFun.hpp"

class Print;
class Client
{
private:
    int _fd;
    std::string _nickname;
    std::string _username;
    bool _authenticated;

public:
    Client(int fd);
    ~Client();

    int getFd() const;
    std::string getFdString() const;

    const std::string& getNickname() const;
    void setNickname(const std::string& nickname);
    const std::string& getUsername() const;
    void setUsername(const std::string& username);
    bool isAuthenticated() const;
    void setAuthenticated(bool auth);

    bool sendMessage(const std::string& message);
};

#endif
