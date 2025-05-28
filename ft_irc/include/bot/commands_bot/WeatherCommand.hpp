#ifndef WEATHERCOMMAND_HPP
#define WEATHERCOMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class WeatherCommand : public ACommand
{
private:
    // Private to prevent copying
    WeatherCommand(const WeatherCommand& other);
    WeatherCommand& operator=(const WeatherCommand& other);

public:
    WeatherCommand(Server* server);
    virtual ~WeatherCommand();

    // Execute the NICK command
    virtual void execute(Client* client, const Message& message);

    // Static creator for factory
    static ACommand* create(Server* server);
};

#endif
