#include "PrintdataCommand.hpp"

PrintdataCommand::PrintdataCommand(Server* server) : ACommand(server) {}

PrintdataCommand::~PrintdataCommand() {}

// Private copy constructor
PrintdataCommand::PrintdataCommand(const PrintdataCommand& other) : ACommand(other._server) {}

PrintdataCommand& PrintdataCommand::operator=(const PrintdataCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Static creator for factory
ACommand* PrintdataCommand::create(Server* server) { return (new PrintdataCommand(server)); }

// Execute the JOIN command
void PrintdataCommand::execute(Client* client, const Message& message)
{
    (void) client;
    (void) message;
    _server->print_clients(true);
}
