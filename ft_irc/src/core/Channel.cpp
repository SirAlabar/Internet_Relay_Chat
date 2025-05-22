#include "Channel.hpp"
#include "Client.hpp"
#include "UtilsFun.hpp"

Channel::Channel(const std::string& name) : _name(name) {}

Channel::~Channel() {}

const std::string& Channel::getName() const { return _name; }

const std::string& Channel::getTopic() const { return _topic; }

void Channel::setTopic(const std::string& topic) { _topic = topic; }

void Channel::addClient(Client* client)
{
    Print::Debug("Map");
    for (std::map<int, Client*>::iterator ite = _clients.begin(); ite != _clients.end();
         ite++)
    {
        Print::Debug(toString(ite->first) + " " + ite->second->getUsername());
    }
    if (client) _clients[client->getFd()] = client;
}

void Channel::addOperator(Client* _operator)
{
    if (_operator) _clients[_operator->getFd()] = _operator;
}

void Channel::removeClient(Client* client)
{
    if (client) _clients.erase(client->getFd());
}

bool Channel::isOperator(Client* client) const
{
    if (!client) return false;

    return _operators.find(client->getFd()) != _operators.end();
}
