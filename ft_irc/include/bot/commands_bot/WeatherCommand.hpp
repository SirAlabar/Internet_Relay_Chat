#ifndef WEATHERCOMMAND_HPP
#define WEATHERCOMMAND_HPP

#include "ABotCommand.hpp"
#include "BotContext.hpp"
#include "HTTPClient.hpp"

class Client;
class Server;
class Message;

/**
 * WeatherCommand - Implements weather information command for IRC bot
 * 
 * This command integrates with wttr.in API to provide weather information
 * for cities worldwide. Uses HTTPClient for API requests and formats
 * responses appropriately for IRC channels.
 * 
 * Usage: !weather <city> [format]
 * Examples: !weather porto, !weather new york detailed, !weather são paulo forecast
 */
class WeatherCommand : public ABotCommand
{
private:
    HTTPClient _httpClient;

    // Weather format options for different output types
    enum WeatherFormat 
    {
        FORMAT_SIMPLE,    // Single line: "porto: ☀️ +16°C"
        FORMAT_DETAILED,  // Multiple lines with humidity, wind, feels like
        FORMAT_FORECAST,  // 3-day forecast with daily breakdown
        FORMAT_FULL       // Complete ASCII art report with graphics
    };

    // Private copy constructor and assignment operator to prevent copying
    WeatherCommand(const WeatherCommand& other);
    WeatherCommand& operator=(const WeatherCommand& other);
    
    // Weather data retrieval methods - one for each format type
    std::string getWeatherSimple(const std::string& city);
    std::string getWeatherDetailed(const std::string& city);
    std::string getWeatherForecast(const std::string& city);
    std::string getWeatherFull(const std::string& city);
    
    // URL building and formatting helper methods
    std::string buildWttrUrl(const std::string& city) const;
    std::string formatCityForUrl(const std::string& city) const;
    
    // Response processing and validation methods
    std::string formatWeatherResponse(const std::string& response) const;
    std::string cleanResponse(const std::string& response) const;
    std::string getErrorMessage(const std::string& response) const;
    bool isValidCity(const std::string& city) const;
    
    // ASCII report processing methods for full format
    std::string extractTodayOnly(const std::string& fullReport);
    std::vector<std::string> splitIntoIrcMessages(const std::string& fullReport);

public:
    // Constructor and destructor
    WeatherCommand(Server* server);
    virtual ~WeatherCommand();

    // Main execution method - called by CommandBotFactory when !weather is used
    virtual void execute(BotContext* botctx, std::string& message);

    // Static creator for factory pattern - required by CommandBotFactory
    static ABotCommand* create(Server* server);
    
    // Helper method to get command help text for users
    std::string getWeatherHelp() const;
};

#endif