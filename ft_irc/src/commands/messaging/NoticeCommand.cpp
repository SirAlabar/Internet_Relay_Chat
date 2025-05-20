#include "NoticeCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

NoticeCommand::NoticeCommand(Server* server) : ACommand(server) {}

NoticeCommand::~NoticeCommand() {}

// Private copy constructor
NoticeCommand::NoticeCommand(const NoticeCommand& other) : ACommand(other._server) {}

NoticeCommand& NoticeCommand::operator=(const NoticeCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* NoticeCommand::create(Server* server)
{
	return (new NoticeCommand(server));
}

// Execute the NOTICE command
void NoticeCommand::execute(Client* client, const Message& message)
{
	// Future implementation
}