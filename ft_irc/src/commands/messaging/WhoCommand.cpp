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
		Print::Debug("WHO without parameters - listing all visible users");
		listAllUsers(client);
	}
	else if (target[0] == '#' || target[0] == '&')
	{
		Print::Debug("WHO for channel: " + target);
		listChannelUsers(client, target);
	}
	else
	{
		Print::Debug("WHO for user/mask: " + target);
		listUsersByMask(client, target);
	}

	// Send end of WHO list
	sendNumericReply(client, 315, target + " :End of WHO list");
	Print::Ok("WHO command completed");
}

void WhoCommand::listAllUsers(Client* client)
{
	Print::Debug("Listing all visible users");
	
	std::map<std::string, Channel*>& channels = _server->getChannels();
	std::set<std::string> listedUsers; // To avoid duplicates
	
	int userCount = 0;
	
	// List users from all channels
	for (std::map<std::string, Channel*>::iterator chanIt = channels.begin(); 
		 chanIt != channels.end(); ++chanIt)
	{
		Channel* channel = chanIt->second;
		if (channel)
		{
			// Get all clients in the channel and send WHO replies
			std::map<int, Client*> channelClients = getChannelClients(channel);
			
			for (std::map<int, Client*>::iterator clientIt = channelClients.begin();
				 clientIt != channelClients.end(); ++clientIt)
			{
				Client* targetClient = clientIt->second;
				if (targetClient && !targetClient->getNickname().empty())
				{
					// Avoid duplicate entries
					if (listedUsers.find(targetClient->getNickname()) == listedUsers.end())
					{
						sendWhoReply(client, targetClient, "*");
						listedUsers.insert(targetClient->getNickname());
						userCount++;
					}
				}
			}
		}
	}
	Print::Debug("Listed " + toString(userCount) + " users");
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
	
	// Get all clients in the channel
	std::map<int, Client*> channelClients = getChannelClients(channel);
	int userCount = 0;
	
	for (std::map<int, Client*>::iterator clientIt = channelClients.begin();
		 clientIt != channelClients.end(); ++clientIt)
	{
		Client* targetClient = clientIt->second;
		if (targetClient && !targetClient->getNickname().empty())
		{
			sendWhoReply(client, targetClient, channelName);
			userCount++;
		}
	}
	
	Print::Debug("Listed " + toString(userCount) + " users in channel: " + channelName);
}

void WhoCommand::listUsersByMask(Client* client, const std::string& mask)
{
	Print::Debug("Listing users matching mask: " + mask);
	
	// Check if it's a specific nickname
	Client* targetClient = _server->getClientByNick(mask);
	if (targetClient && !targetClient->getNickname().empty())
	{
		sendWhoReply(client, targetClient, "*");
		Print::Debug("Found user: " + mask);
	}
	else
	{
		Print::Debug("User not found: " + mask);
	}
}

void WhoCommand::sendWhoReply(Client* client, Client* targetClient, const std::string& channel)
{

}

std::map<int, Client*> WhoCommand::getChannelClients(Channel* channel)
{

}