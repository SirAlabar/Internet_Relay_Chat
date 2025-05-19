#include <sstream>

#include "Message.hpp"
#include "UtilsFun.hpp"

Message::CommandMap Message::_commandHandlers;

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
        _commandHandlers["PRIVMSG"] = &Message::handlePRIVMSG;
        _commandHandlers["INVALID"] = &Message::handleINVALID;
        _commandHandlers["QUIT"] = &Message::handleQUIT;
        _commandHandlers["PART"] = &Message::handlePART;
        _commandHandlers["LIST"] = &Message::handleLIST;
        _commandHandlers["PASS"] = &Message::handlePASS;
        _commandHandlers["CAP"] = &Message::handleCAP;
        _commandHandlers["MOTD"] = &Message::handleMOTD;
        _commandHandlers["PING"] = &Message::handlePING;
        _commandHandlers["WHOIS"] = &Message::handleWHOIS;
        _commandHandlers["WHO"] = &Message::handleWHO;
        _commandHandlers["BOT"] = &Message::handleBOT;
    }
}

Message::Message(const std::string& rawMessage)
{
    initCommandHandler();
    std::istringstream message(rawMessage);
    message >> _command;

    std::getline(message, _params);
    if (!_params.empty() && _params[0] == ' ')
    {
        _params = _params.substr(1);
    }
}

Message::~Message() {}

const std::string& Message::getPrefix() const { return _prefix; }

const std::string& Message::getCommand() const { return _command; }

const std::string& Message::getParams() const { return _params; }

Message Message::parse(const std::string& rawMessage) { return Message(rawMessage); }
void Message::handleJOIN() {};
void Message::handleTOPIC() {};
void Message::handleKICK() {};
void Message::handleINVITE() {};
void Message::handleMODE() {};
void Message::handleHELP() {};
void Message::handleNICK() {};
void Message::handleUSER() {};
void Message::handlePRIVMSG() {};
void Message::handleQUIT() {};
void Message::handlePART() {};
void Message::handleLIST() {};
void Message::handlePASS() {};
void Message::handleCAP() {};
void Message::handleMOTD() {};
void Message::handlePING() {};
void Message::handleWHOIS() {};
void Message::handleWHO() {};
void Message::handleBOT() {};
void Message::handleINVALID() {};
