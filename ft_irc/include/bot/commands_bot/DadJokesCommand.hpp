#ifndef DADJOKESCOMMAND_HPP
#define DADJOKESCOMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class DadJokesCommand : public ACommand
{
private:
    // Private to prevent copying
    DadJokesCommand(const DadJokesCommand& other);
    DadJokesCommand& operator=(const DadJokesCommand& other);

public:
    DadJokesCommand(Server* server);
    virtual ~DadJokesCommand();

    // Execute the NICK command
    virtual void execute(Client* client, const Message& message);

    // Static creator for factory
    static ACommand* create(Server* server);
};

#endif
