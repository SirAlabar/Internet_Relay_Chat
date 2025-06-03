#include "InviteCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

InviteCommand::InviteCommand(Server* server) : ACommand(server) {}

InviteCommand::~InviteCommand() {}

// Private copy constructor
InviteCommand::InviteCommand(const InviteCommand& other) : ACommand(other._server) {}

InviteCommand& InviteCommand::operator=(const InviteCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* InviteCommand::create(Server* server)
{
	return (new InviteCommand(server));
}

// Execute the INVITE command
void InviteCommand::execute(Client* client, const Message& message)
{
	Print::Do("execute INVITE command");

	if (!client)
	{
		Print::Fail("Client NULL");
		return;
	}

	if (!client->isAuthenticated() || client->getNickname().empty())
	{
		Print::Fail("Client not properly registered");
		sendErrorReply(client, IRC::ERR_NOTREGISTERED, ":You have not registered");
		return;
	}

	// INVITE requires exactly 2 parameters: nickname and channel
	if (message.getSize() < 2 || message.getParams(0).empty() || message.getParams(1).empty())
	{
		Print::Fail("Not enough parameters for INVITE");
		sendErrorReply(client, IRC::ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters");
		return;
	}

	std::string targetNick = message.getParams(0);
	std::string channelName = message.getParams(1);

	Print::Debug("INVITE command: inviting '" + targetNick + "' to '" + channelName + "'");

	if (!isValidChannelName(channelName))
	{
		Print::Warn("Invalid channel name: " + channelName);
		sendErrorReply(client, IRC::ERR_NOSUCHCHANNEL, channelName + " :No such channel");
		return;
	}

	Channel* channel = _server->getChannel(channelName);
	if (!channel)
	{
		Print::Warn("Channel does not exist: " + channelName);
		sendErrorReply(client, IRC::ERR_NOSUCHCHANNEL, channelName + " :No such channel");
		return;
	}

	if (!channel->hasClient(client))
	{
		Print::Warn("Inviter not in channel: " + channelName);
		sendErrorReply(client, IRC::ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
		return;
	}

	// uncoment after mode command
	// if (channel->hasMode('i') && !channel->isOperator(client))
	// {
	// 	Print::Warn("Inviter is not operator in: " + channelName);
	// 	sendErrorReply(client, 482, channelName + " :You're not channel operator");
	// 	return;
	// }

	Client* targetClient = _server->getClientByNick(targetNick);
	if (!targetClient)
	{
		Print::Warn("Target user not found: " + targetNick);
		sendErrorReply(client, IRC::ERR_NOSUCHNICK, targetNick + " :No such nick/channel");
		return;
	}

	if (channel->hasClient(targetClient))
	{
		Print::Warn("Target already in channel: " + targetNick + " in " + channelName);
		sendErrorReply(client, IRC::ERR_USERONCHANNEL, targetNick + " " + channelName + " :is already on channel");
		return;
	}

	executeInvite(client, targetClient, channel);

	Print::Ok("INVITE command completed successfully");
}

void InviteCommand::executeInvite(Client* inviter, Client* target, Channel* channel)
{
	std::string inviterNick = inviter->getNickname();
	std::string inviterUser = inviter->getUsername();
	std::string targetNick = target->getNickname();
	std::string channelName = channel->getName();

	Print::Debug("Executing invite: " + inviterNick + " invites " + targetNick + " to " + channelName);

	channel->addInvitedUser(targetNick);
	// Send RPL_INVITING (341) to the inviter
	sendNumericReply(inviter, IRC::RPL_INVITING, targetNick + " " + channelName);

	// Send INVITE message to the target user
	std::string inviteMsg = ":" + inviterNick;
	if (!inviterUser.empty())
	{
		inviteMsg += "!" + inviterUser + "@localhost";
	}
	inviteMsg += " INVITE " + targetNick + " :" + channelName + "\r\n";

	Print::Debug("Sending invite message to target: " + inviteMsg);

	if (target->sendMessage(inviteMsg))
	{
		Print::Ok("Invitation sent from " + inviterNick + " to " + targetNick + " for channel " + channelName);
	}
	else
	{
		Print::Fail("Failed to send invitation to " + targetNick);
	}
}
