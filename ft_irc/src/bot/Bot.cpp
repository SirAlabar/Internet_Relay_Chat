#include <unistd.h>

#include <cerrno>
#include <csignal>
#include <cstddef>

#include "Bot.hpp"
#include "UtilsFun.hpp"
/* class Bot
{
private:
    Socket _socket;
    std::string _nickname;
    std::string _serverHost;
    int _serverPort;
    std::string _password;
    bool _connected;
    bool _authenticated;
    std::string _messageBuffer;

public:
    Bot(std::string& host, int port, std::string& pass);
    ~Bot();

    bool connect();
    void disconnect();
    bool isConnected() const;

    void run();

    void processMessage(const std::string& rawMessage);
    void sendMessage(const std::string& message);

    void authenticate();
    void joinChannel(const std::string& channel);
    void partChannel(const std::string& channel);
    void handleBotCommand(const std::string& channel, const std::string& user,
                          const std::string& command);
}; */

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
    _joinedChannels.clear();
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
                processMessage(completeMessage);
                completeMessage.clear();
            }
        }
    }
    Print::Warn("[BOT] Exiting main loop...");
    if (_connected) disconnect();
}

void Bot::processMessage(const std::string& rawMessage)
{
    Print::Do("[BOT] Processing the message");
    Print::Ok("[BOT] received " + rawMessage);
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
    _joinedChannels.clear();
    Print::Ok("[BOT] Disconnected successfully. Byeeeeeeeee");
};

// Global server instance for signal handling
Bot* g_bot = NULL;

// Signal handler for clean shutdown
void sigHandlerBot(int signum)
{
    Print::StdOut("\nReceived signal " + toString(signum) + ". Shutting down server...");
    if (g_bot)
    {
        g_bot->disconnect();
        g_bot = NULL;
    }
}

int main(int argc, char* argv[])
{
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
