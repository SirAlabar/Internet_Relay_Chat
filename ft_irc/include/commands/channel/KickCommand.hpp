#ifndef KICK_COMMAND_HPP
#define KICK_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class KickCommand : public ACommand
{
private:
	// Private to prevent copying
	KickCommand(const KickCommand& other);
	KickCommand& operator=(const KickCommand& other);

public:
	KickCommand(Server* server);
	virtual ~KickCommand();

	// Execute the KICK command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif