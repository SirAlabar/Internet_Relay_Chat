#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <iostream>

#include "Bot.hpp"
#include "CommandBotFactory.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "UtilsFun.hpp"

Bot::Bot(std::string host, int port, std::string password)
    : _nickname("IRCBot"),
      _serverHost(host),
      _serverPort(port),
      _password(password),
      _connected(false),
      _authenticated(false)
{
    _socket = Socket();
    _messageBuffer.clear();
    Print::Log("[BOT] Bot initialized - Target: " + host + " : " + toString(port));
}

Bot::~Bot() {};
bool Bot::connect()
{
    Print::Do("[BOT] Attempting to connect to " + _serverHost + " : " +
              toString(_serverPort));
    if (!_socket.create(AF_INET, SOCK_STREAM, 0))
    {
        Print::Fail("Error creating socket: " + toString(_socket.getLastError()));
        return (false);
    }
    // Connect to server
    if (!_socket.connect(_serverHost, _serverPort))
    {
        Print::Fail("[BOT] Failed to connect: " + _socket.getLastError());
        return (false);
    }
    Print::Ok("[BOT] Connected to server successfully!");
    if (!authenticate())
    {
        Print::Fail("[BOT] Failed to authenticate: " + _socket.getLastError());
        disconnect();
        return (false);
    }
    Print::Ok("[BOT] started on port " + toString(_serverPort));
    return (true);
}

bool Bot::authenticate()
{
    Print::Do("[BOT] Start authenticating...");
    std::string pass = "PASS " + _password + "\r\n";
    std::string nick = "NICK " + _nickname + "\r\n";
    std::string user = "USER " + _nickname + " 0 * :IRC Bot\r\n";
    if (_socket.send(pass) < 0)
    {
        Print::Fail("[BOT] Failed to send PASS command");
        return (false);
    }
    if (_socket.send(nick) < 0)
    {
        Print::Fail("[BOT] Failed to send NICK command");
        return (false);
    }
    if (_socket.send(user) < 0)
    {
        Print::Fail("[BOT] Failed to send USER command");
        return (false);
    }
    _authenticated = true;
    Print::Ok("[BOT] Authentication commands succesfull sent");
    return (true);
}

void Bot::run()
{
    Print::Do("[BOT] Entering the main loop....");
    _connected = true;

    char buffer[1024] = {0};
    std::string completeMessage;
    int serverFd = _socket.getFd();

    while (_connected)
    {
        ssize_t bytesRead = recv(serverFd, buffer, sizeof(buffer) - 1, 0);
        Print::Debug("Received " + toString(bytesRead) +
                     " bytes from client FD: " + toString(serverFd));
        if (bytesRead < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                usleep(100000);
                continue;
            }
            else
            {
                Print::Fail("[BOT] Failed receiving data: " +
                            std::string(strerror(errno)));
                break;
            }
        }
        else if (bytesRead == 0)
        {
            Print::Warn("[BOT] Server closed connection");
            break;
        }
        else
        {
            _messageBuffer += std::string(buffer, bytesRead);
            Print::Ok("[BOT] Received " + toString(bytesRead) + " bytes");
            size_t pos;
            while ((pos = _messageBuffer.find("\r\n")) != std::string::npos)
            {
                completeMessage = _messageBuffer.substr(0, pos);
                _messageBuffer.erase(0, pos + 2);
                CommandBotFactory::executeCommand(completeMessage, this);
                completeMessage.clear();
            }
        }
    }
    Print::Warn("[BOT] Exiting main loop...");
    if (_connected) disconnect();
}

void Bot::disconnect()
{
    Print::Do("[BOT] Starting Bot shutdown process....");
    _connected = false;
    _authenticated = false;
    _messageBuffer.clear();
    _password.clear();
    _serverHost.clear();
    if (_socket.isValid())
    {
        std::string quit = "QUIT :Bot shutting down \r\n";
        if (_socket.send(quit) < 0)
            Print::Warn("[BOT] Failed to send QUIT message");
        else
            Print::Ok("[BOT] Sent QUIT message");
        usleep(500000);
    }
    if (_socket.isValid())
    {
        _socket.close();
        Print::Debug("[BOT] Socket closed");
    }
    _messageBuffer.clear();
    Print::Ok("[BOT] Disconnected successfully. Byeeeeeeeee");
};

// Global server instance for signal handling
Bot* g_bot = NULL;

void Bot::sendMessage(const std::string& message)
{
    Print::Do("[BOT] Sending message...");
    ssize_t sentBytes = _socket.send(message);
    if (sentBytes < 0)
        Print::Fail("[BOT] Failed to sent message: " + _socket.getLastError());
    else if (sentBytes < (ssize_t)message.length())
        Print::Warn("[BOT] Partially sent message");
    else
        Print::Ok("[BOT] Message successfully sent");
}
// Signal handler for clean shutdown
void sigHandlerBot(int signum)
{
    Print::Log("\nReceived signal " + toString(signum) + ". Shutting down server...");
    if (g_bot)
    {
        g_bot->disconnect();
        g_bot = NULL;
    }
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    if (argc == 3)
    {
        signal(SIGINT, sigHandlerBot);   // Ctrl+C
        signal(SIGTERM, sigHandlerBot);  // kill command
        Bot bot("localhost", toInt(argv[1]), std::string(argv[2]));
        g_bot = &bot;
        if (bot.connect())
        {
            Print::Ok("[BOT] Starting main loop...");
            bot.run();
        }
        else
        {
            Print::Fail("[BOT] Failed to connect...");
            return 1;
        }
    }
    else
    {
        Print::StdErr("Usage ./ircbot <port> <pass>");
        return 1;
    }
    return 0;
}
