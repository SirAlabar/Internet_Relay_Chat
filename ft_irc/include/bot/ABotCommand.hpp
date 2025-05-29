#ifndef ABOTCOMMAND_HPP
#define ABOTCOMMAND_HPP

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "Bot.hpp"
#include "BotContext.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "General.hpp"
#include "Message.hpp"
#include "Server.hpp"

// Forward declarations
class Client;
class Server;
class Message;
class Channel;

class ABotCommand
{
protected:
    Server* _server;  // Reference to the IRC server

    // helpers, to make code less redudndant
    bool validateClient(Client* client) const;
    bool validateClientRegist(Client* client) const;
    bool validateParameterCount(Client* client, const Message& message, size_t minParams,
                                const std::string& commandName) const;
    Channel* validateAndGetChannel(Client* client, const ::std::string& channelName);
    bool validateChannelMembership(Client* client, Channel* channel,
                                   const std::string& channelName) const;

private:
    // Private to prevent copies
    ABotCommand(const ABotCommand& other);
    ABotCommand& operator=(const ABotCommand& other);

public:
    ABotCommand(Server* server);
    virtual ~ABotCommand();

    // The main execute method that all command implementations must override
    virtual void execute(BotContext* botctx, std::string& message) = 0;

    std::vector<std::string> splitArguments(const std::string& args,
                                            char delimiter = ' ');
    bool isValidChannelName(const std::string& channelName) const;
    bool isValidNickname(const std::string& nickname) const;

    // Send replies to clients
    void sendReply(Client* client, const std::string& reply) const;
    void sendNumericReply(Client* client, int numeric, const std::string& message) const;
    void sendErrorReply(Client* client, int numeric, const std::string& message) const;
};

#endif
