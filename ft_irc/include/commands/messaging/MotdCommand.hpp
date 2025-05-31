#ifndef MOTD_COMMAND_HPP
#define MOTD_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class MotdCommand : public ACommand
{
private:
	MotdCommand(const MotdCommand& other);
	MotdCommand& operator=(const MotdCommand& other);

    void    sendLocalMessage(Client* client) const;

public:
	MotdCommand(Server* server);
	virtual ~MotdCommand();

	virtual void execute(Client* client, const Message& message);

	static ACommand* create(Server* server);
};

#endif
