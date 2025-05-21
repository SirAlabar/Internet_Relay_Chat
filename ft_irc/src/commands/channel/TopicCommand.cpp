#include "TopicCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

TopicCommand::TopicCommand(Server* server) : ACommand(server) {}

TopicCommand::~TopicCommand() {}

// Private copy constructor
TopicCommand::TopicCommand(const TopicCommand& other) : ACommand(other._server) {}

TopicCommand& TopicCommand::operator=(const TopicCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* TopicCommand::create(Server* server)
{
	return (new TopicCommand(server));
}

// Execute the TOPIC command
void TopicCommand::execute(Client* client, const Message& message)
{
	// Future implementation
}