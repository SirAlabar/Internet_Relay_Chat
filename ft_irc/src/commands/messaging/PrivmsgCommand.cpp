#include "PrivmsgCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
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
ACommand* PrivmsgCommand::create(Server* server)
{
	return (new PrivmsgCommand(server));
}

// Execute the PRIVMSG command
void PrivmsgCommand::execute(Client* client, const Message& message)
{
	// Future implementation
}