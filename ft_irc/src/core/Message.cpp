#include <sstream>

#include "CommandFactory.hpp"
#include "Message.hpp"
#include "UtilsFun.hpp"

Message::Message(const std::string& rawMessage)
{
    std::istringstream message(rawMessage);
    message >> _command;

    std::getline(message, _params);
    if (!_params.empty() && _params[0] == ' ')
    {
        _params = _params.substr(1);
    }
    Print::Debug("Command: " + _command);
    Print::Debug("Params: " + _params);
}

Message::~Message() {}

const std::string& Message::getCommand() const { return _command; }

const std::string& Message::getParams() const { return _params; }

Message Message::parse(const std::string& rawMessage) { return Message(rawMessage); }
