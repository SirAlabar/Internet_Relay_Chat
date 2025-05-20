#include "UserCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "UtilsFun.hpp"
#include <ctime>

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
ACommand* UserCommand::create(Server* server)
{
	return (new UserCommand(server));
}

// Execute the USER command
void UserCommand::execute(Client* client, const Message& message)
{
	Print::Debug("Starting execution of USER command");
  
	if (!client)
	{
		Print::Debug("Client is NULL, returning");
		return;
	}

	// Check if client already has a username
	if (!client->getUsername().empty())
	{
		sendErrorReply(client, 462, ":You may not reregister");
		return;
	}


	/////parser commands


}