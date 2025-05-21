#include "ListCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

ListCommand::ListCommand(Server* server) : ACommand(server) {}

ListCommand::~ListCommand() {}

// Private copy constructor
ListCommand::ListCommand(const ListCommand& other) : ACommand(other._server) {}

ListCommand& ListCommand::operator=(const ListCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* ListCommand::create(Server* server)
{
	return (new ListCommand(server));
}

// Execute the LIST command
void ListCommand::execute(Client* client, const Message& message)
{
	// Future implementation
}