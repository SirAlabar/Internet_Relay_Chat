#include "WhoCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

WhoCommand::WhoCommand(Server* server) : ACommand(server) {}

WhoCommand::~WhoCommand() {}

// Private copy constructor
WhoCommand::WhoCommand(const WhoCommand& other) : ACommand(other._server) {}

WhoCommand& WhoCommand::operator=(const WhoCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* WhoCommand::create(Server* server)
{
	return (new WhoCommand(server));
}

// Execute the WHO command
void WhoCommand::execute(Client* client, const Message& message)
{
	Print::Do("execute WHO command");

	if (!client)
	{
		Print::Fail("Client NULL");
		return;
	}

	if (!client->isAuthenticated())
	{
		Print::Fail("Client not authenticated");
		sendErrorReply(client, 451, ":You have not registered");
		return;
	}

	if (client->getNickname().empty() || client->getUsername().empty())
	{
		Print::Fail("Client not fully registered");
		sendErrorReply(client, 451, ":You have not registered");
		return;
	}

	std::string target = message.getParams(0);
	Print::Debug("WHO target: '" + target + "'");

	if (target.empty())
	{
		Print::Debug("WHO without parameters - no action");
	}
	else if (target[0] == '#' || target[0] == '&')
	{
		Print::Debug("WHO for channel: " + target);
		listChannelUsers(client, target);
	}
	else
	{
		Print::Debug("WHO for user: " + target);
		listSpecificUser(client, target);
	}

	// Send end of WHO list
	sendNumericReply(client, 315, target + " :End of WHO list");
	Print::Ok("WHO command completed");
}

void WhoCommand::listChannelUsers(Client* client, const std::string& channelName)
{
	Print::Debug("Listing users in channel: " + channelName);
	
	Channel* channel = _server->getChannel(channelName);
	if (!channel)
	{
		Print::Warn("Channel not found: " + channelName);
		return;
	}
	
	std::map<int, Client*> channelClients = channel->getClients();
	int userCount = 0;
	
	for (std::map<int, Client*>::iterator it = channelClients.begin();
		 it != channelClients.end(); ++it)
	{
		Client* targetClient = it->second;
		if (targetClient && !targetClient->getNickname().empty())
		{
			sendWhoReply(client, targetClient, channelName);
			userCount++;
		}
	}
	
	Print::Debug("Listed " + toString(userCount) + " users in channel");
}

void WhoCommand::listSpecificUser(Client* client, const std::string& nickname)
{
	Print::Debug("Looking for specific user: " + nickname);
	
	Client* targetClient = _server->getClientByNick(nickname);
	if (targetClient && !targetClient->getNickname().empty())
	{
		sendWhoReply(client, targetClient, "*");
		Print::Debug("Found user: " + nickname);
	}
	else
	{
		Print::Debug("User not found: " + nickname);
	}
}

void WhoCommand::sendWhoReply(Client* client, Client* targetClient, const std::string& channel)
{
	// Format: 352 <client> <channel> <user> <host> <server> <nick> <flags> :<hopcount> <realname>
	std::string flags = "H"; // H = Here
	
	if (channel != "*")
	{
		Channel* chan = _server->getChannel(channel);
		if (chan && chan->isOperator(targetClient))
		{
			flags += "@";
		}
	}
	
	std::string response = channel + " " + targetClient->getUsername() + " localhost server " 
						 + targetClient->getNickname() + " " + flags + " :0 " + targetClient->getNickname();
	
	sendNumericReply(client, 352, response);
}