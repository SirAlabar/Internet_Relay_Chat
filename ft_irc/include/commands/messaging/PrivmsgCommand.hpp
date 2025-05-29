#ifndef PRIVMSG_COMMAND_HPP
#define PRIVMSG_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class PrivmsgCommand : public ACommand
{
private:
    // Private to prevent copying
    PrivmsgCommand(const PrivmsgCommand& other);
    PrivmsgCommand& operator=(const PrivmsgCommand& other);

    void handleChannelMessage(Client* sender, const std::string& channelName,
                              const std::string& message);
    void handlePrivateMessage(Client* sender, const std::string& targetNick,
                              const std::string& message);
    std::string createMessage(Client* sender, const std::string& command,
                              const std::string& target, const std::string& message);

    bool isDccMessage(const std::string& message);
    void sendDccNotify(Client* sender, const std::string& target,
                       const std::string& message);

public:
    PrivmsgCommand(Server* server);
    virtual ~PrivmsgCommand();

    // Execute the PRIVMSG command
    virtual void execute(Client* client, const Message& message);

    // Static creator for factory
    static ACommand* create(Server* server);
};

#endif
