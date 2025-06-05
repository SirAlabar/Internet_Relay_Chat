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

	std::string reply = ":server 324 " + client->getNickname() + " " + 
						channel->getName() + " " + modes;
	if (!params.empty())
	{
		reply += " " + params;
	}
	reply += "\r\n";

	client->sendMessage(reply);
	Print::Debug("Sent channel modes: " + modes);
}

void ModeCommand::processModeChanges(Client* client, Channel* channel, 
									const std::string& modeString, const Message& message)
{
	bool adding = true;
	std::string addedModes = "";
	std::string removedModes = "";
	std::string appliedParams = "";
	size_t paramIndex = 2;

	Print::Debug("Starting mode processing: " + modeString);

	for (size_t i = 0; i < modeString.length(); ++i)
	{
		char mode = modeString[i];
		Print::Debug("Processing char: " + std::string(1, mode) + " (adding: " + toString(adding) + ")");

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
		bool modeChanged = false;
		switch (mode)
		{
			case 'i': // Invite-only
				modeChanged = processInviteOnlyMode(channel, adding);
				if (modeChanged)
				{
					if (adding)
						addedModes += "i";
					else
						removedModes += "i";
				}
				break;

			case 't': // Topic restricted
				modeChanged = processTopicRestrictedMode(channel, adding);
				if (modeChanged)
				{
					if (adding)
						addedModes += "t";
					else
						removedModes += "t";
				}
				break;

			case 'k': // Channel key
				modeChanged = processChannelKeyMode(client, channel, adding, message, paramIndex, appliedParams);
				if (modeChanged)
				{
					if (adding)
						addedModes += "k";
					else
						removedModes += "k";
				}
				break;

			case 'o': // Operator privilege
				modeChanged = processOperatorMode(client, channel, adding, message, paramIndex, appliedParams);
				if (modeChanged)
				{
					if (adding)
						addedModes += "o";
					else
						removedModes += "o";
				}
				break;

			case 'l': // User limit
				modeChanged = processUserLimitMode(client, channel, adding, message, paramIndex, appliedParams);
				if (modeChanged)
				{
					if (adding)
						addedModes += "l";
					else
						removedModes += "l";
				}
				break;

			default:
				Print::Warn("Unknown mode: " + std::string(1, mode));
				sendErrorReply(client, IRC::ERR_UNKNOWNMODE, std::string(1, mode) + " :is unknown mode char to me");
				break;
		}
	}

	// Build final mode string
	std::string finalModes = "";
	if (!addedModes.empty())
	{
		finalModes += "+" + addedModes;
	}
	if (!removedModes.empty())
	{
		finalModes += "-" + removedModes;
	}

	// Send mode change notification to all channel members
	if (!finalModes.empty())
	{
		Print::Debug("Final modes to broadcast: " + finalModes);
		Print::Debug("Params to broadcast: " + appliedParams);
		broadcastModeChange(client, channel, finalModes, appliedParams);
	}
	else
	{
		Print::Debug("No mode changes applied");
	}
}

bool ModeCommand::processInviteOnlyMode(Channel* channel, bool adding)
{
	if (adding != channel->isInviteOnly())
	{
		channel->setInviteOnly(adding);
		Print::Debug("Invite-only mode " + std::string(adding ? "enabled" : "disabled"));
		return true;
	}
	Print::Debug("Invite-only mode already " + std::string(adding ? "enabled" : "disabled"));
	return false;
}

bool ModeCommand::processTopicRestrictedMode(Channel* channel, bool adding)
{
	if (adding != channel->isTopicRestricted())
	{
		channel->setTopicRestricted(adding);
		Print::Debug("Topic restriction " + std::string(adding ? "enabled" : "disabled"));
		return true;
	}
	Print::Debug("Topic restriction already " + std::string(adding ? "enabled" : "disabled"));
	return false;
}

bool ModeCommand::processChannelKeyMode(Client* client, Channel* channel, bool adding,
									   const Message& message, size_t& paramIndex,
									   std::string& appliedParams)
{
	std::string key = "";
	if(paramIndex < message.getSize() && !message.getParams(paramIndex).empty())
	{
		key += message.getParams(paramIndex);
		paramIndex++;
	}
	else
	{
		std::string strMode = (adding ? "+k" : "-k");
		Print::Warn("MODE " + strMode + " requires a parameter");
		sendErrorReply(client, IRC::ERR_NEEDMOREPARAMS, "MODE " + strMode + " :Not enough parameters");
		return false;
	}

	if (adding)
	{
		channel->setKey(key);
		if (!appliedParams.empty())
		{
			appliedParams += " ";
		}
		appliedParams += key;
		Print::Ok("Channel key set to: " + key);
		return true;
	}
	else
	{
		if (!channel->hasKey())
		{
			Print::Warn("Channel has no key to remove");
			return false;
		}
		else if (key == channel->getKey())
		{
			channel->removeKey();
			Print::Ok("Channel key removed");
			return true;
		}
		else
		{
			Print::Warn("Wrong key for -k");
			sendErrorReply(client, IRC::ERR_KEYSET,
				channel->getName() + " :Key incorrect");
			return false;
		}
	}
}

bool ModeCommand::processOperatorMode(Client* client, Channel* channel, bool adding,
									 const Message& message, size_t& paramIndex,
									 std::string& appliedParams)
{
	if (paramIndex < message.getSize() && !message.getParams(paramIndex).empty())
	{
		std::string targetNick = message.getParams(paramIndex);
		Client* targetClient = _server->getClientByNick(targetNick);

		if (!targetClient)
		{
			Print::Warn("Target user not found: " + targetNick);
			sendErrorReply(client, IRC::ERR_NOSUCHNICK, targetNick + " :No such nick");
		}
		else if (!channel->hasClient(targetClient))
		{
			Print::Warn("Target user not in channel: " + targetNick);
			sendErrorReply(client, IRC::ERR_USERNOTINCHANNEL, targetNick + " " + channel->getName() +
			" :They aren't on that channel");
		}
		else
		{
			bool currentlyOp = channel->isOperator(targetClient);
			if (adding && !currentlyOp)
			{
				channel->addOperator(targetClient);
				Print::Debug("Added operator: " + targetNick);
			}
			else if (!adding && currentlyOp)
			{
				channel->removeOperator(targetClient);
				Print::Debug("Removed operator: " + targetNick);
			}
			else
			{
				Print::Debug("User " + targetNick + " is already " + (adding ? "operator" : "not operator"));
				paramIndex++;
				return false;
			}
			
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
		Print::Warn("MODE " + std::string(adding ? "+" : "-") + "o requires a parameter");
		sendErrorReply(client, IRC::ERR_NEEDMOREPARAMS,
			std::string("MODE ") + (adding ? "+" : "-") + "o :Not enough parameters");
	}
	return false;
}

bool ModeCommand::processUserLimitMode(Client* client, Channel* channel, bool adding,
									  const Message& message, size_t& paramIndex,
									  std::string& appliedParams)
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
				Print::Warn("Invalid user limit: " + limitStr);
				sendErrorReply(client, IRC::ERR_INVALIDLIMIT, channel->getName() + " l * :Invalid limit");
				paramIndex++;
			}
		}
		else
		{
			Print::Warn("MODE +l requires a parameter");
			sendErrorReply(client, IRC::ERR_NEEDMOREPARAMS, "MODE +l :Not enough parameters");
		}
	}
	else
	{
		if (channel->hasUserLimit())
		{
			channel->removeUserLimit();
			Print::Debug("User limit removed");
			return true;
		}
		Print::Debug("User limit already not set");
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

	Print::Debug("Broadcasting mode change: " + modeChange);
	channel->broadcast(modeChange, -1);
	Print::Ok("Mode change broadcasted");
}
