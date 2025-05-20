#include "ModeCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
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
	// Future implementation
}