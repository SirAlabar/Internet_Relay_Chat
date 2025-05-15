#include "Server.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "Client.hpp"


Server::Server() : _running(false)
{}

Server::~Server()
{
    stop();
}

// Setup server with port and password
bool Server::setupServer(int port, const std::string& password)
{
    _password = password;

    //Create the server socket
    if (!_serverSocket.create(AF_INET, SOCK_STREAM, 0))
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

// Start the server on specified port with password
bool Server::start(int port, const std::string& password)
{
    if (!setupServer(port, password))
    {
        return (false);
    }

    pollfd serverPollFd;
    serverPollFd.fd = _serverSocket.getFd();
    serverPollFd.events = POLLIN;
    serverPollFd.revents = 0;

    _pollFds.push_back(serverPollFd);
    _running = true;

    return (true);
}

// Main server loop - handles events using poll()
void Server::run()
{
    while (_running)
    {
        // Execute poll() to check for events
        int ready = poll(_pollFds.data(), _pollFds.size(), -1);
        if (ready < 0)
        {
            if (errno == EINTR)
                continue;
            
            std::cerr << "Error in poll: " << strerror(errno) << std::endl;
            break;
        }
        // Process all fds with events
        for (size_t i = 0; i < _pollFds.size() && ready > 0; ++i) 
        {
            if (_pollFds[i].revents == 0)
                continue;
            ready--;
            // Check if we have a new connection on the server socket
            if (_pollFds[i].fd == _serverSocket.getFd() && (_pollFds[i].revents & POLLIN)) 
            {
                processNewConnection();
            }
            // Process messages from existing clients
            else if (_pollFds[i].revents & POLLIN)
            {
                processClientMessage(_pollFds[i].fd);
            }
            // Handle disconnections
            else if (_pollFds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
                removeClient(_pollFds[i].fd);
            }
        }
    }
}


// Stop the server and clean up resources
void Server::stop()
{
    _running = false;

    // Clean up clients
    // for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    // {
    //     close(it->first);
    //     delete it->second;
    // }
    // _clients.clear();
    // // Clean up channels
    // for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) 
    // {
    //     delete it->second;
    // }
    _channels.clear();
    // Clean up poll fds
    _pollFds.clear();
    // Close server socket
    _serverSocket.close();
    std::cout << "IRC Server shutdown" << std::endl;
}

// Process a new client connection
void Server::processNewConnection()
{
    Socket clientSocket = _serverSocket.accept();
    if (!clientSocket.isValid()) 
    {
        std::cerr << "Error accepting connection: " << _serverSocket.getLastError() << std::endl;
        return ;
    }
    // Set client socket as non-blocking
    clientSocket.setNonBlocking();
    int clientFd = clientSocket.getFd();
    // Add to poll
    pollfd clientPollFd;
    clientPollFd.fd = clientFd;
    clientPollFd.events = POLLIN;  // Monitor for read
    clientPollFd.revents = 0;
    _pollFds.push_back(clientPollFd);

    // Create Client object and add to map
    Client* client = new Client(clientFd);
    _clients[clientFd] = client;
    // implement clients functions
   
    std::cout << "New connection accepted. FD: " << clientFd << std::endl;
}

// void Server::processClientMessage(int clientFd)
// {
//     //implement after cliente
// }

void Server::processClientMessage(int clientFd)
{
    Client* client = getClient(clientFd);
    if (!client)
    {
        std::cerr << "Error: Client not found for FD: " << clientFd << std::endl;
        return;
    }
    
    // Buffer for receiving data
    char buffer[1024];
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRead <= 0)
    {
        if (bytesRead == 0 || errno != EAGAIN)
        {
            // Connection closed or error
            removeClient(clientFd);
        }
        return;
    }
    
    // Null-terminate the received data
    buffer[bytesRead] = '\0';
    
    // Append to client buffer
    _clientBuffers[clientFd] += buffer;
    
    // Process complete messages (ending with \r\n)
    std::string& clientBuffer = _clientBuffers[clientFd];
    size_t pos;
    
    while ((pos = clientBuffer.find("\r\n")) != std::string::npos)
    {
        // Extract one complete message
        std::string rawMessage = clientBuffer.substr(0, pos);
        clientBuffer.erase(0, pos + 2);  // Remove processed message
        
        // For debugging: print received message
        std::cout << "Received from client " << clientFd << ": " << rawMessage << std::endl;
        
        // Basic echo response for testing
        std::string response = ":" + std::string("server") + " NOTICE * :Echo: " + rawMessage + "\r\n";
        client->sendMessage(response);
    }
}


// Get server password
const std::string& Server::getPassword() const
{
    return (_password);
}

// Get all channels
std::map<std::string, Channel*>& Server::getChannels() 
{
    return (_channels);
}

// Get channel by name
Channel* Server::getChannel(const std::string& name) 
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end())
    {
        return (it->second);
    }
    return (NULL);
}

// Get client by file descriptor
Client* Server::getClient(int fd) 
{
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if (it != _clients.end())
    {
        return (it->second);
    }
    return (NULL);
}

// // Get client by nickname
// Client* Server::getClientByNick(const std::string& nickname) 
// {
//     //implement after channel
// }

Client* Server::getClientByNick(const std::string& nickname)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second->getNickname() == nickname)
        {
            return it->second;
        }
    }
    return NULL;
}


// Remove channel by name
void Server::removeChannel(const std::string& name) 
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end()) 
    {
        delete it->second;
        _channels.erase(it);
    }
}

// Remove client and cleanup associated resources
void Server::removeClient(int clientFd) 
{
    // Remove from poll
    for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) 
    {
        if (it->fd == clientFd) 
        {
            _pollFds.erase(it);
            break;
        }
    }
    // Clear buffer
    _clientBuffers.erase(clientFd);
    // Remove client
    if (_clients.find(clientFd) != _clients.end()) 
    {
        // implement after channel <<<<<<<<<<<<
        delete _clients[clientFd];
        _clients.erase(clientFd);
    }
    
    close(clientFd);
    std::cout << "Client disconnected. FD: " << clientFd << std::endl;
}

// Broadcast message to all clients except excludeFd
void Server::broadcast(const std::string& message, int excludeFd)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) 
    {
        if (it->first != excludeFd) 
        {
            // implement after message
        }
    }
}


// Channel* Server::createChannel(const std::string& name, Client* creator) 
// {
//     //implement channel return (channel)
// }

Channel* Server::createChannel(const std::string& name, Client* creator)
{
    // For initial testing, we'll just implement a stub
    // Return NULL for now (you can implement Channel class later)
    std::cout << "Channel creation requested: " << name << std::endl;
    return NULL;
}

void Server::executeCommand(Client* client, const Message& message)
{
    //implement after commandfactory
}
