#include <unistd.h>

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

bool Bot::connect()
{
    Print::Do("[BOT] Attempting to connect to " + _serverHost + " : " +
              toString(_serverPort));
    if (!_socket.create(AF_INET, SOCK_STREAM, 0))
    {
        Print::Fail("Error creating socket: " + toString(_socket.getLastError()));
        return (false);
    }
    // Configure socket options
    if (!_socket.connect(_serverHost, _serverPort))
    {
        Print::Fail("[BOT] Error connecting");
        return (false);
    }
    // Set as non-blocking
    if (!_socket.setNonBlocking())
    {
        Print::Fail("Error setting socket to non-blocking: " +
                    toString(_socket.getLastError()));
        return (false);
    }
    Print::Ok("[BOT] started on port " + toString(_serverPort));

    return (true);
}

int main(int argc, char* argv[])
{
    if (argc == 3)
    {
        Bot bot("localhost", toInt(argv[1]), std::string(argv[2]));
    }
    else
    {
        Print::StdErr("Usage ./ircbot <port> <pass>");
        return 1;
    }
    return 0;
}
