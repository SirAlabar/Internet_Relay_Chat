#ifndef PART_COMMAND_HPP
#define PART_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class PartCommand : public ACommand
{
private:
	// Private to prevent copying
	PartCommand(const PartCommand& other);
	PartCommand& operator=(const PartCommand& other);

public:
	PartCommand(Server* server);
	virtual ~PartCommand();

	// Execute the PART command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif