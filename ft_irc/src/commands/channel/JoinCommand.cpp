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
    Print::Do("executing JOIN command");
    if (!client || !client->isAuthenticated())
    {
        Print::Fail("Client NULL or not auth");
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
        sendErrorReply(client, 461, "USER :");
        return;
    }
    Channel* channel = _server->getChannel(channelName);
    if (!channel)
    {
        _server->createChannel(channelName, client);
        _server->getChannels()[channelName] = channel;
    }
    else
    {
        channel->addClient(client);
        std::string joinMessage =
            ":" + client->getNickname() + " JOIN :" + channelName + "\r\n";
        client->sendMessage(joinMessage);

        if (!channel->getTopic().empty())
            sendNumericReply(client, 332, channelName + " :" + channel->getTopic());
    }
    // Future implementation
}
