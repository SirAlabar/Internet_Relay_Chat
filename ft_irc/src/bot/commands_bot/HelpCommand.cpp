#include <iostream>

#include "BotContext.hpp"
#include "Client.hpp"
#include "HelpCommand.hpp"
#include "Message.hpp"
#include "Server.hpp"

HelpCommand::HelpCommand(Server* server) : ABotCommand(server) {}

HelpCommand::~HelpCommand() {}

// private, not used
HelpCommand::HelpCommand(const HelpCommand& other) : ABotCommand(other._server) {}

HelpCommand& HelpCommand::operator=(const HelpCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

// Static create method for factory
ABotCommand* HelpCommand::create(Server* server) { return (new HelpCommand(server)); }

// Execute the Help command
void HelpCommand::execute(BotContext* botctx, std::string& message)
{
	if (!botctx)
	{
		return;
	}
	Print::Do("executing Help command");

	std::string lowerArgs = message;
	for (size_t i = 0; i < lowerArgs.length(); ++i)
	{
		lowerArgs[i] = tolower(lowerArgs[i]);
	}

	if (lowerArgs.length() >= 4 && lowerArgs.substr(0, 4) == "help")
	{
		lowerArgs = lowerArgs.substr(4);
		
		// Trim leading whitespace
		while (!lowerArgs.empty() && (lowerArgs[0] == ' ' || lowerArgs[0] == '\t'))
		{
			lowerArgs.erase(0, 1);
		}
		
		// Trim trailing whitespace
		while (!lowerArgs.empty() && (lowerArgs[lowerArgs.length()-1] == ' ' 
				|| lowerArgs[lowerArgs.length()-1] == '\t'))
		{
			lowerArgs.erase(lowerArgs.length()-1);
		}
	}

	Print::Debug("[Help] Args (lowercase): '" + lowerArgs + "'");

	if (lowerArgs.empty())
	{
		showGeneralHelp(botctx);
	}
	else
	{
		showSpecificHelp(botctx, lowerArgs);
	}
}

void HelpCommand::showGeneralHelp(BotContext* botctx)
{
	botctx->reply("🤖 IRC Bot Help - Available Commands:");
	botctx->reply("══════════════════════════════════════");
	botctx->reply("!help [command]     - Show this help or help for specific command");
	botctx->reply("!weather <city>     - Get weather information for a city");
	botctx->reply("!dadjokes           - Get a random dad joke");
	botctx->reply("!game               - Play a \"game\" (surprise!)");
	botctx->reply("══════════════════════════════════════");
	botctx->reply("💡 Type !help <command> for detailed usage (e.g., !help weather)");
	botctx->reply("📝 Commands are case-insensitive: !HELP, !Help, !help all work");
}

void HelpCommand::showSpecificHelp(BotContext* botctx, const std::string& command)
{
	if (command == "weather")
	{
		showWeatherHelp(botctx);
	}
	else if (command == "dadjokes")
	{
		showDadJokesHelp(botctx);
	}
	else if (command == "game")
	{
		showGameHelp(botctx);
	}
	else if (command == "help")
	{
		showHelpHelp(botctx);
	}
	else
	{
		botctx->reply("❌ Unknown command: '" + command + "'");
		botctx->reply("💡 Type !help to see all available commands");
	}
}

void HelpCommand::showWeatherHelp(BotContext* botctx)
{
	botctx->reply("🌤️ Weather Command Help:");
	botctx->reply("═══════════════════════");
	botctx->reply("Usage: !weather <city> [format]");
	botctx->reply("");
	botctx->reply("📍 Basic usage:");
	botctx->reply("  !weather porto           - Simple weather info");
	botctx->reply("  !weather london          - Weather for London");
	botctx->reply("  !weather new york        - Multi-word cities work too");
	botctx->reply("");
	botctx->reply("📊 Available formats:");
	botctx->reply("  !weather porto detailed  - More detailed information");
	botctx->reply("  !weather porto full      - Complete weather report");
	botctx->reply("");
	botctx->reply("✨ Examples:");
	botctx->reply("  !weather tokyo");
	botctx->reply("  !weather são paulo detailed");
	botctx->reply("  !weather paris full");
	botctx->reply("");
	botctx->reply("ℹ️  Powered by wttr.in - supports cities worldwide!");
}


void HelpCommand::showDadJokesHelp(BotContext* botctx)
{
	botctx->reply("😄 Dad Jokes Command Help:");
	botctx->reply("════════════════════════");
	botctx->reply("Usage: !dadjokes");
	botctx->reply("");
	botctx->reply("📝 Description:");
	botctx->reply("  Get a random dad joke to brighten your day!");
	botctx->reply("  No parameters needed - just type the command.");
	botctx->reply("");
	botctx->reply("✨ Examples:");
	botctx->reply("  !dadjokes");
	botctx->reply("  !DADJOKES    (case doesn't matter)");
	botctx->reply("");
	botctx->reply("🎲 Features over 24 different jokes in rotation!");
}

void HelpCommand::showGameHelp(BotContext* botctx)
{
	botctx->reply("🎮 Game Command Help:");
	botctx->reply("═══════════════════");
	botctx->reply("Usage: !game");
	botctx->reply("");
	botctx->reply("📝 Description:");
	botctx->reply("  Play a special \"game\" - it's a surprise!");
	botctx->reply("  No parameters needed.");
	botctx->reply("");
	botctx->reply("✨ Examples:");
	botctx->reply("  !game");
	botctx->reply("  !GAME    (case doesn't matter)");
	botctx->reply("");
	botctx->reply("⚠️  Warning: This may or may not be what you expect... 😉");
}

void HelpCommand::showHelpHelp(BotContext* botctx)
{
	botctx->reply("❓ Help Command Help:");
	botctx->reply("═══════════════════");
	botctx->reply("Usage: !help [command]");
	botctx->reply("");
	botctx->reply("📝 Description:");
	botctx->reply("  Show general help or detailed help for a specific command.");
	botctx->reply("");
	botctx->reply("✨ Examples:");
	botctx->reply("  !help              - Show all available commands");
	botctx->reply("  !help weather      - Show detailed weather command help");
	botctx->reply("  !help dadjokes     - Show dad jokes command help");
	botctx->reply("  !help game         - Show game command help");
	botctx->reply("");
	botctx->reply("💡 All commands and parameters are case-insensitive!");
}