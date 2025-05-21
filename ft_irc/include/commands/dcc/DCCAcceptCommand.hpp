#ifndef DCC_ACCEPT_COMMAND_HPP
#define DCC_ACCEPT_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;
class DCCManager;

class DCCAcceptCommand : public ACommand
{
private:
	// Private to prevent copying
	DCCAcceptCommand(const DCCAcceptCommand& other);
	DCCAcceptCommand& operator=(const DCCAcceptCommand& other);
	
	DCCManager* _dccManager;

public:
	DCCAcceptCommand(Server* server);
	virtual ~DCCAcceptCommand();

	// Execute the DCC ACCEPT command (via PRIVMSG)
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif