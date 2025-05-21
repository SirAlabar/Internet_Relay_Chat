#ifndef PING_COMMAND_HPP
#define PING_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class PingCommand : public ACommand
{
private:
	// Private to prevent copying
	PingCommand(const PingCommand& other);
	PingCommand& operator=(const PingCommand& other);

public:
	PingCommand(Server* server);
	virtual ~PingCommand();

	// Execute the PING command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif