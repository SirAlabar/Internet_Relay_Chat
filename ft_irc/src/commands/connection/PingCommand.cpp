#include "PingCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "UtilsFun.hpp"

PingCommand::PingCommand(Server* server) : ACommand(server) {}

PingCommand::~PingCommand() {}

// Private copy constructor
PingCommand::PingCommand(const PingCommand& other) : ACommand(other._server) {}

PingCommand& PingCommand::operator=(const PingCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static creator for factory
ACommand* PingCommand::create(Server* server)
{
	return (new PingCommand(server));
}

// Execute the PING command
void PingCommand::execute(Client* client, const Message& message)
{
	Print::Do("execute PING command");

	if (!client)
	{
		Print::Fail("Client NULL");
		return;
	}

	// PING requires at least one parameter
	if (message.getSize() == 0)
	{
		Print::Fail("no parameters");
		sendErrorReply(client, 409, ":No origin specified");
		return;
	}

	// Get the ping token/server
	std::string token = message.getParams(0);
	Print::Debug("PING token: " + token);

	// Format: :server PONG server :token
	std::string pongReply = ":server PONG server :" + token + "\r\n";
	
	// Send PONG back to client
	if (client->sendMessage(pongReply))
	{
		Print::Ok("PONG sent");
	}
	else
	{
		Print::Fail("Failed to send PONG");
	}
}