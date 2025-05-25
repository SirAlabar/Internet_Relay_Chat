#include "PartCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

PartCommand::PartCommand(Server* server) : ACommand(server) {}

PartCommand::~PartCommand() {}

// Private copy constructor
PartCommand::PartCommand(const PartCommand& other) : ACommand(other._server) {}

PartCommand& PartCommand::operator=(const PartCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* PartCommand::create(Server* server)
{
	return (new PartCommand(server));
}

// Execute the PART command
void PartCommand::execute(Client* client, const Message& message)
{
	Print::Do("execute PART command");

	if (!client)
	{
		Print::Fail("Client NULL");
		return;
	}

	if (!client->isAuthenticated() || client->getNickname().empty())
	{
		Print::Fail("Client not properly registered");
		sendErrorReply(client, 451, ":You have not registered");
		return;
	}

	if (message.getSize() == 0 || message.getParams(0).empty())
	{
		Print::Fail("No channel specified");
		sendErrorReply(client, 461, "PART :Not enough parameters");
		return;
	}

	std::string channelList = message.getParams(0);
	std::string partMessage = "";

	if (message.getSize() > 1)
	{
		partMessage = message.getParams(1);
		Print::Debug("Part message: '" + partMessage + "'");
	}

	std::vector<std::string> channels = splitArguments(channelList, ',');
	
	Print::Debug("Processing PART for " + toString(channels.size()) + " channel(s)");

	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string channelName = channels[i];
		
		size_t start = channelName.find_first_not_of(" \t");
		size_t end = channelName.find_last_not_of(" \t");
		
		if (start != std::string::npos && end != std::string::npos)
		{
			channelName = channelName.substr(start, end - start + 1);
		}

		Print::Debug("Processing channel: '" + channelName + "'");
		
		if (!isValidChannelName(channelName))
		{
			Print::Warn("Invalid channel name: " + channelName);
			sendErrorReply(client, 403, channelName + " :No such channel");
			continue;
		}

		Channel* channel = _server->getChannel(channelName);
		if (!channel)
		{
			Print::Warn("Channel does not exist: " + channelName);
			sendErrorReply(client, 403, channelName + " :No such channel");
			continue;
		}

		if (!channel->hasClient(client))
		{
			Print::Warn("Client not in channel: " + channelName);
			sendErrorReply(client, 442, channelName + " :You're not on that channel");
			continue;
		}

		executePartFromChannel(client, channel, partMessage);
	}

	Print::Ok("PART command completed");
}

void PartCommand::executePartFromChannel(Client* client, Channel* channel, const std::string& partMessage)
{
	std::string channelName = channel->getName();
	std::string clientNick = client->getNickname();
	std::string clientUser = client->getUsername();

	Print::Debug("Removing " + clientNick + " from channel " + channelName);

	// Create PART message to broadcast to channel members
	std::string broadcastMsg = ":" + clientNick;
	if (!clientUser.empty())
	{
		broadcastMsg += "!" + clientUser + "@localhost";
	}
	broadcastMsg += " PART " + channelName;
	
	if (!partMessage.empty())
	{
		broadcastMsg += " :" + partMessage;
	}
	broadcastMsg += "\r\n";

	_server->broadcastChannel(broadcastMsg, channelName, -1);

	channel->removeClient(client);
	
	Print::Debug("Client " + clientNick + " removed from channel " + channelName);

	// Clean up empty channels
	if (channel->isEmpty())
	{
		Print::Debug("Channel " + channelName + " is now empty, removing it");
		_server->removeChannel(channelName);
		Print::Ok("Empty channel " + channelName + " removed");
	}
	else
	{
		Print::Debug("Channel " + channelName + " still has " + 
					toString(channel->getClients().size()) + " members");
	}
}