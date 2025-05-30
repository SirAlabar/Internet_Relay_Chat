#include <cstddef>
#include <filesystem>
#include <iostream>
#include <iterator>

#include "ABotCommand.hpp"
#include "BotContext.hpp"
#include "CapCommand.hpp"
#include "GameCommand.hpp"
#include "Client.hpp"
#include "CommandBotFactory.hpp"
#include "Message.hpp"
#include "DadJokesCommand.hpp"
#include "HelpCommand.hpp"
#include "WeatherCommand.hpp"

// Initialize static members
std::map<std::string, CommandBotFactory::CommandCreator>
    CommandBotFactory::_commandCreators;
bool CommandBotFactory::_initialized = false;

// Registers all available commands
void CommandBotFactory::initializeCommands()
{
    if (_initialized)
    {
        return;
    }

    // Channel commands
    registerCommand("WEATHER", &WeatherCommand::create);
    registerCommand("HELP", &HelpCommand::create);
    registerCommand("DADJOKES", &DadJokesCommand::create);
    registerCommand("GAME", &GameCommand::create);

    _initialized = true;
}

// Creates a command based on command name
ABotCommand *CommandBotFactory::createCommand(const std::string &commandName,
                                              Server *server)
{
    if (!_initialized)
    {
        initializeCommands();
    }

    // Convert command to uppercase for case-insensitive comparison
    std::string upperCommandName = commandName;
    for (size_t i = 0; i < upperCommandName.size(); ++i)
    {
        upperCommandName[i] = toupper(upperCommandName[i]);
    }

    // Look up the command in the map
    std::map<std::string, CommandCreator>::iterator it =
        _commandCreators.find(upperCommandName);

    Print::Debug("Looking for command '" + upperCommandName +
                 "': " + (it != _commandCreators.end() ? "Found" : "Not found"));
    // If found, create and return the command
    if (it != _commandCreators.end())
    {
        return (it->second(server));  // return command creator
    }
    Print::Warn("Unknown command: " + commandName);
    return (NULL);
}

// Execute the appropriate command based on the message
void CommandBotFactory::executeCommand(const Message &rawMessage, Bot *bot)
{
    // :joao!joao-pol@localhost PRIVMSG #penis :!hello
    if (rawMessage.getSize() < 3 || rawMessage.getParams(0) != "PRIVMSG" ||
        rawMessage.getParams(2)[0] != '!')
    {
        return;
    }
    std::string channel = rawMessage.getParams(1);
    std::string commandName = rawMessage.getParams(2).substr(1);
    size_t spacePos = commandName.find(' ');
    if (spacePos != std::string::npos) commandName = commandName.substr(0, spacePos);
    Print::Debug("[BOT] Attempting to execute command: " + commandName);
    Print::Debug("[BOT] Command plus args: " + rawMessage.getParams(2).substr(1));

    ABotCommand *command = createCommand(commandName, NULL);

    if (command)
    {
        BotContext botctx(bot, channel, commandName);

        Print::Debug("Command created successfully, executing...");
        std::string botmsg = rawMessage.getRemainder();
        botmsg = botmsg.substr(botmsg.find('!') + 1);
        command->execute(&botctx, botmsg);
        Print::Debug("Command executed, cleaning up...");
        delete command;
    }
    else
    {
        Print::Debug("Command not found, sending error");
    }
}

// Register a new command creator
void CommandBotFactory::registerCommand(const std::string &commandName,
                                        CommandCreator creator)
{
    std::string upperCommandName = commandName;
    for (size_t i = 0; i < upperCommandName.size(); ++i)
    {
        upperCommandName[i] = toupper(upperCommandName[i]);
    }
    _commandCreators[upperCommandName] = creator;
}

// Check if a command exists
bool CommandBotFactory::commandExists(const std::string &commandName)
{
    if (!_initialized)
    {
        initializeCommands();
    }
    std::string upperCommandName = commandName;
    for (size_t i = 0; i < upperCommandName.size(); ++i)
    {
        upperCommandName[i] = toupper(upperCommandName[i]);
    }

    return (_commandCreators.find(upperCommandName) != _commandCreators.end());
}
