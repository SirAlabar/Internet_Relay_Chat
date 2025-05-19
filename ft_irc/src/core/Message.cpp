#include "Message.hpp"
#include "UtilsFun.hpp"
#include <sstream>
#include <string>

Message::Message(const std::string &rawMessage) {
  Print::Log("rawMessage : " + rawMessage);
  std::istringstream message(rawMessage);
  message >> _command;
  std::string remainder;
  std::getline(message, remainder);
  parseParams(remainder);

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

const std::string &Message::getCommand() const { return _command; }

const std::vector<std::string> Message::getParams() const { return _params; }
const std::string Message::getParams(size_t i) const { return _params[i]; }
size_t Message::getSize() const { return _params.size(); }

std::vector<std::string> Message::parseParams(const std::string &rawMessage) {

  std::vector<std::string> result;
  std::istringstream iss(rawMessage);
  std::string temp;
  if (rawMessage.find(":") == rawMessage.npos) {
    while (iss >> temp)
      result.push_back(temp);
  } else {
    std::istringstream before(rawMessage.substr(0, rawMessage.find(':')));
    std::istringstream after(
        rawMessage.substr(rawMessage.find(':') + 1, rawMessage.length()));
    Print::Log("After: " + after.str());
    while (before >> temp)
      result.push_back(temp);
    std::getline(after, temp);
    Print::Log("temp->" + temp);
    if (temp.length() > 0)
      result.push_back(temp);
  }
  Print::Log(toString(result.size()));
  for (size_t i = 0; i < result.size(); i++) {
    Print::Log(result[i]);
  }
  return result;
}
