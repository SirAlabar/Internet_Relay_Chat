#ifndef NOTICE_COMMAND_HPP
#define NOTICE_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;
class Channel;

class NoticeCommand : public ACommand
{
private:
	// Private to prevent copying
	NoticeCommand(const NoticeCommand& other);
	NoticeCommand& operator=(const NoticeCommand& other);

    void sendNoticeToChannel(Client* sender, const std::string& channelName, const std::string& notice);
    void sendNoticeToUser(Client* sender, const std::string& targetNick, const std::string& notice);
	void broadcastToChannel(Channel* channel, const std::string& message, int excludeFd);

public:
	NoticeCommand(Server* server);
	virtual ~NoticeCommand();

	// Execute the NOTICE command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif