#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <vector>

class Message
{
private:
    std::string _command;
    std::string _remainder;
    std::vector<std::string> _params;

public:
    Message(const std::string& rawMessage);
    ~Message();

    const std::string& getCommand() const;
    const std::vector<std::string> getParams() const;
    const std::string getParams(size_t i) const;
    const std::string& getRemainder() const;
    size_t getSize() const;

    static std::vector<std::string> parseParams(const std::string& rawMessage);
};

#endif
