#ifndef DCC_RESUME_COMMAND_HPP
#define DCC_RESUME_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;
class DCCManager;

class DCCResumeCommand : public ACommand
{
private:
	// Private to prevent copying
	DCCResumeCommand(const DCCResumeCommand& other);
	DCCResumeCommand& operator=(const DCCResumeCommand& other);
	
	DCCManager* _dccManager;

public:
	DCCResumeCommand(Server* server);
	virtual ~DCCResumeCommand();

	// Execute the DCC RESUME command (via PRIVMSG)
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif