#include "JoinCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

JoinCommand::JoinCommand(Server* server) : ACommand(server) {}

JoinCommand::~JoinCommand() {}

// Private copy constructor
JoinCommand::JoinCommand(const JoinCommand& other) : ACommand(other._server) {}

JoinCommand& JoinCommand::operator=(const JoinCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* JoinCommand::create(Server* server)
{
	return (new JoinCommand(server));
}

// Execute the JOIN command
void JoinCommand::execute(Client* client, const Message& message)
{
	// Future implementation
}