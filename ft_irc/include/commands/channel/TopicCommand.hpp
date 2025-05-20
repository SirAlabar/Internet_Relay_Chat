#ifndef TOPIC_COMMAND_HPP
#define TOPIC_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class TopicCommand : public ACommand
{
private:
	// Private to prevent copying
	TopicCommand(const TopicCommand& other);
	TopicCommand& operator=(const TopicCommand& other);

public:
	TopicCommand(Server* server);
	virtual ~TopicCommand();

	// Execute the TOPIC command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif