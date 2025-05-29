#include "ABotCommand.hpp"
#include "BotContext.hpp"
#include "General.hpp"

// Constructor with server reference
ABotCommand::ABotCommand(Server* server) : _server(server) {}

ABotCommand::~ABotCommand() {}

ABotCommand::ABotCommand(const ABotCommand& other) : _server(other._server) {}

ABotCommand& ABotCommand::operator=(const ABotCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Split arguments into a vector of strings
std::vector<std::string> ABotCommand::splitArguments(const std::string& args,
                                                     char delimiter)
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
bool ABotCommand::isValidChannelName(const std::string& channelName) const
{
    if (channelName.length() < 2)
    {
        return false;
    }

    // IRC channels typically start with # or &
    Print::Warn("Valid Name? '" + channelName + "'");
    if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
    {
        return (false);
    }

    // Check for invalid characters in channel name
    for (size_t i = 1; i < channelName.size(); ++i)
    {
        // Spaces, control chars, commas, colons are not allowed
        if (channelName[i] <= 32 || channelName[i] == ',' || channelName[i] == ':' ||
            channelName[i] == 7)
        {
            return (false);
        }
    }

    return (true);
}

// Check if a nickname is valid according to IRC standards
bool ABotCommand::isValidNickname(const std::string& nickname) const
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
void ABotCommand::sendReply(Client* client, const std::string& reply) const
{
    if (client)
    {
        client->sendMessage(reply);
    }
}

// Send a numeric reply (according to IRC protocol) to a client
void ABotCommand::sendNumericReply(Client* client, int numeric,
                                   const std::string& message) const
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
void ABotCommand::sendErrorReply(Client* client, int numeric,
                                 const std::string& message) const
{
    sendNumericReply(client, numeric, message);
}

bool ABotCommand::validateClient(Client* client) const
{
    return client ? true : (Print::Fail("Client NULL"), false);
}

bool ABotCommand::validateClientRegist(Client* client) const
{
    if (!validateClient(client)) return false;
    if (!client->isAuthenticated() || client->getNickname().empty())
    {
        Print::Fail("Client not properly registered!");
        sendErrorReply(client, IRC::ERR_NOTREGISTERED, ":You have not registered");
        return false;
    }
    return true;
}

bool ABotCommand::validateParameterCount(Client* client, const Message& message,
                                         size_t minParams,
                                         const std::string& commandName) const
{
    if (!validateClient(client)) return false;

    if (message.getSize() < minParams || message.getParams(0).empty())
    {
        Print::Fail("Not enough parameters for " + commandName);
        sendErrorReply(client, IRC::ERR_NEEDMOREPARAMS,
                       commandName + " :Not enough parameters");
        return false;
    }
    return true;
}

Channel* ABotCommand::validateAndGetChannel(Client* client,
                                            const std::string& channelName)
{
    if (!isValidChannelName(channelName))
    {
        Print::Warn("Invalid channel name: " + channelName);
        sendErrorReply(client, IRC::ERR_NOSUCHCHANNEL, channelName + " :No such channel");
        return NULL;
    }
    if (!validateClient(client)) return NULL;

    Channel* channel = _server->getChannel(channelName);
    if (!channel)
    {
        Print::Warn("Channel does not exist: " + channelName);
        sendErrorReply(client, IRC::ERR_NOSUCHCHANNEL, channelName + " :No such channel");
        return NULL;
    }
    return channel;
}

bool ABotCommand::validateChannelMembership(Client* client, Channel* channel,
                                            const std::string& channelName) const
{
    if (!validateClient(client)) return false;

    if (!channel->hasClient(client))
    {
        Print::Warn("Client not in channel: " + channelName);
        sendErrorReply(client, IRC::ERR_NOTONCHANNEL,
                       channelName + " :You're not on that channel");
        return false;
    }
    return true;
}
