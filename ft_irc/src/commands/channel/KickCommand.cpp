#include "KickCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

KickCommand::KickCommand(Server* server) : ACommand(server) {}

KickCommand::~KickCommand() {}

// Private copy constructor
KickCommand::KickCommand(const KickCommand& other) : ACommand(other._server) {}

KickCommand& KickCommand::operator=(const KickCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* KickCommand::create(Server* server)
{
	return (new KickCommand(server));
}

// Execute the KICK command
void KickCommand::execute(Client* client, const Message& message)
{
	Print::Do("execute KICK command");

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

	if (message.getSize() < 2 || message.getParams(0).empty() || message.getParams(1).empty())
	{
		Print::Fail("Not enough parameters for KICK");
		sendErrorReply(client, 461, "KICK :Not enough parameters");
		return;
	}

	std::string channelName = message.getParams(0);
	std::string targetNick = message.getParams(1);
	std::string kickReason = "";

	if (message.getSize() > 2)
	{
		kickReason = message.getParams(2);
		Print::Debug("Kick reason: '" + kickReason + "'");
	}
	else
	{
		kickReason = client->getNickname();
	}

	Print::Debug("KICK command: channel='" + channelName + "' target='" + targetNick + "' reason='" + kickReason + "'");

	if (!isValidChannelName(channelName))
	{
		Print::Warn("Invalid channel name: " + channelName);
		sendErrorReply(client, 403, channelName + " :No such channel");
		return;
	}

	Channel* channel = _server->getChannel(channelName);
	if (!channel)
	{
		Print::Warn("Channel does not exist: " + channelName);
		sendErrorReply(client, 403, channelName + " :No such channel");
		return;
	}

	// Check if the kicker is in the channel
	if (!channel->hasClient(client))
	{
		Print::Warn("Kicker not in channel: " + channelName);
		sendErrorReply(client, 442, channelName + " :You're not on that channel");
		return;
	}

	// Check if the kicker is an operator
	if (!channel->isOperator(client))
	{
		Print::Warn("Kicker is not operator in: " + channelName);
		sendErrorReply(client, 482, channelName + " :You're not channel operator");
		return;
	}

	// Find target client by nickname
	Client* targetClient = _server->getClientByNick(targetNick);
	if (!targetClient)
	{
		Print::Warn("Target user not found: " + targetNick);
		sendErrorReply(client, 401, targetNick + " :No such nick/channel");
		return;
	}

	// Check if target is in the channel
	if (!channel->hasClient(targetClient))
	{
		Print::Warn("Target not in channel: " + targetNick + " in " + channelName);
		sendErrorReply(client, 441, targetNick + " " + channelName + " :They aren't on that channel");
		return;
	}

	// Execute the kick
	executeKickFromChannel(client, targetClient, channel, kickReason);

	Print::Ok("KICK command completed successfully");
}

void KickCommand::executeKickFromChannel(Client* kicker, Client* target, Channel* channel, const std::string& reason)
{
	std::string channelName = channel->getName();
	std::string kickerNick = kicker->getNickname();
	std::string kickerUser = kicker->getUsername();
	std::string targetNick = target->getNickname();

	Print::Debug("Executing kick: " + kickerNick + " kicks " + targetNick + " from " + channelName);

	std::string broadcastMsg = ":" + kickerNick;
	if (!kickerUser.empty())
	{
		broadcastMsg += "!" + kickerUser + "@localhost";
	}
	broadcastMsg += " KICK " + channelName + " " + targetNick;
	
	if (!reason.empty())
	{
		broadcastMsg += " :" + reason;
	}
	broadcastMsg += "\r\n";

	Print::Debug("Broadcasting KICK message: " + broadcastMsg);

	_server->broadcastChannel(broadcastMsg, channelName, -1);

	channel->removeClient(target);
	
	Print::Ok("User " + targetNick + " kicked from " + channelName + " by " + kickerNick);

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