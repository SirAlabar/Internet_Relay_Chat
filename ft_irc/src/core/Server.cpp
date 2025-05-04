#include "Server.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <unistd.h>


Server::Server() : _running(false)
{}

Server::~Server()
{
    stop();
}

bool setupServer(int port, const std::string& password)
{
    _password = password;

    //Create the server socket
    if (!_serverSocket.create(AF_INET, SOCK_STEAM, 0))
    {
        std::cerr << "Error creating socket: " << _serverSocket.getLastError() << std::endl;
        return (false);
    }
    // Configure socket options
    int opt = 1;
    if (!_serverSocket.setOption(SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
    {
        std::cerr << "Error setting socket options: " << _serverSocket.getLastError() << std::endl;
        return (false);
    }
    // Set as non-blocking
    if (!_serverSocket.setNonBlocking()) 
    {
        std::cerr << "Error setting socket to non-blocking: " << _serverSocket.getLastError() << std::endl;
        return (false);
    }
    // Bind the socket to the port
    if (!_serverSocket.bind(port)) 
    {
        std::cerr << "Error binding: " << _serverSocket.getLastError() << std::endl;
        return (false);
    }
    // Listen for connections
    if (!_serverSocket.listen(10)) 
    {
        std::cerr << "Error listening: " << _serverSocket.getLastError() << std::endl;
        return (false);
    }
    
    std::cout << "IRC Server started on port " << port << std::endl;
    return (true);
}









void processNewConnection();
void processClientMessage(int clientFd);
void removeClient(int clientFd);

Server(const Server& other); // private to prevent copies
Server& operator=(const Server& other);




// Initialization and execution
bool start(int port, const std::string& password);
void run();
void stop();

// Client management
Client* getClient(int fd);
Client* getClientByNick(const std::string& nickname);
void broadcast(const std::string& message, int excludeFd = -1);

// Channel management
Channel* getChannel(const std::string& name);
Channel* createChannel(const std::string& name, Client* creator);
void removeChannel(const std::string& name);
std::map<std::string, Channel*>& getChannels();

// Other helper methods
const std::string& getPassword() const;
void executeCommand(Client* client, const Message& message);