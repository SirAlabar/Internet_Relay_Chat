#ifndef WEATHERCOMMAND_HPP
#define WEATHERCOMMAND_HPP

#include "ABotCommand.hpp"
#include "BotContext.hpp"

class Client;
class Server;
class Message;

class WeatherCommand : public ABotCommand
{
private:
    // Private to prevent copying
    WeatherCommand(const WeatherCommand& other);
    WeatherCommand& operator=(const WeatherCommand& other);

public:
    WeatherCommand(Server* server);
    virtual ~WeatherCommand();

    // Execute the NICK command
    virtual void execute(BotContext* botctx, std::string& message);

    // Static creator for factory
    static ABotCommand* create(Server* server);
};

#endif
