#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "PrivmsgCommand.hpp"
#include "Server.hpp"
#include "UtilsFun.hpp"

PrivmsgCommand::PrivmsgCommand(Server* server) : ACommand(server) {}

PrivmsgCommand::~PrivmsgCommand() {}

// Private copy constructor
PrivmsgCommand::PrivmsgCommand(const PrivmsgCommand& other) : ACommand(other._server) {}

PrivmsgCommand& PrivmsgCommand::operator=(const PrivmsgCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* PrivmsgCommand::create(Server* server) { return (new PrivmsgCommand(server)); }

// Execute the PRIVMSG command
void PrivmsgCommand::execute(Client* client, const Message& message)
{
	Print::Do("execute PRIVMSG command");

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

	// PRIVMSG requires at least 2 parameters: target and message
	if (message.getSize() < 2)
	{
		if (message.getSize() < 1 || message.getParams(0).empty())
		{
			Print::Fail("No recipient specified");
			sendErrorReply(client, 411, ":No recipient given (PRIVMSG)");
			return;
		}
		else
		{
			Print::Fail("No text to send");
			sendErrorReply(client, 412, ":No text to send");
			return;
		}
	}

	std::string target = message.getParams(0);
	std::string messageText = message.getParams(1);

	size_t start = target.find_first_not_of(" \t");
	size_t end = target.find_last_not_of(" \t");
	if (start != std::string::npos && end != std::string::npos)
	{
		target = target.substr(start, end - start + 1);
	}

	if (target.empty() || messageText.empty())
	{
		Print::Fail("Empty target or message");
		sendErrorReply(client, 412, ":No text to send");
		return;
	}

	Print::Debug("PRIVMSG: '" + client->getNickname() + "' -> '" + target + "': '" +
				 messageText + "'");

	// Check if target is a channel (starts with # or &)
	if (target[0] == '#' || target[0] == '&')
	{
		handleChannelMessage(client, target, messageText);
	}
	else
	{
		if (isDccMessage(messageText))
		{
			sendDccNotify(client, target, messageText);
		}
		handlePrivateMessage(client, target, messageText);
	}
	Print::Ok("PRIVMSG command completed");
}

void PrivmsgCommand::handleChannelMessage(Client* sender, const std::string& channelName,
										  const std::string& message)
{
	Print::Debug("Handling channel message to: " + channelName);

	if (!isValidChannelName(channelName))
	{
		Print::Warn("Invalid channel name: " + channelName);
		sendErrorReply(sender, 403, channelName + " :No such channel");
		return;
	}

	Channel* channel = _server->getChannel(channelName);
	if (!channel)
	{
		Print::Warn("Channel does not exist: " + channelName);
		sendErrorReply(sender, 403, channelName + " :No such channel");
		return;
	}

	if (!channel->hasClient(sender))
	{
		Print::Warn("Sender not in channel: " + channelName);
		sendErrorReply(sender, 404, channelName + " :Cannot send to channel");
		return;
	}

	std::string broadcastMsg = createMessage(sender, "PRIVMSG", channelName, message);

	Print::Debug("Broadcasting to channel " + channelName + ": " + broadcastMsg);

	_server->broadcastChannel(broadcastMsg, channelName, sender->getFd());

	Print::Ok("Message sent to channel " + Color::YELLOW + channelName + Color::RESET +
			  " by " + Color::YELLOW + sender->getNickname() + Color::RESET);
}

void PrivmsgCommand::handlePrivateMessage(Client* sender, const std::string& targetNick,
										  const std::string& message)
{
	Print::Debug("Handling private message to: " + targetNick);

	if (!isValidNickname(targetNick))
	{
		Print::Warn("Invalid nickname: " + targetNick);
		sendErrorReply(sender, 401, targetNick + " :No such nick/channel");
		return;
	}

	Client* targetClient = _server->getClientByNick(targetNick);
	if (!targetClient)
	{
		Print::Warn("Target user not found: " + targetNick);
		sendErrorReply(sender, 401, targetNick + " :No such nick/channel");
		return;
	}

	if (targetClient == sender)
	{
		Print::Warn("User trying to send message to themselves");
		sendErrorReply(sender, 401, targetNick + " :No such nick/channel");
		return;
	}

	std::string privateMsg = createMessage(sender, "PRIVMSG", targetNick, message);

	Print::Debug("Sending private message: " + privateMsg);

	if (targetClient->sendMessage(privateMsg))
	{
		Print::Ok("Private message sent to " + Color::YELLOW + targetNick + Color::RESET +
				  " from " + Color::YELLOW + sender->getNickname() + Color::RESET);
	}
	else
	{
		Print::Fail("Failed to send private message to " + targetNick);
	}
}

std::string PrivmsgCommand::createMessage(Client* sender, const std::string& command,
										  const std::string& target,
										  const std::string& message)
{
	std::string result = ":" + sender->getNickname();

	std::string username = sender->getUsername();
	if (!username.empty())
	{
		result += "!" + username + "@localhost";
	}

	result += " " + command + " " + target + " :" + message + "\r\n";

	return result;
}

bool PrivmsgCommand::isDccMessage(const std::string& message)
{
	return (message.size() >= 4 && message.substr(0, 4) == "\001DCC");
}

void PrivmsgCommand::sendDccNotify(Client* sender, const std::string& target,
								   const std::string& message)
{
	std::string dccContent = message;
	if (dccContent[0] == '\001' && dccContent[dccContent.length() - 1] == '\001')
	{
		dccContent = dccContent.substr(1, dccContent.length() - 2);
	}

	std::vector<std::string> parts = splitArguments(dccContent, ' ');
	if (parts.size() >= 3 && parts[0] == "DCC" && parts[1] == "SEND")
	{
		Client* targetClient = _server->getClientByNick(target);
		if (targetClient)
		{
			std::string notification = ":server NOTICE " + target +
									   " :\002File Transfer Offer\002 - " +
									   sender->getNickname() +
									   " wants to send you: \002" + parts[2] + "\002\r\n";

			targetClient->sendMessage(notification);
		}
	}
}
