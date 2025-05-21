#include <algorithm>
#include <sstream>

#include "ACommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"

// Constructor with server reference
ACommand::ACommand(Server* server) : _server(server) {}

ACommand::~ACommand() {}

ACommand::ACommand(const ACommand& other) : _server(other._server) {}

ACommand& ACommand::operator=(const ACommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Split arguments into a vector of strings
std::vector<std::string> ACommand::splitArguments(const std::string& args, char delimiter)
{
    std::vector<std::string> result;
    std::istringstream iss(args);
    std::string token;

    while (std::getline(iss, token, delimiter))
    {
        if (!token.empty())
        {
            result.push_back(token);
        }
    }
    return (result);
}

// Check if a channel name is valid according to IRC standards
bool ACommand::isValidChannelName(const std::string& channelName)
{
    // IRC channels typically start with # or &
    if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
    {
        return (false);
    }

    // Check for invalid characters in channel name
    for (size_t i = 0; i < channelName.size(); ++i)
    {
        // Spaces, control chars, commas, colons are not allowed
        if (channelName[i] <= 32 || channelName[i] == ',' || channelName[i] == ':')
        {
            return (false);
        }
    }

    return (true);
}

// Check if a nickname is valid according to IRC standards
bool ACommand::isValidNickname(const std::string& nickname)
{
    if (nickname.empty())
    {
        return (false);
    }

    // First character should not be a special character
    if (nickname[0] == '#' || nickname[0] == '&' || nickname[0] == ':')
    {
        return (false);
    }

    // Check for invalid characters in nickname
    for (size_t i = 0; i < nickname.size(); ++i)
    {
        // Spaces, control chars, commas, colons are not allowed
        if (nickname[i] <= 32 || nickname[i] == ',' || nickname[i] == ':')
        {
            return (false);
        }
    }

    return (true);
}

// Send a reply message to a client
void ACommand::sendReply(Client* client, const std::string& reply)
{
    if (client)
    {
        client->sendMessage(reply);
    }
}

// Send a numeric reply (according to IRC protocol) to a client
void ACommand::sendNumericReply(Client* client, int numeric, const std::string& message)
{
    if (client)
    {
        std::ostringstream oss;
        std::string nickname = client->getNickname();

        if (nickname.empty())
        {
            nickname = "*";
        }
        oss << ":server " << numeric << " " << nickname << " " << message << "\r\n";
        sendReply(client, oss.str());
    }
}

// Send an error reply to a client
void ACommand::sendErrorReply(Client* client, int numeric, const std::string& message)
{
    sendNumericReply(client, numeric, message);
}
