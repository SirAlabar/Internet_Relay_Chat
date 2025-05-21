#ifndef DCC_SEND_COMMAND_HPP
#define DCC_SEND_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;
class DCCManager;

class DCCSendCommand : public ACommand
{
private:
	// Private to prevent copying
	DCCSendCommand(const DCCSendCommand& other);
	DCCSendCommand& operator=(const DCCSendCommand& other);
	
	DCCManager* _dccManager;

public:
	DCCSendCommand(Server* server);
	virtual ~DCCSendCommand();

	// Execute the DCC SEND command (via PRIVMSG)
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif