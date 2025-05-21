#ifndef NOTICE_COMMAND_HPP
#define NOTICE_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class NoticeCommand : public ACommand
{
private:
	// Private to prevent copying
	NoticeCommand(const NoticeCommand& other);
	NoticeCommand& operator=(const NoticeCommand& other);

public:
	NoticeCommand(Server* server);
	virtual ~NoticeCommand();

	// Execute the NOTICE command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif