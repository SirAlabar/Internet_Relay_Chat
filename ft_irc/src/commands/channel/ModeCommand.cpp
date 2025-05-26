#include "ModeCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "General.hpp"
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
		sendErrorReply(client, IRC::ERR_NOTREGISTERED, ":You have not registered");
		return;
	}

	// MODE requires at least 1 parameter (channel name)
	if (message.getSize() < 1 || message.getParams(0).empty())
	{
		Print::Fail("Not enough parameters");
		sendErrorReply(client, IRC::ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
		return;
	}

	std::string channelName = message.getParams(0);
	
	if (!isValidChannelName(channelName))
	{
		Print::Fail("Invalid channel name");
		sendErrorReply(client, IRC::ERR_NOSUCHCHANNEL, channelName + " :No such channel");
		return;
	}

	Channel* channel = _server->getChannel(channelName);
	if (!channel)
	{
		Print::Fail("Channel not found");
		sendErrorReply(client, IRC::ERR_NOSUCHCHANNEL, channelName + " :No such channel");
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
		sendErrorReply(client, IRC::ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
		return;
	}

	// Check if client is operator
	if (!channel->isOperator(client))
	{
		Print::Fail("Client not operator");
		sendErrorReply(client, IRC::ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
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
	bool adding = true;
	std::string appliedModes = "";
	std::string appliedParams = "";
	size_t paramIndex = 2;

	for (size_t i = 0; i < modeString.length(); ++i)
	{
		char mode = modeString[i];

		if (mode == '+')
		{
			adding = true;
			continue;
		}
		else if (mode == '-')
		{
			adding = false;
			continue;
		}

		// Process individual modes
		switch (mode)
		{
			case 'i': // Invite-only
				processInviteOnlyMode(channel, adding, appliedModes);
				break;

			case 't': // Topic restricted
				processTopicRestrictedMode(channel, adding, appliedModes);
				break;

			case 'k': // Channel key
				processChannelKeyMode(client, channel, adding, message, paramIndex, appliedModes, appliedParams);
				break;

			case 'o': // Operator privilege
				processOperatorMode(client, channel, adding, message, paramIndex, appliedModes, appliedParams);
				break;

			case 'l': // User limit
				processUserLimitMode(client, channel, adding, message, paramIndex, appliedModes, appliedParams);
				break;

			default:
				// Unknown mode
				sendErrorReply(client, IRC::ERR_UNKNOWNMODE, std::string(1, mode) + " :is unknown mode char to me");
				break;
		}
	}

	// Send mode change notification to all channel members
	if (!appliedModes.empty())
	{
		broadcastModeChange(client, channel, appliedModes, appliedParams);
	}
}

bool ModeCommand::processInviteOnlyMode(Channel* channel, bool adding, std::string& appliedModes)
{
	if (adding != channel->isInviteOnly())
	{
		channel->setInviteOnly(adding);
		
		if (adding)
		{
			appliedModes += "+";
			Print::Debug("Channel set to invite-only");
		}
		else
		{
			appliedModes += "-";
			Print::Debug("Channel removed from invite-only");
		}
		
		appliedModes += "i";
		return true;
	}
	return false;
}

bool ModeCommand::processTopicRestrictedMode(Channel* channel, bool adding, std::string& appliedModes)
{
	if (adding != channel->isTopicRestricted())
	{
		channel->setTopicRestricted(adding);
		
		if (adding)
		{
			appliedModes += "+";
			Print::Debug("Topic restriction enabled");
		}
		else
		{
			appliedModes += "-";
			Print::Debug("Topic restriction disabled");
		}
		
		appliedModes += "t";
		return true;
	}
	return false;
}

bool ModeCommand::processChannelKeyMode(Client* client, Channel* channel, bool adding,
									   const Message& message, size_t& paramIndex,
									   std::string& appliedModes, std::string& appliedParams)
{
	if (adding)
	{
		if (paramIndex < message.getSize() && !message.getParams(paramIndex).empty())
		{
			std::string key = message.getParams(paramIndex);
			channel->setKey(key);
			appliedModes += "+k";
			if (!appliedParams.empty()) 
			{
				appliedParams += " ";
			}
			appliedParams += key;
			paramIndex++;
			Print::Debug("Channel key set");
			return true;
		}
		else
		{
			sendErrorReply(client, IRC::ERR_NEEDMOREPARAMS, "MODE +k :Not enough parameters");
		}
	}
	else
	{
		if (channel->hasKey())
		{
			channel->removeKey();
			appliedModes += "-k";
			Print::Debug("Channel key removed");
			return true;
		}
	}
	return false;
}

bool ModeCommand::processOperatorMode(Client* client, Channel* channel, bool adding,
									 const Message& message, size_t& paramIndex,
									 std::string& appliedModes, std::string& appliedParams)
{
	if (paramIndex < message.getSize() && !message.getParams(paramIndex).empty())
	{
		std::string targetNick = message.getParams(paramIndex);
		Client* targetClient = _server->getClientByNick(targetNick);

		if (!targetClient)
		{
			sendErrorReply(client, IRC::ERR_NOSUCHNICK, targetNick + " :No such nick");
		}
		else if (!channel->hasClient(targetClient))
		{
			sendErrorReply(client, IRC::ERR_USERNOTINCHANNEL, targetNick + " " + channel->getName() +
			" :They aren't on that channel");
		}
		else
		{
			if (adding)
			{
				channel->addOperator(targetClient);
				Print::Debug("Added operator: " + targetNick);
			}
			else
			{
				channel->removeOperator(targetClient);
				Print::Debug("Removed operator: " + targetNick);
			}
			appliedModes += (adding ? "+" : "-");
			appliedModes += "o";
			if (!appliedParams.empty())
			{
				appliedParams += " ";
			}
			appliedParams += targetNick;
			paramIndex++;
			return true;
		}
		paramIndex++;
	}
	else
	{
		sendErrorReply(client, IRC::ERR_NEEDMOREPARAMS,
			std::string("MODE ") + (adding ? "+" : "-") + "o :Not enough parameters");
	}
	return false;
}

bool ModeCommand::processUserLimitMode(Client* client, Channel* channel, bool adding,
									  const Message& message, size_t& paramIndex,
									  std::string& appliedModes, std::string& appliedParams)
{
	if (adding)
	{
		if (paramIndex < message.getSize() && !message.getParams(paramIndex).empty())
		{
			std::string limitStr = message.getParams(paramIndex);
			int limit = atoi(limitStr.c_str());
			if (limit > 0)
			{
				channel->setUserLimit(limit);
				appliedModes += "+l";
				if (!appliedParams.empty()) 
				{
					appliedParams += " ";
				}
				appliedParams += limitStr;
				paramIndex++;
				Print::Debug("User limit set to: " + limitStr);
				return true;
			}
			else
			{
				sendErrorReply(client, 696, channel->getName() + " l * :Invalid limit");
			}
		}
		else
		{
			sendErrorReply(client, 461, "MODE +l :Not enough parameters");
		}
	}
	else
	{
		if (channel->hasUserLimit())
		{
			channel->removeUserLimit();
			appliedModes += "-l";
			Print::Debug("User limit removed");
			return true;
		}
	}
	return false;
}

void ModeCommand::broadcastModeChange(Client* client, Channel* channel,
									 const std::string& appliedModes, const std::string& appliedParams)
{
	std::string modeChange = ":" + client->getNickname() + " MODE " + 
							channel->getName() + " " + appliedModes;
	if (!appliedParams.empty())
	{
		modeChange += " " + appliedParams;
	}
	modeChange += "\r\n";

	channel->broadcast(modeChange, -1); // Broadcast to all members
	Print::Debug("Broadcasted mode change: " + appliedModes);
}