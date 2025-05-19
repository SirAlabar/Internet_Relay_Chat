#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <map>
#include <string>

class Message
{
private:
    std::string _command;
    std::string _params;

public:
    Message(const std::string& rawMessage);
    ~Message();

    const std::string& getCommand() const;
    const std::string& getParams() const;

    static Message parse(const std::string& rawMessage);
};

#endif
