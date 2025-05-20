#ifndef MODE_COMMAND_HPP
#define MODE_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class ModeCommand : public ACommand
{
private:
	// Private to prevent copying
	ModeCommand(const ModeCommand& other);
	ModeCommand& operator=(const ModeCommand& other);

public:
	ModeCommand(Server* server);
	virtual ~ModeCommand();

	// Execute the MODE command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif