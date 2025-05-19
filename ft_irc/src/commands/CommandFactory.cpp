#include "commands/CommandFactory.hpp"
// #include "commands/Command.hpp"
#include <iostream>

#include "core/Client.hpp"
#include "core/Message.hpp"
#include "core/Server.hpp"

// Initialize static members
std::map<std::string, CommandFactory::CommandCreator> CommandFactory::_commandCreators;
bool CommandFactory::_initialized = false;

// Registers all available commands
void CommandFactory::initializeCommands()
{
    if (_initialized)
    {
        return;
    }

    // // Channel commands
    // registerCommand("INVITE", &InviteCommand::create);
    // registerCommand("JOIN", &JoinCommand::create);
    // registerCommand("KICK", &KickCommand::create);
    // registerCommand("MODE", &ModeCommand::create);
    // registerCommand("PART", &PartCommand::create);
    // registerCommand("TOPIC", &TopicCommand::create);

    // // Connection commands
    // registerCommand("NICK", &NickCommand::create);
    // registerCommand("PASS", &PassCommand::create);
    // registerCommand("QUIT", &QuitCommand::create);
    // registerCommand("USER", &UserCommand::create);

    // // Messaging commands
    // registerCommand("NOTICE", &NoticeCommand::create);
    // registerCommand("PRIVMSG", &PrivmsgCommand::create);
    // registerCommand("WHO", &WhoCommand::create);

    // // DCC commands (bonus part)
    // registerCommand("DCC", &DCCCommand::create);
    // registerCommand("DCCSEND", &DCCSendCommand::create);
    // registerCommand("DCCACCEPT", &DCCAcceptCommand::create);

    _initialized = true;
}

// Register a new command creator
void CommandFactory::registerCommand(const std::string& commandName,
                                     CommandCreator creator)
{
    std::string upperCommandName = commandName;
    for (size_t i = 0; i < upperCommandName.size(); ++i)
    {
        upperCommandName[i] = toupper(upperCommandName[i]);
    }
    _commandCreators[upperCommandName] = creator;
}

// Creates a command based on command name
Command* CommandFactory::createCommand(const std::string& commandName, Server* server)
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

    // If found, create and return the command
    if (it != _commandCreators.end())
    {
        return (it->second(server));  // return command creator
    }
    Print::Log("Unknown command: " + commandName);
    return (NULL);
}

// Execute the appropriate command based on the message
void CommandFactory::executeCommand(Client* client, Server* server,
                                    const Message& message)
{
    (void)server, (void)client, (void)message;
    std::string commandName = message.getCommand();

    Command* command = createCommand(commandName, server);

    if (command)
    {
        Print::Log("Executing :" + message.getCommand() + "for client" +
                   client->getFdString());
        // command->execute(client, message);
        // delete command;
    }
    else
    {
        Print::StdErr("Command not found: " + message.getCommand());
    }
}

// Check if a command exists
bool CommandFactory::commandExists(const std::string& commandName)
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
