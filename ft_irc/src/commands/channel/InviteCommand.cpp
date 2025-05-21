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
	// Future implementation
}