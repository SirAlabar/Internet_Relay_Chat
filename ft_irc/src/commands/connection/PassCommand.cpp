#include "PassCommand.hpp"

PassCommand::PassCommand(Server* server) : ACommand(server) {}

PassCommand::~PassCommand() {}

// Private copy constructor
PassCommand::PassCommand(const PassCommand& other) : ACommand(other._server) {}

PassCommand& PassCommand::operator=(const PassCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Static creator for factory
ACommand* PassCommand::create(Server* server)
{
    return (new PassCommand(server));
}

void PassCommand::execute(Client* client, const Message& message)
{
    Print::Debug("Starting execution of PASS command");
  
    if (!client)
    {
        Print::Debug("Client is NULL, returning");
        return ;
    }

    std::string password = message.getParams();
    size_t startPass = password.find_first_not_of(" \t");
    if (password.empty() || startPass == std::string::npos)
    {
        Print::Debug("no password, returning...");
        sendErrorReply(client, 461, "PASS :Not enough parameters");
        return ;
    }
    
    password = password.substr(startPass);
    if (!password.empty() && password[0] == ':')
        password = password.substr(1);

    size_t endPass = password.find_last_not_of(" \t\r\n");
    if (endPass != std::string::npos)
        password = password.substr(0, endPass + 1);

    if (client->isAuthenticated())
    {
        sendErrorReply(client, 462, ":You may not reregister");
        return ;
    }

    if(password == _server->getPassword())
        client->setAuthenticated(true);
    else
        sendErrorReply(client, 464, ":Password incorrect!");
}
