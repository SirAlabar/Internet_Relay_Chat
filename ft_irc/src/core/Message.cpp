#include <sstream>
#include <string>

#include "CommandFactory.hpp"
#include "Message.hpp"
#include "UtilsFun.hpp"

Message::Message(const std::string& rawMessage)
{
    std::istringstream message(rawMessage);
    message >> _command;
    std::string remainder;
    std::getline(message, remainder);
    _params = parseParams(remainder);

    Print::Debug("Command: " + _command);
    Print::Debug("_params: ");
    for (size_t i = 0; i < _params.size(); i++)
    {
        Print::Debug(_params[i]);
    }
    Print::Debug("Command: " + _command);
    Print::Debug("Params: " + _params);
}

Message::~Message() {}

const std::string& Message::getCommand() const { return _command; }

const std::vector<std::string> Message::getParams() const { return _params; }
const std::string Message::getParams(size_t i) const
{
    if (i < _params.size())
        return _params[i];
    else
        return "";
}

size_t Message::getSize() const { return _params.size(); }

std::vector<std::string> Message::parseParams(const std::string& rawMessage)
{
    std::vector<std::string> result;
    std::istringstream iss(rawMessage);
    std::string temp;
    if (rawMessage.find(":") == rawMessage.npos)
    {
        while (iss >> temp) result.push_back(temp);
    }
    else
    {
        std::istringstream before(rawMessage.substr(0, rawMessage.find(':')));
        std::istringstream after(
            rawMessage.substr(rawMessage.find(':') + 1, rawMessage.length()));
        while (before >> temp) result.push_back(temp);
        std::getline(after, temp);
        if (temp.length() > 0) result.push_back(temp);
    }
    return result;
}
