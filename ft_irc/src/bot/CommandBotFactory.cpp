#include <iostream>

#include "ACommand.hpp"
#include "CapCommand.hpp"
#include "Client.hpp"
#include "CommandBotFactory.hpp"
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
    registerCommand("weather", &WeatherCommand::create);
    registerCommand("help", &HelpCommand::create);
    registerCommand("dadjokes", &DadJokesCommand::create);

    _initialized = true;
}

// Creates a command based on command name
ACommand *CommandBotFactory::createCommand(const std::string &commandName, Server *server)
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
void CommandBotFactory::executeCommand(Client *client, Server *server,
                                       const Message &message)
{
    std::string commandName = message.getCommand();
    Print::Debug("Attempting to execute command: " + commandName);

    ACommand *command = createCommand(commandName, server);

    if (command)
    {
        Print::Debug("Command created successfully, executing...");
        command->execute(client, message);
        Print::Debug("Command executed, cleaning up...");
        delete command;
    }
    else
    {
        Print::Debug("Command not found, sending error");
        // Send error to client about unknown command
        if (client)
        {
            std::string errorMsg = ":server 421 ";
            if (!client->getNickname().empty())
            {
                errorMsg += client->getNickname();
            }
            else
            {
                errorMsg += "*";
            }
            errorMsg += " " + commandName + " :Unknown command\r\n";
            client->sendMessage(errorMsg);
        }
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
