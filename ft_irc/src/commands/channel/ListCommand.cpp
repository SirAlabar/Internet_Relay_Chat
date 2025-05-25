#include "ListCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

ListCommand::ListCommand(Server* server) : ACommand(server) {}

ListCommand::~ListCommand() {}

// Private copy constructor
ListCommand::ListCommand(const ListCommand& other) : ACommand(other._server) {}

ListCommand& ListCommand::operator=(const ListCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* ListCommand::create(Server* server)
{
	return (new ListCommand(server));
}

// Execute the LIST command
void ListCommand::execute(Client* client, const Message& message)
{
Print::Do("execute LIST command");

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

	sendNumericReply(client, 321, "Channel :Users  Name");

	std::map<std::string, Channel*>& channels = _server->getChannels();
	

	std::string channelParam = message.getParams(0);
	
	if (channelParam.empty())
	{
		Print::Debug("Listing all channels");
		listAllChannels(client, channels);
	}
	else
	{
		Print::Debug("Listing specific channels: " + channelParam);
		listSpecificChannels(client, channels, channelParam);
	}

	sendNumericReply(client, 323, ":End of /LIST");
	Print::Ok("LIST command completed");
}

void ListCommand::listAllChannels(Client* client, std::map<std::string, Channel*>& channels)
{
	std::map<std::string, Channel*>::iterator it = channels.begin();
	std::map<std::string, Channel*>::iterator ite = channels.end();

	for (; it != ite; ++it)
	{
		Channel* channel = it->second;
		if (channel)
		{
			sendChannelInfo(client, channel);
		}
	}
	
	Print::Debug("Listed " + toString(channels.size()) + " channels");
}

void ListCommand::listSpecificChannels(Client* client, std::map<std::string, Channel*>& channels, const std::string& channelList)
{
	std::vector<std::string> requestedChannels = splitArguments(channelList, ',');
	int listedCount = 0;

	for (size_t i = 0; i < requestedChannels.size(); ++i)
	{
		std::string channelName = requestedChannels[i];
		
		size_t start = channelName.find_first_not_of(" \t");
		size_t end = channelName.find_last_not_of(" \t");
		
		if (start != std::string::npos && end != std::string::npos)
		{
			channelName = channelName.substr(start, end - start + 1);
		}
		
		std::map<std::string, Channel*>::iterator it = channels.find(channelName);
		if (it != channels.end() && it->second)
		{
			sendChannelInfo(client, it->second);
			listedCount++;
		}
	}
	
	Print::Debug("Listed " + toString(listedCount) + " specific channels");
}

void ListCommand::sendChannelInfo(Client* client, Channel* channel)
{
	if (!channel)
	{
		return;
	}

	// Count users in channel
	const std::map<int, Client*>& clients = channel->getClients();
	int userCount = static_cast<int>(clients.size());
	
	std::string topic = channel->getTopic();
	if (topic.empty())
	{
		topic = "No topic set";
	}

	// Format: 322 <nick> <channel> <# visible> :<topic>
	std::string listReply = channel->getName() + " " + toString(userCount) + " :" + topic;
	sendNumericReply(client, 322, listReply);
	
	Print::Debug("Sent info for channel: " + channel->getName() + " (" + toString(userCount) + " users)");
}