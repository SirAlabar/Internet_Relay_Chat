#ifndef HELPCOMMAND_HPP
#define HELPCOMMAND_HPP

#include "ABotCommand.hpp"

class Client;
class Server;
class Message;
class BotContext;

class HelpCommand : public ABotCommand
{
private:
	// Private to prevent copying
	HelpCommand(const HelpCommand& other);
	HelpCommand& operator=(const HelpCommand& other);

	void showGeneralHelp(BotContext* botctx);
	void showSpecificHelp(BotContext* botctx, const std::string& command);

	void showWeatherHelp(BotContext* botctx);
	void showDadJokesHelp(BotContext* botctx);
	void showGameHelp(BotContext* botctx);
	void showHelpHelp(BotContext* botctx);

public:
	HelpCommand(Server* server);
	virtual ~HelpCommand();

	// Execute the HELP command
	virtual void execute(BotContext* botctx, std::string& message);

	// Static creator for factory
	static ABotCommand* create(Server* server);
};

#endif
