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

	const std::string& getName() const;
	const std::string& getTopic() const;
	void setTopic(const std::string& topic);

	void addClient(Client* client);
	void removeClient(Client* client);
	bool isOperator(Client* client) const;
	std::map<int, Client*> getClients() const { return _clients; }
    bool hasClient(Client* client) const;
};

#endif
