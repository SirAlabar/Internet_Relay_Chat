#include <iostream>

#include "Client.hpp"
#include "DadJokesCommand.hpp"
#include "Message.hpp"
#include "Server.hpp"

DadJokesCommand::DadJokesCommand(Server* server) : ACommand(server) {}

DadJokesCommand::~DadJokesCommand() {}

// private, not used
DadJokesCommand::DadJokesCommand(const DadJokesCommand& other) : ACommand(other._server)
{
}

DadJokesCommand& DadJokesCommand::operator=(const DadJokesCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Static create method for factory
ACommand* DadJokesCommand::create(Server* server)
{
    return (new DadJokesCommand(server));
}

// Execute the DadJokes command
void DadJokesCommand::execute(Client* client, const Message& message)
{
    Print::Do("executing DadJokes command");
}
