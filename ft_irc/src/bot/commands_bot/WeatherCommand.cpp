#include <iostream>

#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "WeatherCommand.hpp"

WeatherCommand::WeatherCommand(Server* server) : ACommand(server) {}

WeatherCommand::~WeatherCommand() {}

// private, not used
WeatherCommand::WeatherCommand(const WeatherCommand& other) : ACommand(other._server) {}

WeatherCommand& WeatherCommand::operator=(const WeatherCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Static create method for factory
ACommand* WeatherCommand::create(Server* server) { return (new WeatherCommand(server)); }

// Execute the Weather command
void WeatherCommand::execute(Client* client, const Message& message)
{
    Print::Do("executing Weather command");
}
