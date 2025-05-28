#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(const std::string& name)
    : _name(name),
    _topic(""),
    _inviteOnly(false),
    _topicRestricted(true),
    _hasKey(false),
    _key(""),
    _hasUserLimit(false),
    _userLimit(0) {}

Channel::~Channel() {}

const std::string& Channel::getName() const { return _name; }

const std::string& Channel::getTopic() const { return _topic; }

const std::map<int, Client*>& Channel::getClients() { return _clients; };

const std::map<int, Client*>& Channel::getOperators() { return _operators; };

void Channel::setTopic(const std::string& topic) { _topic = topic; }

void Channel::addClient(Client* client)
{
    if (client)
    {
        _clients[client->getFd()] = client;
    }
}

void Channel::addOperator(Client* _operator)
{
    if (_operator)
    {
        _operators[_operator->getFd()] = _operator;
    }
}

void Channel::removeClient(Client* client)
{
    if (client)
    {
        _clients.erase(client->getFd());
    }
}

void Channel::removeOperator(Client* client)
{
    if (client)
    {
        _operators.erase(client->getFd());
    }
}

bool Channel::isOperator(Client* client) const
{
    if (!client)
    {
        return false;
    }

    return (_operators.find(client->getFd()) != _operators.end());
}

bool Channel::hasClient(Client* client) const
{
    if (!client) 
    {
        return false;
    }

    return (_clients.find(client->getFd()) != _clients.end());
}

bool Channel::isEmpty() const { return _clients.empty(); }

// Mode management methods
bool Channel::isInviteOnly() const
{
    return _inviteOnly;
}

void Channel::setInviteOnly(bool inviteOnly)
{
    _inviteOnly = inviteOnly;
}

bool Channel::isTopicRestricted() const
{
    return _topicRestricted;
}

void Channel::setTopicRestricted(bool restricted)
{
    _topicRestricted = restricted;
}

bool Channel::hasKey() const
{
    return _hasKey;
}

const std::string& Channel::getKey() const
{
    return _key;
}

void Channel::setKey(const std::string& key)
{
    _key = key;
    _hasKey = true;
}

void Channel::removeKey()
{
    _key.clear();
    _hasKey = false;
}

bool Channel::hasUserLimit() const
{
    return _hasUserLimit;
}

size_t Channel::getUserLimit() const
{
    return _userLimit;
}

void Channel::setUserLimit(int limit)
{
    _userLimit = limit;
    _hasUserLimit = true;
}

void Channel::removeUserLimit()
{
    _userLimit = 0;
    _hasUserLimit = false;
}

void Channel::broadcast(const std::string& message, int excludeFd)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); 
         it != _clients.end(); ++it)
    {
        if (it->first != excludeFd && it->second)
        {
            it->second->sendMessage(message);
        }
    }
}
