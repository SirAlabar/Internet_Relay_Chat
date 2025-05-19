#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <map>
#include <string>

class Message
{
private:
    std::string _prefix;
    std::string _command;
    std::string _params;
    typedef void (Message::*CommandFunctionPtr)();
    typedef std::map<std::string, Message::CommandFunctionPtr> CommandMap;

    static CommandMap _commandHandlers;
    static void initCommandHandler();

public:
    Message(const std::string& rawMessage);
    ~Message();

    const std::string& getPrefix() const;
    const std::string& getCommand() const;
    const std::string& getParams() const;
    void handleJOIN() {};
    void handleTOPIC() {};
    void handleKICK() {};
    void handleINVITE() {};
    void handleMODE() {};
    void handleHELP() {};
    void handleNICK() {};
    void handleUSER() {};
    void handleINVALID() {};

    static Message parse(const std::string& rawMessage);
};

#endif
