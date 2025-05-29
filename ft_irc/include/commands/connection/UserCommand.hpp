#ifndef USER_COMMAND_HPP
#define USER_COMMAND_HPP

#include "ACommand.hpp"
#include "MotdCommand.hpp"

class Client;
class Server;
class Message;

class UserCommand : public ACommand
{
private:
	// Private to prevent copying
	UserCommand(const UserCommand& other);
	UserCommand& operator=(const UserCommand& other);

public:
	UserCommand(Server* server);
	virtual ~UserCommand();

	// Execute the USER command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif
