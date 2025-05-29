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
ACommand* NickCommand::create(Server* server) { return (new NickCommand(server)); }

// Execute the NICK command
void NickCommand::execute(Client* client, const Message& message)
{
    Print::Do("executing NICK command");
    if (!client || !client->isAuthenticated())
    {
        Print::Fail("Client is NULL, returning");
        return;
    }

    // Get the parameters from the message
    const std::string& params = message.getParams(0);
    Print::Debug("NICK parameters: '" + params + "'");
    // Parse the nickname from parameters
    if (params.empty())
    {
        Print::Fail("Empty parameters, sending error");
        // No nickname provided, send error
        sendErrorReply(client, 431, "No nickname given");
        return;
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
    if ((existingClient && existingClient != client))
    {
        sendErrorReply(client, 433, nickname + " :Nickname is already in use");
        return;
    }
    if ((nickname.find("IRCBot") != std::string::npos && client->isBot() == false))
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
        Print::Ok("Sending response...");
        client->sendMessage(nickChangeNotice);
    }
    else
    {
        Print::Warn("Sending confirmation to unauthenticated client");
        std::string confirmation = ":server NICK :" + nickname + "\r\n";
        client->sendMessage(confirmation);
        // Check if the client has completed registration (has both nickname
        // and username) client->checkRegistration();
    }
    if (!client->getNickname().empty() && !client->getUsername().empty())
    {
        sendNumericReply(client, 001,
                         ":Welcome to the IRC Network " + client->getNickname() + "!" +
                             client->getUsername() + "@localhost");
        sendNumericReply(client, 002, ":Host is server, running version 1.0");
        {
            Message motdMessage("MOTD");
            MotdCommand motdCmd(_server);
            motdCmd.execute(client, motdMessage);
        }

        Print::Ok("Client registration done!");
    }
    else
    {
        Print::Warn("Registration not complete. Nick: '" + client->getNickname() +
                    "', User: '" + client->getUsername() + "'");
    }
}
