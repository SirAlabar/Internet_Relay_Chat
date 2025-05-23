#include "NoticeCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

NoticeCommand::NoticeCommand(Server* server) : ACommand(server) {}

NoticeCommand::~NoticeCommand() {}

// Private copy constructor
NoticeCommand::NoticeCommand(const NoticeCommand& other) : ACommand(other._server) {}

NoticeCommand& NoticeCommand::operator=(const NoticeCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* NoticeCommand::create(Server* server)
{
	return (new NoticeCommand(server));
}

// Execute the NOTICE command
void NoticeCommand::execute(Client* client, const Message& message)
{
	Print::Do("execute NOTICE command");

	if (!client)
	{
		Print::Fail("Client NULL");
		return;
	}

	if (!client->isAuthenticated())
	{
		Print::Fail("Client not authenticated");
		// NOTICE should not generate error responses, so we just log and return
		return;
	}

	if (client->getNickname().empty() || client->getUsername().empty())
	{
		Print::Fail("Client not fully registered");
		return;
	}

	if (message.getSize() < 2)
	{
		Print::Fail("Not enough parameters for NOTICE");
		// NOTICE should not generate error responses for missing parameters
		return;
	}

	std::string target = message.getParams(0);
	std::string noticeText = message.getParams(1);

	// Remove leading spaces from target
	size_t startPos = target.find_first_not_of(" \t");
	if (startPos != std::string::npos)
	{
		target = target.substr(startPos);
	}

	if (target.empty())
	{
		Print::Fail("Empty target");
		return;
	}

	if (noticeText.empty())
	{
		Print::Fail("Empty notice text");
		return;
	}

	Print::Debug("NOTICE from '" + client->getNickname() + "' to '" + target + "': " + noticeText);

	// Check if target is a channel
	if (target[0] == '#' || target[0] == '&')
	{
		sendNoticeToChannel(client, target, noticeText);
	}
	else
	{
		sendNoticeToUser(client, target, noticeText);
	}

	Print::Ok("NOTICE sent");
}

void NoticeCommand::sendNoticeToChannel(Client* sender, const std::string& channelName, const std::string& notice)
{
	Print::Debug("Sending NOTICE to channel: " + channelName);

	Channel* channel = _server->getChannel(channelName);
	if (!channel)
	{
		Print::Warn("Channel not found: " + channelName);
		// NOTICE should fail silently if channel doesn't exist
		return;
	}

	// Format the notice message
	std::string noticeMsg = ":" + sender->getNickname() + "!" + sender->getUsername() 
						  + "@localhost NOTICE " + channelName + " :" + notice + "\r\n";

	// Send to all channel members except the sender
	broadcastToChannel(channel, noticeMsg, sender->getFd());
	
	Print::Debug("Channel NOTICE formatted: " + noticeMsg);
	Print::Ok("NOTICE sent to channel: " + channelName);
}

void NoticeCommand::sendNoticeToUser(Client* sender, const std::string& targetNick, const std::string& notice)
{
	Print::Debug("Sending NOTICE to user: " + targetNick);

	Client* targetClient = _server->getClientByNick(targetNick);
	if (!targetClient)
	{
		Print::Warn("Target user not found: " + targetNick);
		// NOTICE should fail silently if user doesn't exist
		return;
	}

	// Format the notice message
	std::string noticeMsg = ":" + sender->getNickname() + "!" + sender->getUsername() 
						  + "@localhost NOTICE " + targetNick + " :" + notice + "\r\n";

	// Send the notice to the target user
	bool sent = targetClient->sendMessage(noticeMsg);
	
	if (sent)
	{
		Print::Ok("NOTICE delivered to: " + targetNick);
	}
	else
	{
		Print::Warn("Failed to deliver NOTICE to: " + targetNick);
	}

	Print::Debug("User NOTICE formatted: " + noticeMsg);
}

void NoticeCommand::broadcastToChannel(Channel* channel, const std::string& message, int excludeFd)
{
	Print::Debug("Broadcasting message to channel members");
}