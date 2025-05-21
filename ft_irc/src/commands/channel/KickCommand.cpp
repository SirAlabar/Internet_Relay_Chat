#include "KickCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "UtilsFun.hpp"

KickCommand::KickCommand(Server* server) : ACommand(server) {}

KickCommand::~KickCommand() {}

// Private copy constructor
KickCommand::KickCommand(const KickCommand& other) : ACommand(other._server) {}

KickCommand& KickCommand::operator=(const KickCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* KickCommand::create(Server* server)
{
	return (new KickCommand(server));
}

// Execute the KICK command
void KickCommand::execute(Client* client, const Message& message)
{
	// Future implementation
}