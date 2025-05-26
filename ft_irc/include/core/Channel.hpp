#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <string>

class Client;

class Channel
{
private:
    std::string _name;
    std::string _topic;
    std::map<int, Client*> _clients;
    std::map<int, Client*> _operators;

	bool _inviteOnly;
    bool _topicRestricted;
    bool _hasKey;
    std::string _key;
    bool _hasUserLimit;
    int _userLimit;

public:
    Channel(const std::string& name);
    ~Channel();

    // Getters
    const std::string& getName() const;
    const std::string& getTopic() const;
    const std::map<int, Client*>& getClients();
    const std::map<int, Client*>& getOperators();

    // Setters
    void setTopic(const std::string& topic);

    // Client management
    void addClient(Client* client);
    void removeClient(Client* client);
    bool hasClient(Client* client) const;

    // Operator management
    void addOperator(Client* _operator);
	void removeOperator(Client* client);
    bool isOperator(Client* client) const;

	// Mode management
    bool isInviteOnly() const;
    void setInviteOnly(bool inviteOnly);
    
    bool isTopicRestricted() const;
    void setTopicRestricted(bool restricted);
    
    bool hasKey() const;
    const std::string& getKey() const;
    void setKey(const std::string& key);
    void removeKey();
    
    bool hasUserLimit() const;
    int getUserLimit() const;
    void setUserLimit(int limit);
    void removeUserLimit();

    // Broadcasting
    void broadcast(const std::string& message, int excludeFd = -1);

    // Utility
    bool isEmpty() const;
};

#endif
