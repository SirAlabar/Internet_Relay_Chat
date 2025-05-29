#include <iostream>

#include "BotContext.hpp"
#include "Client.hpp"
#include "DadJokesCommand.hpp"
#include "Message.hpp"
#include "Server.hpp"

DadJokesCommand::DadJokesCommand(Server* server) : ABotCommand(server) {}

DadJokesCommand::~DadJokesCommand() {}

// private, not used
DadJokesCommand::DadJokesCommand(const DadJokesCommand& other)
    : ABotCommand(other._server)
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
ABotCommand* DadJokesCommand::create(Server* server)
{
    return (new DadJokesCommand(server));
}

// Execute the DadJokes command
void DadJokesCommand::execute(BotContext* botctx, std::string& message)
{
    Print::Do("executing DadJokes command");
}
