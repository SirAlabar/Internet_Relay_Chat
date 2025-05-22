#ifndef CAP_COMMAND_HPP
#define CAP_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class CapCommand : public ACommand
{
private:
	// Private to prevent copying
	CapCommand(const CapCommand& other);
	CapCommand& operator=(const CapCommand& other);

public:
	CapCommand(Server* server);
	virtual ~CapCommand();

	// Execute the CAP command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif