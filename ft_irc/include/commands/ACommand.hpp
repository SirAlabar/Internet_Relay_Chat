#ifndef ACOMMAND_HPP
#define ACOMMAND_HPP

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include "General.hpp"

#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"

// Forward declarations
class Client;
class Server;
class Message;
class Channel;

class ACommand
{
protected:
	Server* _server;  // Reference to the IRC server
	
	//helpers, to make code less redudndant
	bool    validateClient(Client* client) const;
	bool    validateClientRegist(Client* client) const;
	bool    validateParameterCount(Client* client, const Message& message,
								size_t minParams, const std::string& commandName) const;
	Channel* validateAndGetChannel(Client* client,
								const::std::string& channelName);
	bool    validateChannelMembership(Client* client, Channel* channel,
									const std::string& channelName) const;

private:
	// Private to prevent copies
	ACommand(const ACommand& other);
	ACommand& operator=(const ACommand& other);

public:
	ACommand(Server* server);
	virtual ~ACommand();

	// The main execute method that all command implementations must override
	virtual void execute(Client* client, const Message& message) = 0;

	std::vector<std::string> splitArguments(const std::string& args,
						char delimiter = ' ');
	bool isValidChannelName(const std::string& channelName) const;
	bool isValidNickname(const std::string& nickname) const;

	// Send replies to clients
	void sendReply(Client* client, const std::string& reply) const;
	void sendNumericReply(Client* client, int numeric, const std::string& message) const;
	void sendErrorReply(Client* client, int numeric, const std::string& message) const;
};

#endif
