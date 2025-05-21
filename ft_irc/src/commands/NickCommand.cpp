#include <iostream>

#include "Client.hpp"
#include "Message.hpp"
#include "NickCommand.hpp"
#include "Server.hpp"

NickCommand::NickCommand(Server* server) : ACommand(server) {}

NickCommand::~NickCommand() {}

// private, not used
NickCommand::NickCommand(const NickCommand& other) : ACommand(other._server) {}

NickCommand& NickCommand::operator=(const NickCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Static create method for factory

ACommand* NickCommand::create(Server* server)
{
    return (new NickCommand(server));
}

// Execute the NICK command
void NickCommand::execute(Client* client, const Message& message)
{
    Print::Debug("Starting execution of NICK command");
    if (!client)
    {
        Print::Debug("Client is NULL, returning");
        return;
    }
    // TODO: Move parser to a virtual Acommand::parse() = 0, so that we clean the execute
    // function Get the parameters from the message
    const std::string nickname = message.getParams(0);
    Print::Debug("NICK parameters: '" + nickname + "'");
    // Parse the nickname from parameters
    if (nickname.empty())
    {
        Print::Debug("Empty parameters, sending error");
        // No nickname provided, send error
        sendErrorReply(client, 431, "No nickname given");
        return;
    }
    // Extract the nickname from params
    if (!isValidNickname(nickname))  // TODO: sloth: I need to take only the first word
                                     // even when we have `:`
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
    Print::Debug("Extracted nickname: '" + nickname + "'");
    // Previous nickname (if any)
    std::string oldNick = client->getNickname();
    Print::Debug("Old nickname: '" + oldNick + "'");
    // Set the new nickname
    client->setNickname(nickname);
    Print::Debug("Nickname updated to: '" + client->getNickname() + "'");
    // If the client was already registered, inform others about the nick
    // change 

    if (client->isAuthenticated())
    {
        std::string nickChangeNotice = ":" + oldNick + " NICK :" + nickname + "\r\n";
        // Send to all channels the client is in
        // client->broadcastToChannels(nickChangeNotice);
        Print::Debug("Sending response");
        client->sendMessage(nickChangeNotice);
    }
    else
    {
        Print::Debug("Sending confirmation to unauthenticated client");
        std::string confirmation = ":server NICK :" + nickname + "\r\n";
        client->sendMessage(confirmation);
        // Check if the client has completed registration (has both nickname
        // and username) client->checkRegistration();
    }
}
