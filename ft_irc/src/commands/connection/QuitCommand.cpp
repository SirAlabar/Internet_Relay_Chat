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
	Print::Do("execute QUIT command");

	if (!client)
	{
		Print::Fail("Client NULL");
		return;
	}

	std::string quitMessage = "Client Quit";
	if (message.getSize() > 0 && !message.getParams(0).empty())
	{
		quitMessage = message.getParams(0);
		// Remove ':'
		if (quitMessage[0] == ':')
			quitMessage = quitMessage.substr(1);
	}

	Print::Debug("Quit message: " + quitMessage);

	// broadcast message
	std::string quitNotification = ":" + client->getNickname() + 
		"!" + client->getUsername() + "@localhost QUIT :" + quitMessage + "\r\n";

	Print::Debug("Broadcasting quit to channels");

	// Broadcast quit message
	_server->broadcast(quitNotification, client->getFd());

	std::string quitConfirmation = "ERROR :Closing Link: localhost (Quit: " + 
		quitMessage + ")\r\n";
	client->sendMessage(quitConfirmation);

	Print::Ok("Client quit processed");
}