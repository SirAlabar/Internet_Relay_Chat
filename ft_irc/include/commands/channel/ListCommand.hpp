#ifndef LISTCOMMAND_HPP
#define LISTCOMMAND_HPP

#include "ACommand.hpp"
#include <map>

class Server;
class Client;
class Message;
class Channel;

class ListCommand : public ACommand
{
private:
	// Private copy constructor and assignment operator to prevent copying
	ListCommand(const ListCommand& other);
	ListCommand& operator=(const ListCommand& other);

	// Helper methods
	void listAllChannels(Client* client, std::map<std::string, Channel*>& channels);
	void listSpecificChannels(Client* client, std::map<std::string, Channel*>& channels, const std::string& channelList);
	void sendChannelInfo(Client* client, Channel* channel);

public:
	ListCommand(Server* server);
	virtual ~ListCommand();

	// Execute the LIST command
	virtual void execute(Client* client, const Message& message);

	// Static creator method for CommandFactory
	static ACommand* create(Server* server);
};

#endif