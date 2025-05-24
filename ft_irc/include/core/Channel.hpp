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
    bool isOperator(Client* client) const;

    // Utility
    bool isEmpty() const;
};

#endif
