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
	Print::Do("execute PONG command");

	if (!client)
	{
		Print::Fail("Client NULL");
		return;
	}

	// PONG is a response to PING
	if (message.getSize() > 0)
	{
		Print::Debug("PONG received with token: " + message.getParams(0));
	}
	else
	{
		Print::Debug("PONG received without token");
	}
	
	Print::Ok("client is alive");
}