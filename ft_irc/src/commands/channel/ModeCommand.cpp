#include "ModeCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

ModeCommand::ModeCommand(Server* server) : ACommand(server) {}

ModeCommand::~ModeCommand() {}

// Private copy constructor
ModeCommand::ModeCommand(const ModeCommand& other) : ACommand(other._server) {}

ModeCommand& ModeCommand::operator=(const ModeCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* ModeCommand::create(Server* server)
{
	return (new ModeCommand(server));
}

// Execute the MODE command
void ModeCommand::execute(Client* client, const Message& message)
{
	Print::Do("execute MODE command");

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

	// MODE requires at least 1 parameter (channel name)
	if (message.getSize() < 1 || message.getParams(0).empty())
	{
		Print::Fail("Not enough parameters");
		sendErrorReply(client, 461, "MODE :Not enough parameters");
		return;
	}

	std::string channelName = message.getParams(0);
	
	if (!isValidChannelName(channelName))
	{
		Print::Fail("Invalid channel name");
		sendErrorReply(client, 403, channelName + " :No such channel");
		return;
	}

	Channel* channel = _server->getChannel(channelName);
	if (!channel)
	{
		Print::Fail("Channel not found");
		sendErrorReply(client, 403, channelName + " :No such channel");
		return;
	}

	// If only channel name provided, show current modes
	if (message.getSize() == 1)
	{
		Print::Debug("Showing channel modes");
		showChannelModes(client, channel);
		return;
	}

	if (!channel->hasClient(client))
	{
		Print::Fail("Client not in channel");
		sendErrorReply(client, 442, channelName + " :You're not on that channel");
		return;
	}

	// Check if client is operator
	if (!channel->isOperator(client))
	{
		Print::Fail("Client not operator");
		sendErrorReply(client, 482, channelName + " :You're not channel operator");
		return;
	}

	std::string modeString = message.getParams(1);
	Print::Debug("Processing modes: " + modeString);

	processModeChanges(client, channel, modeString, message);

	Print::Ok("MODE command processed");
}

void ModeCommand::showChannelModes(Client* client, Channel* channel)
{
	std::string modes = "+";
	std::string params = "";

	if (channel->isInviteOnly())
	{
		modes += "i";
	}
	if (channel->isTopicRestricted())
	{
		modes += "t";
	}
	if (channel->hasKey())
	{
		modes += "k";
		if (!params.empty())
		{
			params += " ";
		}
		params += channel->getKey();
	}
	if (channel->hasUserLimit())
	{
		modes += "l";
		if (!params.empty())
		{
			params += " ";
		}
		params += toString(channel->getUserLimit());
	}
	if (modes == "+")
	{
		modes = "+";
	}

	std::string reply = ":server 324 " + client->getNickname() + " " + 
						channel->getName() + " " + modes;
	if (!params.empty())
	{
		reply += " " + params;
	}
	reply += "\r\n";

	client->sendMessage(reply);
}

void ModeCommand::processModeChanges(Client* client, Channel* channel, 
									const std::string& modeString, const Message& message)
{

}

bool ModeCommand::processInviteOnlyMode(Channel* channel, bool adding, std::string& appliedModes)
{

}

bool ModeCommand::processTopicRestrictedMode(Channel* channel, bool adding, std::string& appliedModes)
{

}

bool ModeCommand::processChannelKeyMode(Client* client, Channel* channel, bool adding,
									   const Message& message, size_t& paramIndex,
									   std::string& appliedModes, std::string& appliedParams)
{

}

bool ModeCommand::processOperatorMode(Client* client, Channel* channel, bool adding,
									 const Message& message, size_t& paramIndex,
									 std::string& appliedModes, std::string& appliedParams)
{

}

bool ModeCommand::processUserLimitMode(Client* client, Channel* channel, bool adding,
									  const Message& message, size_t& paramIndex,
									  std::string& appliedModes, std::string& appliedParams)
{

}

void ModeCommand::broadcastModeChange(Client* client, Channel* channel,
									 const std::string& appliedModes, const std::string& appliedParams)
{

}