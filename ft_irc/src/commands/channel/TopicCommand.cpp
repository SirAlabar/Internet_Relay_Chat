#include "Channel.hpp"
#include "Client.hpp"
#include "General.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "TopicCommand.hpp"
#include "UtilsFun.hpp"

TopicCommand::TopicCommand(Server* server) : ACommand(server) {}

TopicCommand::~TopicCommand() {}

// Private copy constructor
TopicCommand::TopicCommand(const TopicCommand& other) : ACommand(other._server) {}

TopicCommand& TopicCommand::operator=(const TopicCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Static creator for factory
ACommand* TopicCommand::create(Server* server) { return (new TopicCommand(server)); }

// Execute the TOPIC command
void TopicCommand::execute(Client* client, const Message& message)
{
    Print::Do("execute TOPIC command");
    if (!validateClientRegist(client) ||
        !validateParameterCount(client, message, 1, "TOPIC"))
    {
        return;
    }

    std::string channelName = message.getParams(0);
    Channel* channel = validateAndGetChannel(client, channelName);
    if (!channel || !validateChannelMembership(client, channel, channelName))
    {
        return;
    }

    if (message.getSize() == 1)
    {
        std::string currentTopic = channel->getTopic();
        if (currentTopic.empty())
        {
            sendNumericReply(client, IRC::RPL_NOTOPIC, channelName + " :No topic is set");
        }
        else
        {
            sendNumericReply(client, IRC::RPL_TOPIC, channelName + " :" + currentTopic);
        }
        return;
    }

    if (channel->isTopicRestricted() && !channel->isOperator(client))
    {
        Print::Warn("Client isn't operator and topic is restricted");
        sendErrorReply(client, IRC::ERR_CHANOPRIVSNEEDED,
                       channelName + " :You're not channel operator");
        return;
    }

    std::string newTopic = message.getParams(1);
    if (newTopic.empty())
    {
        channel->setTopic("");
    }
    else
    {
        channel->setTopic(newTopic);
    }

    std::string topicMsg = ":" + client->getNickname();
    if (!client->getUsername().empty())
    {
        topicMsg += "!" + client->getUsername() + "@localhost";
        // need to change that localhost, blhac!!!
    }

    topicMsg += " TOPIC " + channelName + " :" + newTopic + "\r\n";
    _server->broadcastChannel(topicMsg, channelName, -1);
    Print::Ok("");
}
