#ifndef COMMANDBOTFABotCTORY_HPP
#define COMMANDBOTFABotCTORY_HPP

#include <map>
#include <string>

// Forward declarations
class ABotCommand;
class Client;
class Server;
class Message;

class CommandBotFactory
{
private:
    // Map of command names to their corresponding creator functions
    typedef ABotCommand* (*CommandCreator)(Server* server);
    static std::map<std::string, CommandCreator> _commandCreators;

    // Private to prevent instantiation and copy
    CommandBotFactory();
    CommandBotFactory(const CommandBotFactory& other);
    CommandBotFactory& operator=(const CommandBotFactory& other);
    ~CommandBotFactory();

    static void initializeCommands();
    static bool _initialized;

public:
    // Create a command based on the message
    static ABotCommand* createCommand(const std::string& commandName, Server* server);
    // Execute the appropriate command based on the message
    static void executeCommand(const Message& message, Server* server = NULL);
    // Register a new command creator
    static void registerCommand(const std::string& commandName, CommandCreator creator);
    // Check if a command exists
    static bool commandExists(const std::string& commandName);
};

#endif
