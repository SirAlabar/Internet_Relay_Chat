#include "WhoCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

WhoCommand::WhoCommand(Server* server) : ACommand(server) {}

WhoCommand::~WhoCommand() {}

// Private copy constructor
WhoCommand::WhoCommand(const WhoCommand& other) : ACommand(other._server) {}

WhoCommand& WhoCommand::operator=(const WhoCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* WhoCommand::create(Server* server)
{
	return (new WhoCommand(server));
}

// Execute the WHO command
void WhoCommand::execute(Client* client, const Message& message)
{
	// Future implementation
}