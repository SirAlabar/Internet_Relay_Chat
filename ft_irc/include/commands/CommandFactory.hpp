#ifndef COMMANDFACTORY_HPP
#define COMMANDFACTORY_HPP

#include <map>
#include <string>

// Forward declarations
class ACommand;
class Client;
class Server;
class Message;

class CommandFactory
{
private:
    // Map of command names to their corresponding creator functions
    typedef ACommand* (*CommandCreator)(Server* server);
    static std::map<std::string, CommandCreator> _commandCreators;

    // Private to prevent instantiation and copy
    CommandFactory();
    CommandFactory(const CommandFactory& other);
    CommandFactory& operator=(const CommandFactory& other);
    ~CommandFactory();

    static void initializeCommands();
    static bool _initialized;

public:
    // Register a new command creator
    static void registerCommand(const std::string& commandName, CommandCreator creator);
    // Create a command based on the message
    static ACommand* createCommand(const std::string& commandName, Server* server);
    // Execute the appropriate command based on the message
    static void executeCommand(Client* client, Server* server, const Message& message);
    // Check if a command exists
    static bool commandExists(const std::string& commandName);
};

#endif
