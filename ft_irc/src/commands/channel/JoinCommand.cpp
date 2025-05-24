#include <new>

#include "Channel.hpp"
#include "Client.hpp"
#include "JoinCommand.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "UtilsFun.hpp"

JoinCommand::JoinCommand(Server* server) : ACommand(server) {}

JoinCommand::~JoinCommand() {}

// Private copy constructor
JoinCommand::JoinCommand(const JoinCommand& other) : ACommand(other._server) {}

JoinCommand& JoinCommand::operator=(const JoinCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Static creator for factory
ACommand* JoinCommand::create(Server* server) { return (new JoinCommand(server)); }

// Execute the JOIN command
void JoinCommand::execute(Client* client, const Message& message)
{
    Print::Do("exec JOIN command to " + message.getParams(0));
    if (!client || !client->isAuthenticated() || client->getNickname().empty())
    {
        Print::Fail("Client NULL, not auth or without NickName");
        return;
    }

    if (message.getSize() < 1 || (message.getParams(0)).empty())
    {
        Print::Fail("wrong message size or empty user");
        sendErrorReply(client, 461, "USER :Not enough parameters");
        return;
    }
    std::string channelName = message.getParams(0);
    if (!isValidChannelName(channelName))
    {
        Print::Fail("wrong channel name");
        sendErrorReply(client, 403, channelName + " :No such channel");
        return;
    }

    Channel* channel = _server->getChannel(channelName);
    std::string joinMessage =
                ":" + client->getNickname() + " JOIN :" + channelName + "\r\n";
    if (!channel)
    {
        channel = _server->createChannel(channelName, client);
        _server->getChannels()[channelName] = channel;
        client->sendMessage(joinMessage);
    }
    else
    {
        Print::Debug(channel->getName());
        if (channel->getClients().find(client->getFd()) != channel->getClients().end())
        {
            Print::Fail("User already in channel");
            sendErrorReply(client, 403, channelName + " :No such channel");
            return;
        }
        else
        {
            channel->addClient(client);
            client->sendMessage(joinMessage);
            _server->broadcastChannel(joinMessage, channelName, client->getFd());
        }
        if (!channel->getTopic().empty())
            sendNumericReply(client, 332, channelName + " :" + channel->getTopic());
    }
    Print::Ok("Join " + Color::YELLOW +  client->getNickname() 
              + Color::RESET + " to " 
              + Color::YELLOW + channelName + Color::RESET);
}
