#include <iostream>

#include "BotContext.hpp"
#include "Client.hpp"
#include "GameCommand.hpp"
#include "Message.hpp"
#include "Server.hpp"

GameCommand::GameCommand(Server* server) : ABotCommand(server) {}

GameCommand::~GameCommand() {}

// private, not used
GameCommand::GameCommand(const GameCommand& other) : ABotCommand(other._server) {}

GameCommand& GameCommand::operator=(const GameCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static create method for factory
ABotCommand* GameCommand::create(Server* server) { return (new GameCommand(server)); }

// Execute the Game command
void GameCommand::execute(BotContext* botctx, std::string& message)
{
	if (!botctx)
	{
		return;
	}
	(void)message;
	botctx->reply("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
}
