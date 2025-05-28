#include <iostream>

#include "Client.hpp"
#include "HelpCommand.hpp"
#include "Message.hpp"
#include "Server.hpp"

HelpCommand::HelpCommand(Server* server) : ACommand(server) {}

HelpCommand::~HelpCommand() {}

// private, not used
HelpCommand::HelpCommand(const HelpCommand& other) : ACommand(other._server) {}

HelpCommand& HelpCommand::operator=(const HelpCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Static create method for factory
ACommand* HelpCommand::create(Server* server) { return (new HelpCommand(server)); }

// Execute the Help command
void HelpCommand::execute(Client* client, const Message& message)
{
    Print::Do("executing Help command");
}
