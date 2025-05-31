#ifndef GAMECOMMAND_HPP
#define GAMECOMMAND_HPP

#include "ABotCommand.hpp"

class Client;
class Server;
class Message;

class GameCommand : public ABotCommand
{
private:
	// Private to prevent copying
	GameCommand(const GameCommand& other);
	GameCommand& operator=(const GameCommand& other);

public:
	GameCommand(Server* server);
	virtual ~GameCommand();

	// Execute the HELP command
	virtual void execute(BotContext* botctx, std::string& message);

	// Static creator for factory
	static ABotCommand* create(Server* server);
};

#endif
