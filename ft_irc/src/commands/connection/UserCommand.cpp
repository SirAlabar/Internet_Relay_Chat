#include <ctime>

#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "UserCommand.hpp"
#include "UtilsFun.hpp"

UserCommand::UserCommand(Server* server) : ACommand(server) {}

UserCommand::~UserCommand() {}

// Private copy constructor
UserCommand::UserCommand(const UserCommand& other) : ACommand(other._server) {}

UserCommand& UserCommand::operator=(const UserCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* UserCommand::create(Server* server) { return (new UserCommand(server)); }

// Execute the USER command
void UserCommand::execute(Client* client, const Message& message)
{
	Print::Do("executing USER command");
	if (!client || !client->isAuthenticated())
	{
		Print::Fail("Client NULL or not auth");
		return;
	}

	if (!client->getUsername().empty())
	{
		Print::Warn("Already registered");
		sendErrorReply(client, 462, ":You may not reregister");
		return;
	}

	if (message.getSize() < 4 || (message.getParams(0)).empty())
	{
		Print::Fail("wrong message size or empty user");
		sendErrorReply(client, 461, "USER :Not enough parameters");
		return;
	}

	client->setUsername(message.getParams(0));
	Print::Debug("Username set to: '" + message.getParams(0) + "'");
	Print::Debug("Realname: '" + message.getParams(3) + "'");

	if (!client->getNickname().empty() && !client->getUsername().empty())
	{
		sendNumericReply(client, IRC::RPL_WELCOME,
						 ":Welcome to the IRC Network " + client->getNickname() + "!" +
							 client->getUsername() + "@localhost");
		sendNumericReply(client, IRC::RPL_YOURHOST, ":Host is server, running version 1.0");
		sendNumericReply(client, IRC::RPL_CREATED,
		   ":This server was created " + _server->getStartupTime());
		sendNumericReply(client, IRC::RPL_MYINFO,
		   "server ft_irc-1.0 o itkol");
		sendNumericReply(client, IRC::RPL_ISUPPORT,
		   "CHANTYPES=#& CHANMODES=itkol PREFIX=(o)@ :are supported by this server");
		{
			Message motdMessage("MOTD");
			MotdCommand motdCmd(_server);
			motdCmd.execute(client, motdMessage);
		}
		Print::Ok("Client registration done!");
	}
	else
	{
		Print::Warn("Registration not complete. Nick: '" + client->getNickname() +
					"', User: '" + client->getUsername() + "'");
	}
}
