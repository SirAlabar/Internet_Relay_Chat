#ifndef NICK_COMMAND_HPP
#define NICK_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class NickCommand : public ACommand
{
private:
    // Private to prevent copying
    NickCommand(const NickCommand& other);
    NickCommand& operator=(const NickCommand& other);

public:
    NickCommand(Server* server);
    virtual ~NickCommand();

    // Execute the NICK command
    virtual void execute(Client* client, const Message& message);

    // Static creator for factory
    static ACommand* create(Server* server);
};

#endif
