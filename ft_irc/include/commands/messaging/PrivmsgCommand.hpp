#ifndef PRIVMSG_COMMAND_HPP
#define PRIVMSG_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class PrivmsgCommand : public ACommand
{
private:
	// Private to prevent copying
	PrivmsgCommand(const PrivmsgCommand& other);
	PrivmsgCommand& operator=(const PrivmsgCommand& other);

public:
	PrivmsgCommand(Server* server);
	virtual ~PrivmsgCommand();

	// Execute the PRIVMSG command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif