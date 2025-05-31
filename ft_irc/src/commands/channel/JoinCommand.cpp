#include <new>

#include "Channel.hpp"
#include "Client.hpp"
#include "JoinCommand.hpp"
#include "General.hpp"
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
	
	if(message.getParams(0) == "0")
	{
		_server->removeClientFromChannels(client);
		Print::Ok("client " + Color::YELLOW + client->getNickname()
				  + Color::RESET + " left all channels");
		return ;
	}

	std::vector<std::string> channels = splitArguments(message.getParams(0), ',');
	std::vector<std::string> keys = (message.getSize() > 1 ?
		splitArguments(message.getParams(1), ',') : std::vector<std::string>());

	for (size_t i = 0; i < channels.size(); i++)
	{
		joinChannel(client, channels[i],
					(i >= keys.size() ? "" : keys[i]));
	}
}

void    JoinCommand::joinChannel(Client* client, const std::string& channelName, const std::string& key)
{
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
		if(key != "")
		{
			channel->setKey(key);
		}
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
			{
				if (channel->isInviteOnly())
				{
					Print::Warn("Channel is invite-only");
					sendErrorReply(client, IRC::ERR_INVITEONLYCHAN, channelName + " :Cannot join channel (+i)");
					return;
				}
				if (channel->hasKey() 
					&& (key.empty() || key != channel->getKey()))
				{
					Print::Warn("Wrong or missing channel key");
					sendErrorReply(client, IRC::ERR_BADCHANNELKEY, channelName + " :Cannot join channel (+k)");
					return;
				}
				if (channel->hasUserLimit()
					&& channel->getClients().size() >= channel->getUserLimit()
					&& !client->isBot())
				{
					Print::Warn("Channel full");
					sendErrorReply(client, IRC::ERR_CHANNELISFULL, channelName + " :Cannot join channel (+l)");
					return;
				}
			}
			channel->addClient(client);
			client->sendMessage(joinMessage);
			_server->broadcastChannel(joinMessage, channelName, client->getFd());
		}
		if (!channel->getTopic().empty())
		{
			sendNumericReply(client, 332, channelName + " :" + channel->getTopic());
		}
		else
		{
			sendNumericReply(client, 331, channelName + " :No topic is set");
		}
	}

	sendList(client, channel);

	Print::Ok("Join " + Color::YELLOW +  client->getNickname() 
			  + Color::RESET + " to " 
			  + Color::YELLOW + channelName + Color::RESET);
}

void    JoinCommand::sendList(Client* client, Channel* channel)
{
	if(!channel || !client)
	{
		return;
	}

	std::string list = "";
	const std::map<int, Client*>& clients = channel->getClients();

	for(std::map<int, Client*>::const_iterator it = clients.begin();
		it != clients.end(); it++)
	{
		if(it->second && !it->second->isBot())
		{
			if (!list.empty())
			{
				list += " ";
			}
			if (channel->isOperator(it->second))
			{
				list += "@";
			}
			list += it->second->getNickname();
		}
	}

	sendNumericReply(client, IRC::RPL_NAMREPLY,
					 "= " + channel->getName() + " :" + list);
	sendNumericReply(client, IRC::RPL_ENDOFNAMES,
					 channel->getName() + " :End of NAMES list");
}
