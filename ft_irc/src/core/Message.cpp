#include <sstream>

#include "Message.hpp"

Message::Message(const std::string& rawMessage)
{
    // Simple parsing of IRC message
    std::string message = rawMessage;

    // Check for prefix
    if (!message.empty() && message[0] == ':')
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

Message Message::parse(const std::string& rawMessage)
{
    return Message(rawMessage);
}
