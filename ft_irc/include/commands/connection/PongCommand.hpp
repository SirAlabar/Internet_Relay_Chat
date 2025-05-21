#ifndef PONG_COMMAND_HPP
#define PONG_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class PongCommand : public ACommand
{
private:
	// Private to prevent copying
	PongCommand(const PongCommand& other);
	PongCommand& operator=(const PongCommand& other);

public:
	PongCommand(Server* server);
	virtual ~PongCommand();

	// Execute the PONG command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif