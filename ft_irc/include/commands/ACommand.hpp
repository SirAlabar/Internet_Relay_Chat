#ifndef ACOMMAND_HPP
#define ACOMMAND_HPP

#include <string>
#include <vector>

// Forward declarations
class Client;
class Server;
class Message;

class ACommand
{
protected:
    Server* _server;  // Reference to the IRC server

private:
    // Private to prevent copies
    ACommand(const ACommand& other);
    ACommand& operator=(const ACommand& other);

public:
    ACommand(Server* server);
    virtual ~ACommand();

    // The main execute method that all command implementations must override
    virtual void execute(Client* client, const Message& message) = 0;

    std::vector<std::string> splitArguments(const std::string& args,
					    char delimiter = ' ');
    bool isValidChannelName(const std::string& channelName);
    bool isValidNickname(const std::string& nickname);

    // Send replies to clients
    void sendReply(Client* client, const std::string& reply);
    void sendNumericReply(Client* client, int numeric, const std::string& message);
    void sendErrorReply(Client* client, int numeric, const std::string& message);
};

#endif
