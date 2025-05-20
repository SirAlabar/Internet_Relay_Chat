#include "PingCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "UtilsFun.hpp"

PingCommand::PingCommand(Server* server) : ACommand(server) {}

PingCommand::~PingCommand() {}

// Private copy constructor
PingCommand::PingCommand(const PingCommand& other) : ACommand(other._server) {}

PingCommand& PingCommand::operator=(const PingCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* PingCommand::create(Server* server)
{
	return (new PingCommand(server));
}

// Execute the PING command
void PingCommand::execute(Client* client, const Message& message)
{
	// Future implementation
}