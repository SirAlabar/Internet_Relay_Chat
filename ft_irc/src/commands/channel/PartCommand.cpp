#include "PartCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

PartCommand::PartCommand(Server* server) : ACommand(server) {}

PartCommand::~PartCommand() {}

// Private copy constructor
PartCommand::PartCommand(const PartCommand& other) : ACommand(other._server) {}

PartCommand& PartCommand::operator=(const PartCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* PartCommand::create(Server* server)
{
	return (new PartCommand(server));
}

// Execute the PART command
void PartCommand::execute(Client* client, const Message& message)
{
	// Future implementation
}