#ifndef HELPCOMMAND_HPP
#define HELPCOMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class HelpCommand : public ACommand
{
private:
    // Private to prevent copying
    HelpCommand(const HelpCommand& other);
    HelpCommand& operator=(const HelpCommand& other);

public:
    HelpCommand(Server* server);
    virtual ~HelpCommand();

    // Execute the NICK command
    virtual void execute(Client* client, const Message& message);

    // Static creator for factory
    static ACommand* create(Server* server);
};

#endif
