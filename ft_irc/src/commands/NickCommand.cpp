#include "NickCommand.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include <iostream>


NickCommand::NickCommand(Server* server) : ACommand(server)
{}

NickCommand::~NickCommand()
{}

// private, not used
NickCommand::NickCommand(const NickCommand& other) : ACommand(other._server)
{}

// private, not used
NickCommand& NickCommand::operator=(const NickCommand& other)
{
    if (this != &other)
    {
        ACommand::operator=(other);
    }
    return (*this);
}

// Static create method for factory
Command* NickCommand::create(Server* server)
{
    return (new NickCommand(server));
}

// Execute the NICK command
void NickCommand::execute(Client* client, const Message& message)
{
    if (!client)
    {
        return ;
    }

    // Get the parameters from the message
    const std::string& params = message.getParams();
    // Parse the nickname from parameters
    if (params.empty())
    {
        // No nickname provided, send error
        sendErrorReply(client, 431, "No nickname given");
        return ;
    }
    // Extract the nickname from params
    std::string nickname = params;
    // If the nickname contains spaces, only use the first part
    size_t spacePos = nickname.find(' ');
    if (spacePos != std::string::npos)
    {
        nickname = nickname.substr(0, spacePos);
    }
    // Validate the nickname
    if (!isValidNickname(nickname))
    {
        sendErrorReply(client, 432, nickname + " :Erroneous nickname");
        return;
    }
    // Check if the nickname is already in use
    Client* existingClient = _server->getClientByNick(nickname);
    if (existingClient && existingClient != client)
    {
        sendErrorReply(client, 433, nickname + " :Nickname is already in use");
        return;
    }
    // Previous nickname (if any)
    std::string oldNick = client->getNickname();
    // Set the new nickname
    client->setNickname(nickname);
    // If the client was already registered, inform others about the nick change
    if (client->isRegistered())
    {
        std::string nickChangeNotice = ":" + oldNick + " NICK :" + nickname + "\r\n";
        // Send to all channels the client is in
        client->broadcastToChannels(nickChangeNotice);
    }
    else
    {
        // Check if the client has completed registration (has both nickname and username)
        client->checkRegistration();
    }
}