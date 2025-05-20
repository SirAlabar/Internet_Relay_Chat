#ifndef QUIT_COMMAND_HPP
#define QUIT_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class QuitCommand : public ACommand
{
private:
	// Private to prevent copying
	QuitCommand(const QuitCommand& other);
	QuitCommand& operator=(const QuitCommand& other);

public:
	QuitCommand(Server* server);
	virtual ~QuitCommand();

	// Execute the QUIT command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif