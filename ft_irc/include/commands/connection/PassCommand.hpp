#ifndef PASS_HPP
#define PASS_HPP

#include "ACommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "UtilsFun.hpp"

class Client;
class Server;
class Message;

class PassCommand : public ACommand
{
private:
	// Private to prevent copying
	PassCommand(const PassCommand& other);
	PassCommand& operator=(const PassCommand& other);

public:
	PassCommand(Server* server);
	virtual ~PassCommand();
	
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};



#endif
