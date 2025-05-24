#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>

#include <map>
#include <string>
#include <vector>

#include "Socket.hpp"

// Forward declarations
class Client;
class Channel;
class Command;
class Message;

class Server
{
private:
    Socket _serverSocket;  // main server socket
    std::map<int, Socket*> _clientSockets;
    std::vector<pollfd> _pollFds;               // Array of pollfd structures for poll()
    std::map<int, Client*> _clients;            // Map of fds to client objects
    std::map<std::string, Channel*> _channels;  // Map of name to Channel objects
    std::string _password;                      // Server password
    bool _running;

    std::map<int, std::string> _clientBuffers;  // Buffer to store partial messagens by fd

    bool setupServer(int port, const std::string& password);
    void processNewConnection();
    void processClientMessage(int clientFd);
    void removeClient(int clientFd);

    Server(const Server& other);  // private to prevent copies
    Server& operator=(const Server& other);

public:
    Server();
    ~Server();

    // Initialization and execution
    bool start(int port, const std::string& password);
    void run();
    void stop();

    // Client management
    Client* getClient(int fd);
    Client* getClientByNick(const std::string& nickname);
    void broadcast(const std::string& message, int excludeFd = -1);
    void broadcastChannel(const std::string& message, const std::string& channel, int excludeFd = -1);

    // Channel management
    Channel* getChannel(const std::string& name);
    Channel* createChannel(const std::string& name, Client* creator);
    void addChannel(Channel* channel);
    void removeChannel(const std::string& name);
    std::map<std::string, Channel*>& getChannels();
    void cleanupEmptyChannels();

    // Other helper methods
    const std::string& getPassword() const;

    // utils for print data structures - only for understand what's in
    void print_clients();
};

#endif
