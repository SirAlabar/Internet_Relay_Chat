#ifndef JOIN_COMMAND_HPP
#define JOIN_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class JoinCommand : public ACommand
{
private:
    // Private to prevent copying
    JoinCommand(const JoinCommand& other);
    JoinCommand& operator=(const JoinCommand& other);

public:
    JoinCommand(Server* server);
    virtual ~JoinCommand();

    // Execute the JOIN command
    virtual void execute(Client* client, const Message& message);

    // Static creator for factory
    static ACommand* create(Server* server);
};

#endif
