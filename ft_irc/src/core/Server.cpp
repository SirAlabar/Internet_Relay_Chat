#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>

#include "General.hpp"

#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "CommandFactory.hpp"
#include "Channel.hpp"
#include "Socket.hpp"
#include "UtilsFun.hpp"

Server::Server() : _running(false) {}

Server::~Server() { stop(); }

// Setup server with port and password
bool Server::setupServer(int port, const std::string& password)
{
	_password = password;

    // Create the server socket
    if (!_serverSocket.create(AF_INET, SOCK_STREAM, 0))
    {
        Print::StdErr("Error creating socket: " + toString(_serverSocket.getLastError()));
        // std::cerr << "Error creating socket: " << _serverSocket.getLastError()
        // 	  << std::endl;

        return (false);
    }
    // Configure socket options
    int opt = 1;
    if (!_serverSocket.setOption(SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        Print::StdErr("Error setting socket options: " +
                      toString(_serverSocket.getLastError()));
        // std::cerr << "Error setting socket options: " << _serverSocket.getLastError()
        // 	  << std::endl;

        return (false);
    }
    // Set as non-blocking
    if (!_serverSocket.setNonBlocking())
    {
        Print::StdErr("Error setting socket to non-blocking: " +
                      toString(_serverSocket.getLastError()));
        // std::cerr << "Error setting socket to non-blocking: "
        // 	  << _serverSocket.getLastError() << std::endl;

        return (false);
    }
    // Bind the socket to the port
    if (!_serverSocket.bind(port))
    {
        Print::StdErr("Error binding: " + toString(_serverSocket.getLastError()));
        // std::cerr << "Error binding: " << _serverSocket.getLastError() << std::endl;

        return (false);
    }
    // Listen for connections
    if (!_serverSocket.listen(10))
    {
        Print::StdErr("Error listening: " + toString(_serverSocket.getLastError()));
        // std::cerr << "Error listening: " << _serverSocket.getLastError() << std::endl;
        return (false);
    }

    Print::StdOut("IRC Server started on port " + toString(port));
    // std::cout << "IRC Server started on port " << port << std::endl;

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
    Print::Debug("DEBUG: Server entering main event loop");
    // std::cout << "DEBUG: Server entering main event loop" << std::endl;

    while (_running)
    {
        // Execute poll() to check for events

        Print::Debug("Calling poll() with " + toString(_pollFds.size()) +
                     " file descriptors");
        // std::cout << "DEBUG: Calling poll() with " << _pollFds.size()
        // 	  << " file descriptors" << std::endl;
        int ready = poll(_pollFds.data(), _pollFds.size(), -1);

        Print::Debug("poll() returned with " + toString(ready) + " events");
        // std::cout << "DEBUG: poll() returned with " << ready << " events" << std::endl;

        if (ready < 0)
        {
            Print::StdErr("Error in Poll(): " + toString(strerror(errno)) +
                          " (errno: " + toString(errno));
            // std::cerr << "ERROR in poll(): " << strerror(errno) << " (errno: " << errno
            //    << ")" << std::endl;
            if (errno == EINTR)
            {
                Print::Debug("poll() interrupted by signal, continuing");
                // std::cout << "DEBUG: poll() interrupted by signal, continuing"
                // 	  << std::endl;

                continue;
            }
            break;
        }

        // Process all fds with events
        for (size_t i = 0; i < _pollFds.size() && ready > 0; ++i)
        {
            std::stringstream ss;
            ss << "Checking FD: " << _pollFds[i].fd
               << " events: " << (_pollFds[i].revents & POLLIN ? "POLLIN " : "")
               << (_pollFds[i].revents & POLLOUT ? "POLLOUT " : "")
               << (_pollFds[i].revents & POLLHUP ? "POLLHUP " : "")
               << (_pollFds[i].revents & POLLERR ? "POLLERR " : "")
               << (_pollFds[i].revents & POLLNVAL ? "POLLNVAL " : "");
            Print::Debug(ss.str());
            // std::cout << "DEBUG: Checking FD: " << _pollFds[i].fd
            //    << " events: " << (_pollFds[i].revents & POLLIN ? "POLLIN " : "")
            //    << (_pollFds[i].revents & POLLOUT ? "POLLOUT " : "")
            //    << (_pollFds[i].revents & POLLHUP ? "POLLHUP " : "")
            //    << (_pollFds[i].revents & POLLERR ? "POLLERR " : "")
            //    << (_pollFds[i].revents & POLLNVAL ? "POLLNVAL " : "") << std::endl;

            if (_pollFds[i].revents == 0)
            {
                Print::Debug("No events for FD: " + toString(_pollFds[i].fd) +
                             ", skipping");
                // std::cout << "DEBUG: No events for FD: " << _pollFds[i].fd << ",
                // skipping"
                // 	  << std::endl;

                continue;
            }

            ready--;

            // Check if we have a new connection on the server socket
            if (_pollFds[i].fd == _serverSocket.getFd() && (_pollFds[i].revents & POLLIN))
            {
                Print::Debug("New connection event on server socket");
                // std::cout << "DEBUG: New connection event on server socket" <<
                // std::endl;

                processNewConnection();
            }
            // Process messages from existing clients
            else if (_pollFds[i].revents & POLLIN)
            {
                Print::Debug("Data available on client FD: " + toString(_pollFds[i].fd));
                // std::cout << "DEBUG: Data available on client FD: " << _pollFds[i].fd
                // 	  << std::endl;

                processClientMessage(_pollFds[i].fd);
            }
            // Handle errors
            else if (_pollFds[i].revents & (POLLERR | POLLNVAL))
            {
                Print::StdErr("ERROR condition on FD: " + toString(_pollFds[i].fd));
                // std::cout << "ERROR condition on FD: " << _pollFds[i].fd << std::endl;

                removeClient(_pollFds[i].fd);
            }
            // Handle hangup WITH data available
            else if ((_pollFds[i].revents & POLLHUP) && (_pollFds[i].revents & POLLIN))
            {
                Print::Debug("POLLHUP+POLLIN received for FD: " +
                             toString(_pollFds[i].fd) + " - processing remaining data");
                // std::cout << "DEBUG: POLLHUP+POLLIN received for FD: " <<
                // _pollFds[i].fd
                // 	  << " - processing remaining data" << std::endl;

                processClientMessage(_pollFds[i].fd);
            }
            // Handle hangup WITHOUT data available - DON'T disconnect yet
            else if (_pollFds[i].revents & POLLHUP)
            {
                Print::Debug("POLLHUP (only) received for FD: " +
                             toString(_pollFds[i].fd) + " - keeping connection");
                // std::cout << "DEBUG: POLLHUP (only) received for FD: " <<
                // _pollFds[i].fd
                // 	  << " - keeping connection" << std::endl;
            }
        }
        print_clients();
    }

    std::cout << "DEBUG: Exiting server main event loop" << std::endl;
}

// Stop the server and clean up resources
void Server::stop()
{
	std::cout << "Starting server shutdown process..." << std::endl;
	_running = false;

    // Close Clients
    std::cout << "Cleaning up " << _clients.size() << " clients..." << std::endl;
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end();
         ++it)
    {
        delete it->second;
    }
    _clients.clear();

    // Cloese client sockets
    std::cout << "Cleaning up " << _clientSockets.size() << " client sockets..."
              << std::endl;
    for (std::map<int, Socket*>::iterator it = _clientSockets.begin();
         it != _clientSockets.end(); ++it)
    {
        int fd = it->first;
        std::cout << "Explicitly closing client socket FD: " << fd << std::endl;
        if (it->second && it->second->isValid())
        {
            it->second->close();
        }
        delete it->second;
    }
    _clientSockets.clear();
    ;

    // Close channels
    std::cout << "Cleaning up " << _channels.size() << " channels..." << std::endl;
    for (std::map<std::string, Channel*>::iterator it = _channels.begin();
         it != _channels.end(); ++it)
    {
        delete it->second;
    }
    _channels.clear();

    // Clear poll
    std::cout << "Freeing poll file descriptors data..." << std::endl;
    {
        std::vector<pollfd>().swap(_pollFds);
    }

	// Clear poll
	Print::StdOut("Freeing poll file descriptors data...");
	{
		std::vector<pollfd>().swap(_pollFds);
	}

	// Close server socket
	Print::StdOut("Closing server socket...");
	_serverSocket.close();

	Print::StdOut("IRC Server shutdown complete.");
}

// Process a new client connection
void Server::processNewConnection()
{
    Socket* clientSocket = new Socket(_serverSocket.accept());
    if (!clientSocket->isValid())
    {
        std::cerr << "Error accepting connection: " << _serverSocket.getLastError()
                  << std::endl;
        return;
    }

	std::cout << "DEBUG: Socket accepted successfully" << std::endl;

	// Set client socket as non-blocking
	clientSocket->setNonBlocking();
	int clientFd = clientSocket->getFd();

	_clientSockets[clientFd] = clientSocket;

	// Add to poll
	pollfd clientPollFd;
	clientPollFd.fd = clientFd;
	clientPollFd.events = POLLIN;  // Monitor for read
	clientPollFd.revents = 0;
	_pollFds.push_back(clientPollFd);

	// Create Client object and add to map
	Client* client = new Client(clientFd);
	_clients[clientFd] = client;

	std::string welcomeMsg = ":server 001 * :Welcome to the IRC server!\r\n";
	client->sendMessage(welcomeMsg);

	std::cout << "New connection accepted. FD: " << clientFd << std::endl;
}
void Server::processClientMessage(int clientFd)
{
    Print::Debug("Processing message from client FD: " + toString(clientFd));

    Client* client = getClient(clientFd);
    if (!client)
    {
        Print::StdErr("Error: Client not found for FD: " + toString(clientFd));
        return;
    }

    // Buffer for receiving data
    char buffer[1024] = {0};
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

    Print::Debug("Received " + toString(bytesRead) + " bytes from client FD: " + 
                toString(clientFd));

    if (bytesRead < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            Print::Debug("No data available, but connection is still open");
            return;
        }

        Print::StdErr("Error receiving data: " + toString(strerror(errno)));
        removeClient(clientFd);
        return;
    }

    if (bytesRead == 0)
    {
        Print::Debug("Client closed connection gracefully");
        removeClient(clientFd);
        return;
    }

    Message msg(buffer);
    CommandFactory::executeCommand(client, this, msg);
    // Append to client buffer
    _clientBuffers[clientFd] += buffer;

    // TEST
    std::string welcome =
        ":server NOTICE * :Hello! You are connected to the IRC server\r\n";
    client->sendMessage(welcome);

    // Append to client buffer
    _clientBuffers[clientFd] += buffer;
    
    // Process complete messages (ending with \r\n)
    std::string& clientBuffer = _clientBuffers[clientFd];
    size_t pos;
    
    while ((pos = clientBuffer.find("\r\n")) != std::string::npos)
    {
        // Extract a complete message
        std::string rawMessage = clientBuffer.substr(0, pos);
        // Remove the processed message from the buffer
        clientBuffer.erase(0, pos + 2);
        
        // Parse and execute the message
        Message message = Message::parse(rawMessage);
        Print::Debug("Processing command: " + message.getCommand());
        
        // Handle PING specially for keep-alive
        if (message.getCommand() == "PING")
        {
            std::string pongReply = ":server PONG server :" + message.getParams() + "\r\n";
            client->sendMessage(pongReply);
        }
        else
        {
            // Execute the command using factory
            CommandFactory::executeCommand(client, this, message);
        }
    }
    
    // If buffer gets too large without complete messages, clear it (prevent DoS)
    if (clientBuffer.size() > 4096)
    {
        clientBuffer.clear();
        Print::StdErr("Warning: Client buffer overflow, clearing buffer");
    }
}

// Get server password
const std::string& Server::getPassword() const { return (_password); }

// Get all channels
std::map<std::string, Channel*>& Server::getChannels() { return (_channels); }

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

// Get client by nickname
Client* Server::getClientByNick(const std::string& nickname)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end();
         ++it)
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
	std::cout << "Removing client FD: " << clientFd << std::endl;

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

    // Remove client object
    if (_clients.find(clientFd) != _clients.end())
    {
        delete _clients[clientFd];
        _clients.erase(clientFd);
    }
    // Remove and delete socket
    if (_clientSockets.find(clientFd) != _clientSockets.end())
    {
        delete _clientSockets[clientFd];
        _clientSockets.erase(clientFd);
    }

	std::cout << "Client disconnected. FD: " << clientFd << std::endl;
}

// Broadcast message to all clients except excludeFd
void Server::broadcast(const std::string& message, int excludeFd)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end();
         ++it)
    {
        if (it->first != excludeFd)
        {
            // implement after message
        }
    }
}

Channel* Server::createChannel(const std::string& name, Client* creator)
{
	// For initial testing, we'll just implement a stub
	// Return NULL for now
	std::cout << "Channel creation requested: " << name << std::endl;
	return NULL;
}

void Server::print_clients()
{
    if (DEBUG)
    {

        std::map<int, Client*>::iterator it = _clients.begin();
        std::map<int, Client*>::iterator ite = _clients.end();

		for (; it != ite; it++)
		{
			Print::Debug(Color::YELLOW + 
						 "\n\tClient fd on server map: " + toString(it->first) + 
						 "\n\tfd on Client class: " + it->second->getFdString() + 
						 "\n\tnickname: " + it->second->getNickname() + 
						 "\n\tusername: " + it->second->getUsername() + 
						 "\n\tautenticated? == " + toString(it->second->isAuthenticated())
			);
		}
	}
}

void Server::print_clients()
{
    if (DEBUG)
    {
        std::map<int, Client*>::iterator it = _clients.begin();
        std::map<int, Client*>::iterator ite = _clients.end();

        for (; it != ite; it++)
        {
            Print::Debug(
                Color::YELLOW + "\n\tClient fd on server map: " + toString(it->first) +
                "\n\tfd on Client class: " + it->second->getFdString() +
                "\n\tnickname: " + it->second->getNickname() +
                "\n\tusername: " + it->second->getUsername() +
                "\n\tautenticated? == " + toString(it->second->isAuthenticated()));
        }
    }
}
