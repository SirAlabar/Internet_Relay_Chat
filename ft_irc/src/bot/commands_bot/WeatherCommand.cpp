#include <iostream>

#include "BotContext.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "WeatherCommand.hpp"

WeatherCommand::WeatherCommand(Server* server) : ABotCommand(server) {}

WeatherCommand::~WeatherCommand() {}

// private, not used
WeatherCommand::WeatherCommand(const WeatherCommand& other) : ABotCommand(other._server)
{
}

WeatherCommand& WeatherCommand::operator=(const WeatherCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Static create method for factory
ABotCommand* WeatherCommand::create(Server* server)
{
    return (new WeatherCommand(server));
}

// Execute the Weather command
void WeatherCommand::execute(BotContext* botctx, std::string& message)
{
    if (!botctx) return;
    (void)message;
    botctx->reply("Hello " + message);
}
