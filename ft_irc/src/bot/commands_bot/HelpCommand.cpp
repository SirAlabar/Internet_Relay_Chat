#include <iostream>

#include "BotContext.hpp"
#include "Client.hpp"
#include "HelpCommand.hpp"
#include "Message.hpp"
#include "Server.hpp"

HelpCommand::HelpCommand(Server* server) : ABotCommand(server) {}

HelpCommand::~HelpCommand() {}

// private, not used
HelpCommand::HelpCommand(const HelpCommand& other) : ABotCommand(other._server) {}

HelpCommand& HelpCommand::operator=(const HelpCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Static create method for factory
ABotCommand* HelpCommand::create(Server* server) { return (new HelpCommand(server)); }

// Execute the Help command
void HelpCommand::execute(BotContext* botctx, std::string& message)
{
    if (!botctx) return;
    (void)message;
    Print::Do("executing Help command");
}
