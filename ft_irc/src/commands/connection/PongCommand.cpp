#include "PongCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "UtilsFun.hpp"

PongCommand::PongCommand(Server* server) : ACommand(server) {}

PongCommand::~PongCommand() {}

// Private copy constructor
PongCommand::PongCommand(const PongCommand& other) : ACommand(other._server) {}

PongCommand& PongCommand::operator=(const PongCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* PongCommand::create(Server* server)
{
	return (new PongCommand(server));
}

// Execute the PONG command
void PongCommand::execute(Client* client, const Message& message)
{
	// Future implementation
}