#ifndef WHOISCOMMAND_HPP
#define WHOISCOMMAND_HPP

#include "ACommand.hpp"
#include <ctime>

class Client;
class Server;
class Message;
class Channel;

class WhoIsCommand : public ACommand
{
private:
	// Private copy constructor and assignment operator
	WhoIsCommand(const WhoIsCommand& other);
	WhoIsCommand& operator=(const WhoIsCommand& other);

	void sendWhoIsInfo(Client* client, Client* targetClient);
	std::string getClientChannels(Client* targetClient);

public:
	WhoIsCommand(Server* server);
	virtual ~WhoIsCommand();

	// Execute the WHOIS command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif