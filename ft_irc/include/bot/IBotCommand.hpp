#ifndef BOTCOMMAND_HPP
#define BOTCOMMAND_HPP

#include <string>

#include "Bot.hpp"
class Bot;

class BotCommand
{
public:
	virtual ~BotCommand() {}
	virtual void execute(const std::string& channel, const std::string& user,
						const std::string& args, Bot* bot) = 0;
	virtual std::string getHelp() const = 0;
};

// Specific commands
class HelpCommand : public BotCommand
{
};
class JokesCommand : public BotCommand
{
};
class WeatherCommand : public BotCommand
{
};
#endif  // !BOTCOMMAND_HPP
