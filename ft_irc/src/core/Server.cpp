#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

#include "Channel.hpp"
#include "Client.hpp"
#include "CommandFactory.hpp"
#include "General.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include "UtilsFun.hpp"

Server::Server() : _running(false) {}

Server::~Server() { stop(); }

std::string Server::_botpass = "botbot";
// Setup server with port and password
bool Server::setupServer(int port, const std::string& password)
{
    _password = password;
    Print::Do("SetupServer...");
    // Create the server socket
    if (!_serverSocket.create(AF_INET, SOCK_STREAM, 0))
    {
        Print::Fail("Error creating socket: " + toString(_serverSocket.getLastError()));
        return (false);
    }
    // Configure socket options
    int opt = 1;
    if (!_serverSocket.setOption(SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        Print::Fail("Error setting socket options: " +
                    toString(_serverSocket.getLastError()));
        return (false);
    }
    // Set as non-blocking
    if (!_serverSocket.setNonBlocking())
    {
        Print::Fail("Error setting socket to non-blocking: " +
                    toString(_serverSocket.getLastError()));
        return (false);
    }
    // Bind the socket to the port
    if (!_serverSocket.bind(port))
    {
        Print::Fail("Error binding: " + toString(_serverSocket.getLastError()));
        return (false);
    }
    // Listen for connections
    if (!_serverSocket.listen(10))
    {
        Print::Fail("Error listening: " + toString(_serverSocket.getLastError()));
        return (false);
    }

    Print::Ok("IRC Server started on port " + toString(port));

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
    extern volatile bool g_shutdown_requested;
    Print::Debug("Server entering main event loop");

    while (_running && !g_shutdown_requested)
    {
        int ready = poll(_pollFds.data(), _pollFds.size(), 1000);
        // Execute poll() to check for events
        if (g_shutdown_requested)
        {
            Print::Log("Gracefully shutting down from signal...");
            _running = false;
        }
        Print::Debug("Calling poll() with " + toString(_pollFds.size()) +
                     " file descriptors");

        Print::Debug("poll() returned with " + toString(ready) + " events");

        if (ready < 0)
        {
            Print::StdErr("Error in Poll(): " + toString(strerror(errno)) +
                          " (errno: " + toString(errno));
            if (errno == EINTR)
            {
                Print::Debug("poll() interrupted by signal, continuing");
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
            if (_pollFds[i].revents == 0)
            {
                Print::Debug("No events for FD: " + toString(_pollFds[i].fd) +
                             ", skipping");
                continue;
            }

            ready--;

            // Check if we have a new connection on the server socket
            if (_pollFds[i].fd == _serverSocket.getFd() && (_pollFds[i].revents & POLLIN))
            {
                Print::Debug("New connection event on server socket");
                processNewConnection();
            }
            // Process messages from existing clients
            else if (_pollFds[i].revents & POLLIN)
            {
                Print::Debug("Data available on client FD: " + toString(_pollFds[i].fd));
                processClientMessage(_pollFds[i].fd);
            }
            // Handle errors
            else if (_pollFds[i].revents & (POLLERR | POLLNVAL))
            {
                Print::StdErr("ERROR condition on FD: " + toString(_pollFds[i].fd));
                removeClient(_pollFds[i].fd);
            }
            // Handle hangup WITH data available
            else if ((_pollFds[i].revents & POLLHUP) && (_pollFds[i].revents & POLLIN))
            {
                Print::Debug("POLLHUP+POLLIN received for FD: " +
                             toString(_pollFds[i].fd) + " - processing remaining data");
                processClientMessage(_pollFds[i].fd);
            }
            // Handle hangup WITHOUT data available - DON'T disconnect yet
            else if (_pollFds[i].revents & POLLHUP)
            {
                Print::Debug("POLLHUP (only) received for FD: " +
                             toString(_pollFds[i].fd) + " - keeping connection");
            }
        }
        addBotToAllChannels(getClientByNick("IRCBot"));
        print_clients();
    }

    Print::Debug("Exiting server main event loop");
}

// Stop the server and clean up resources
void Server::stop()
{
    Print::Do("Starting server shutdown process...\t\t\n");
    _running = false;

    // Clearn client buffer
    Print::Do("Cleaning up " + toString(_clientBuffers.size()) + " client buffers...");
    _clientBuffers.clear();
    Print::Ok("client buffers cleared!");

    // Close client sockets
    Print::Do("Cleaning up " + toString(_clientSockets.size()) +
              " client sockets...\t\t\n");
    for (std::map<int, Socket*>::iterator it = _clientSockets.begin();
         it != _clientSockets.end(); ++it)
    {
        int fd = it->first;
        Print::Debug("Explicitly closing client socket FD: " + toString(fd));
        if (it->second && it->second->isValid())
        {
            it->second->close();
        }
        delete it->second;
    }
    _clientSockets.clear();
    Print::Ok("Sockets cleared!");

    // Close channels
    Print::Do("Cleaning up " + toString(_channels.size()) + " channels...");
    for (std::map<std::string, Channel*>::iterator it = _channels.begin();
         it != _channels.end(); ++it)
    {
        delete it->second;
    }
    _channels.clear();
    Print::Ok("channels cleared!");

    // Close Clients
    Print::Do("Cleaning up " + toString(_clients.size()) + " clients...");
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end();
         ++it)
    {
        delete it->second;
    }
    _clients.clear();
    Print::Ok("clients clear!");

    // Clear poll
    Print::Do("Freeing poll file descriptors data...");
    {
        std::vector<pollfd>().swap(_pollFds);
        Print::Ok("");
    }

    // Close server socket
    Print::Do("Closing server socket...");
    _serverSocket.close();

    Print::Ok("IRC Server shutdown complete.");
}

// Process a new client connection
void Server::processNewConnection()
{
    Print::Do("ProcessNewConnection");
    Socket* clientSocket = new Socket(_serverSocket.accept());
    if (!clientSocket->isValid())
    {
        Print::Fail("Error accepting connection: " + _serverSocket.getLastError());
        return;
    }

    Print::Ok("Socket accepted successfully");

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

    Print::Ok("New connection accepted. FD: " + toString(clientFd));
}

void Server::processClientMessage(int clientFd)
{
    extern volatile bool g_shutdown_requested;

    if (g_shutdown_requested)
    {
        Print::Debug("Shutdown requested, skipping message processing");
        return;
    }

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

    Print::Debug("Received " + toString(bytesRead) +
                 " bytes from client FD: " + toString(clientFd));

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
    Print::Debug("Buffer map size BEFORE: " + toString(_clientBuffers.size()));
    // Append to client buffer
    _clientBuffers[clientFd] += buffer;
    Print::Debug("Buffer map size AFTER: " + toString(_clientBuffers.size()));
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
        Message message = Message(rawMessage);
        Print::Debug("Processing command: " + message.getCommand());
        CommandFactory::executeCommand(client, this, message);
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
const std::string& Server::getBotPassword() const { return (_botpass); }

void Server::setBot(bool status) { _botConnected = status; }
bool Server::hasBot() const { return _botConnected; }
Client* Server::getBot() const
{
    Client* bot = NULL;
    std::map<int, Client*>::const_iterator it = _clients.begin();
    std::map<int, Client*>::const_iterator ite = _clients.end();
    for (; it != ite; it++)
        if (it->second->isBot()) bot = it->second;
    return bot;
}

void Server::addBotToAllChannels(Client* bot)
{
    Print::Do("Adding bot to all channels");
    if (!bot || !bot->isBot())
    {
        Print::Warn("Bot not found.");
        return;
    }
    for (std::map<std::string, Channel*>::iterator it = _channels.begin();
         it != _channels.end(); it++)
    {
        Channel* channel = it->second;
        if (channel && !channel->hasClient(bot)) channel->addClient(bot);
    }
    Print::Ok("Bot added to all channels");
}
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
    Print::Debug("Removing client FD: " + toString(clientFd));

    Client* client = getClient(clientFd);
    if (client)
    {
        for (std::map<std::string, Channel*>::iterator it = _channels.begin();
             it != _channels.end(); it++)
        {
            it->second->removeClient(client);
        }
    }

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
    Print::Debug("Buffer map size AFTER remove: " + toString(_clientBuffers.size()));

    // Remove client object
    if (_clients.find(clientFd) != _clients.end())
    {
        if (_clients.find(clientFd)->second->isBot()) setBot(false);
        delete _clients[clientFd];
        _clients.erase(clientFd);
    }
    // Remove and delete socket
    if (_clientSockets.find(clientFd) != _clientSockets.end())
    {
        delete _clientSockets[clientFd];
        _clientSockets.erase(clientFd);
    }

    Print::Debug("Client disconnected. FD: " + toString(clientFd));
}

// Broadcast message to all clients except excludeFd
void Server::broadcast(const std::string& message, int excludeFd)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end();
         ++it)
    {
        if (it->first != excludeFd)
        {
            it->second->sendMessage(message);
        }
    }
}

Channel* Server::createChannel(const std::string& name, Client* creator)
{
    Print::Debug("Channel creation requested: " + name);

    // Verify if already exists
    Channel* existingChannel = getChannel(name);
    if (existingChannel)
    {
        Print::Debug("Channel " + name + " already exists");
        return existingChannel;
    }

    // Create new channel
    Channel* newChannel = new Channel(name);
    _channels[name] = newChannel;

    Print::Ok("Channel " + name + " created successfully");
    if (creator)
    {
        newChannel->addClient(creator);
        newChannel->addOperator(creator);
        Print::Debug("Added creator " + creator->getNickname() + " to channel " + name);
    }

    return newChannel;
}

void Server::print_clients(bool command)
{
    std::stringstream ss;
    ss << std::right << Color::YELLOW << std::setw(10) << "FD" << "|" << std::setw(10)
       << "NICK" << "|" << std::setw(10) << "USER" << "|" << std::setw(10) << "AUTH?"
       << "|" << std::setw(10) << "BOT?" << "|";
    Print::Debug(ss.str(), command);

    std::map<int, Client*>::iterator it = _clients.begin();
    std::map<int, Client*>::iterator ite = _clients.end();
    for (; it != ite; it++)
    {
        std::stringstream ssa;
        ssa << std::right << Color::YELLOW << std::setw(10) << Server::formatStr(it->second->getFdString())
            << "|" << std::setw(10) << Server::formatStr(it->second->getNickname()) << "|"
            << std::setw(10) << Server::formatStr(it->second->getUsername()) << "|"
            << std::setw(10)
            << Server::formatStr(it->second->isAuthenticated() ? "YES" : "") << "|"
            << std::setw(10) << Server::formatStr(it->second->isBot() ? "YES" : "")
            << "|";
        Print::Debug(ssa.str(), command);
    }
    std::map<std::string, Channel*>::iterator it_channel = _channels.begin();
    for (; it_channel != _channels.end(); it_channel++)
    {
        std::stringstream ssa;
        ssa << Color::GREEN + "======= Channel: " + it_channel->second->getName()
            + (it_channel->second->isInviteOnly() ? " | Invite_Only" : "")
            + (it_channel->second->hasUserLimit() ? " | Has_User_limit: "
                    + (toString(it_channel->second->getUserLimit())) : "" )
            + (it_channel->second->hasKey() ? " | Has_Key" : "")
            + (it_channel->second->isTopicRestricted() ? " | Topic_Restricted" : "");
        Print::Debug(ssa.str(), command);

        std::stringstream ssb;
        ssb << std::right << Color::ORANGE << std::setw(10) << "FD" << "|" << std::setw(10) << "NICK"
            << "|" << std::setw(10) << "OPERATOR" << "|";
        Print::Debug(ssb.str(), command);

        std::map<int, Client*>::const_iterator it_client = it_channel->second->getClients().begin();
        for (; it_client != it_channel->second->getClients().end(); it_client++)
        {
            std::stringstream ssb;
            ssb << std::right << Color::ORANGE << std::setw(10) << toString(it_client->first) << "|"
                << std::setw(10) << Server::formatStr(toString(it_client->second->getNickname())) << "|"
                << std::setw(10) << (it_channel->second->isOperator(it_client->second) ? "YES" : "")
                << "|";
            Print::Debug(ssb.str(), command);
        }
        std::cerr << Color::RESET;
    }
    std::cerr << Color::RESET;
}

void Server::cleanupEmptyChannels()
{
    std::vector<std::string> channelsToRemove;
    for (std::map<std::string, Channel*>::iterator it = _channels.begin();
         it != _channels.end(); it++)
    {
        if (it->second->isEmpty()) channelsToRemove.push_back(it->first);
    }
    for (size_t i = 0; i < channelsToRemove.size(); i++)
        removeChannel(channelsToRemove[i]);
}

void Server::broadcastChannel(const std::string& message, const std::string& chName,
                              int excludeFd)
{
    Channel* channel = getChannel(chName);
    if (!channel) return;

    const std::map<int, Client*>& clients = channel->getClients();

    std::map<int, Client*>::const_iterator it = clients.begin();
    std::map<int, Client*>::const_iterator ite = clients.end();
    for (; it != ite; it++)
    {
        if (it->first != excludeFd) it->second->sendMessage(message);
    }
}

std::string Server::formatStr(const std::string& str)
{
    std::string format = str;

    if (format.length() > 10) format = format.substr(0, 9) + ".";
    return (format);
}
