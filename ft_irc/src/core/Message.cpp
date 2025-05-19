#include <sstream>

#include "Message.hpp"
#include "UtilsFun.hpp"

std::map<std::string, Message::CommandHandler> Message::_commandHandlers;

void Message::initCommandHandler()
{
    if (_commandHandlers.empty())
    {
        _commandHandlers["JOIN"] = &Message::handleJOIN;
        _commandHandlers["TOPIC"] = &Message::handleTOPIC;
        _commandHandlers["KICK"] = &Message::handleKICK;
        _commandHandlers["INVITE"] = &Message::handleINVITE;
        _commandHandlers["MODE"] = &Message::handleMODE;
        _commandHandlers["HELP"] = &Message::handleHELP;
        _commandHandlers["NICK"] = &Message::handleNICK;
        _commandHandlers["USER"] = &Message::handleUSER;
        _commandHandlers["INVALID"] = &Message::handleINVALID;
    }
}

Message::Message(const std::string& rawMessage)
{
    initCommandHandler();
    // Simple parsing of IRC message
    std::istringstream message(rawMessage);

    // Check for prefix
    if (!message.str().empty() && message.str()[0] == ':')
    {
        size_t spacePos = message.find(' ');
        if (spacePos != std::string::npos)
        {
            _prefix = message.substr(1, spacePos - 1);
            message = message.substr(spacePos + 1);
        }
    }

    // Extract command
    size_t spacePos = message.find(' ');
    if (spacePos != std::string::npos)
    {
        _command = message.substr(0, spacePos);
        _params = message.substr(spacePos + 1);
    }
    else
    {
        _command = message;
    }
}

Message::~Message() {}

const std::string& Message::getPrefix() const { return _prefix; }

const std::string& Message::getCommand() const { return _command; }

const std::string& Message::getParams() const { return _params; }

Message Message::parse(const std::string& rawMessage) { return Message(rawMessage); }
