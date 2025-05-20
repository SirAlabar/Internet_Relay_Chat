#include "QuitCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "UtilsFun.hpp"

QuitCommand::QuitCommand(Server* server) : ACommand(server) {}

QuitCommand::~QuitCommand() {}

// Private copy constructor
QuitCommand::QuitCommand(const QuitCommand& other) : ACommand(other._server) {}

QuitCommand& QuitCommand::operator=(const QuitCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* QuitCommand::create(Server* server)
{
	return (new QuitCommand(server));
}

// Execute the QUIT command
void QuitCommand::execute(Client* client, const Message& message)
{
	// Future implementation
}