#ifndef MODE_COMMAND_HPP
#define MODE_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;
class Channel;

class ModeCommand : public ACommand
{
private:
	// Private to prevent copying
	ModeCommand(const ModeCommand& other);
	ModeCommand& operator=(const ModeCommand& other);

	// Helper methods for mode processing
	void showChannelModes(Client* client, Channel* channel);
	void processModeChanges(Client* client, Channel* channel, 
						   const std::string& modeString, const Message& message);
	
	// Individual mode processors
	bool processInviteOnlyMode(Channel* channel, bool adding, std::string& appliedModes);
	bool processTopicRestrictedMode(Channel* channel, bool adding, std::string& appliedModes);
	bool processChannelKeyMode(Client* client, Channel* channel, bool adding, 
							   const Message& message, size_t& paramIndex, 
							   std::string& appliedModes, std::string& appliedParams);
	bool processOperatorMode(Client* client, Channel* channel, bool adding,
							const Message& message, size_t& paramIndex,
							std::string& appliedModes, std::string& appliedParams);
	bool processUserLimitMode(Client* client, Channel* channel, bool adding,
							 const Message& message, size_t& paramIndex,
							 std::string& appliedModes, std::string& appliedParams);
	void broadcastModeChange(Client* client, Channel* channel, 
							const std::string& appliedModes, const std::string& appliedParams);

public:
	ModeCommand(Server* server);
	virtual ~ModeCommand();

	// Execute the MODE command
	virtual void execute(Client* client, const Message& message);

	// Static creator for factory
	static ACommand* create(Server* server);
};

#endif