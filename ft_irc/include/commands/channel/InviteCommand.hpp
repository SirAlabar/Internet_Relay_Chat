#ifndef INVITE_COMMAND_HPP
#define INVITE_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;
class Channel;

class InviteCommand : public ACommand
{
private:
	// Private to prevent copying
	InviteCommand(const InviteCommand& other);
	InviteCommand& operator=(const InviteCommand& other);

	void executeInvite(Client* inviter, Client* target, Channel* channel);

public:
	InviteCommand(Server* server);
	virtual ~InviteCommand();

	// Execute the INVITE command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif