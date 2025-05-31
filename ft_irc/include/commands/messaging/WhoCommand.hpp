#ifndef WHO_COMMAND_HPP
#define WHO_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;
class Channel;

class WhoCommand : public ACommand
{
private:
	// Private to prevent copying
	WhoCommand(const WhoCommand& other);
	WhoCommand& operator=(const WhoCommand& other);

	void listChannelUsers(Client* client, const std::string& channelName);
	void listSpecificUser(Client* client, const std::string& nickname);
	void sendWhoReply(Client* client, Client* targetClient, const std::string& channel);
	
public:
	WhoCommand(Server* server);
	virtual ~WhoCommand();

	// Execute the WHO command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif