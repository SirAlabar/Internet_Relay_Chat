#include "WhoIsCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

WhoIsCommand::WhoIsCommand(Server* server) : ACommand(server) {}

WhoIsCommand::~WhoIsCommand() {}

// Private copy constructor
WhoIsCommand::WhoIsCommand(const WhoIsCommand& other) : ACommand(other._server) {}

WhoIsCommand& WhoIsCommand::operator=(const WhoIsCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* WhoIsCommand::create(Server* server)
{
	return (new WhoIsCommand(server));
}

// Execute the WHOIS command
void WhoIsCommand::execute(Client* client, const Message& message)
{
	Print::Do("execute WHOIS command");

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

	if (message.getSize() < 1)
	{
		Print::Fail("Not enough parameters for WHOIS");
		sendErrorReply(client, 431, ":No nickname given");
		return;
	}

	std::string targetNick = message.getParams(0);
	Print::Debug("WHOIS request for: '" + targetNick + "'");

	Client* targetClient = _server->getClientByNick(targetNick);
	if (!targetClient)
	{
		Print::Warn("Target user not found: " + targetNick);
		sendErrorReply(client, 401, targetNick + " :No such nick/channel");
		return;
	}

	sendWhoIsInfo(client, targetClient);
	Print::Ok("WHOIS information sent for: " + targetNick);
}

void WhoIsCommand::sendWhoIsInfo(Client* client, Client* targetClient)
{
	std::string targetNick = targetClient->getNickname();
	std::string targetUser = targetClient->getUsername();
	
	Print::Debug("Sending WHOIS info for: " + targetNick);

	// 311 RPL_WHOISUSER: <nick> <user> <host> * :<real name>
	std::string whoisUser = targetNick + " " + targetUser + " localhost * :" + targetNick;
	sendNumericReply(client, 311, whoisUser);
	Print::Debug("Sent RPL_WHOISUSER");

	// 319 RPL_WHOISCHANNELS: <nick> :<channels> (se tiver canais)
	std::string channels = getClientChannels(targetClient);
	if (!channels.empty())
	{
		std::string whoisChannels = targetNick + " :" + channels;
		sendNumericReply(client, 319, whoisChannels);
		Print::Debug("Sent RPL_WHOISCHANNELS: " + channels);
	}

	// 318 RPL_ENDOFWHOIS: <nick> :End of WHOIS list
	std::string endWhois = targetNick + " :End of WHOIS list";
	sendNumericReply(client, 318, endWhois);
	Print::Debug("Sent RPL_ENDOFWHOIS");

	Print::Ok("WHOIS completed for: " + targetNick);
}

std::string WhoIsCommand::getClientChannels(Client* targetClient)
{
	std::string channelList = "";
	std::map<std::string, Channel*>& channels = _server->getChannels();
	
	Print::Debug("Getting channels for user: " + targetClient->getNickname());
	
	int channelCount = 0;
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); 
		 it != channels.end(); ++it)
	{
		Channel* channel = it->second;
		if (channel && channel->hasClient(targetClient))
		{
			if (!channelList.empty())
			{
				channelList += " ";
			}
			
			// Add @ if user is operator in this channel
			if (channel->isOperator(targetClient))
			{
				channelList += "@";
			}
			
			channelList += channel->getName();
			channelCount++;
		}
	}
	
	Print::Debug("Found " + toString(channelCount) + " channels");
	return channelList;
}