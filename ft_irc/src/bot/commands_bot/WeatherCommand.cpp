#include <iostream>

#include "BotContext.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "WeatherCommand.hpp"

// Constructor - Initialize HTTP client with timeout for weather requests
WeatherCommand::WeatherCommand(Server* server) : ABotCommand(server)
{
    // Initialize HTTP client with reasonable timeout for weather requests
    _httpClient.setTimeout(8); // 8 seconds should be enough for wttr.in
    Print::Debug("[Weather] WeatherCommand initialized");
}

// Destructor
WeatherCommand::~WeatherCommand()
{
    Print::Debug("[Weather] WeatherCommand destroyed");
}

// Private copy constructor
WeatherCommand::WeatherCommand(const WeatherCommand& other) : ABotCommand(other._server)
{
}

// Private assignment operator
WeatherCommand& WeatherCommand::operator=(const WeatherCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return *this;
}

// Static creator for factory pattern
ABotCommand* WeatherCommand::create(Server* server)
{
    return (new WeatherCommand(server));
}

// Execute the Weather command - Main entry point for bot commands
void WeatherCommand::execute(BotContext* botctx, std::string& message)
{
    Print::Do("[Weather] Executing weather command");
    
    if (!botctx) 
    {
        Print::Fail("[Weather] BotContext is null");
        return;
    }
    
    // Parse command: "weather porto", "weather porto detailed", "weather porto forecast"
    std::string cityName;
    WeatherFormat format = FORMAT_SIMPLE;
    
    if (message.length() > 8 && message.substr(0, 8) == "weather ") 
    {
        std::string args = message.substr(8);
        
        // Check for format modifiers
        if (args.find(" detailed") != std::string::npos) 
        {
            format = FORMAT_DETAILED;
            args = args.substr(0, args.find(" detailed"));
        }
        else if (args.find(" forecast") != std::string::npos) 
        {
            format = FORMAT_FORECAST;
            args = args.substr(0, args.find(" forecast"));
        }
        else if (args.find(" full") != std::string::npos) 
        {
            format = FORMAT_FULL;
            args = args.substr(0, args.find(" full"));
        }
        
        cityName = args;
        // Clean whitespace
        while (!cityName.empty() && cityName[0] == ' ') 
        {
            cityName.erase(0, 1);
        }
        while (!cityName.empty() && cityName[cityName.length()-1] == ' ') 
        {
            cityName.erase(cityName.length()-1);
        }
    }
    
    if (cityName.empty()) 
    {
        botctx->reply("❌ Usage: !weather <city> [detailed|forecast|full]");
        botctx->reply("Examples: !weather porto, !weather london detailed, !weather paris forecast");
        return;
    }
    
    if (!isValidCity(cityName)) 
    {
        botctx->reply("❌ Invalid city name");
        return;
    }
    
    std::string response;
    
    // Get weather data based on format
    switch (format) 
    {
        case FORMAT_SIMPLE:
            response = getWeatherSimple(cityName);
            if (!response.empty()) 
            {
                botctx->reply("🌍 " + response);
            }
            break;
            
        case FORMAT_DETAILED:
            response = getWeatherDetailed(cityName);
            if (!response.empty()) 
            {
                botctx->reply("🌤️ Detailed weather for " + cityName + ":");
                botctx->reply(response);
            }
            break;
            
        case FORMAT_FORECAST:
            response = getWeatherForecast(cityName);
            if (!response.empty()) 
            {
                botctx->reply("📅 3-day forecast for " + cityName + ":");
                std::vector<std::string> lines = splitIntoIrcMessages(response);
                for (size_t i = 0; i < lines.size() && i < 5; ++i)  // Limit to 5 lines
                {
                    botctx->reply(lines[i]);
                }
            }
            break;
            
        case FORMAT_FULL:
            response = getWeatherFull(cityName);
            if (!response.empty()) 
            {
                botctx->reply("📊 Full weather report for " + cityName + ":");
                std::string todayOnly = extractTodayOnly(response);
                if (!todayOnly.empty()) 
                {
                    std::vector<std::string> lines = splitIntoIrcMessages(todayOnly);
                    for (size_t i = 0; i < lines.size() && i < 8; ++i)  // Limit to avoid spam
                    {
                        botctx->reply(lines[i]);
                    }
                }
            }
            break;
    }
    
    if (response.empty()) 
    {
        botctx->reply("❌ Weather service unavailable. Please try again later.");
    }
}

// Get simple weather format - Single line with basic info
std::string WeatherCommand::getWeatherSimple(const std::string& city)
{
    std::string formattedCity = formatCityForUrl(city);
    std::string url = "http://wttr.in/" + formattedCity + "?format=4&m";
    
    std::string response = _httpClient.get(url);
    if (_httpClient.isSuccess() && !response.empty()) 
    {
        std::string errorMsg = getErrorMessage(response);
        if (!errorMsg.empty()) 
        {
            return errorMsg;
        }
        return cleanResponse(response);
    }
    return "";
}

// Get detailed weather format - Multiple lines with humidity, wind, etc
std::string WeatherCommand::getWeatherDetailed(const std::string& city)
{
    std::string formattedCity = formatCityForUrl(city);
    // Custom format: location, temperature+feels, humidity+wind, condition, precipitation
    std::string url = "http://wttr.in/" + formattedCity + 
                     "?format=%l\\n🌡️%t+feels+%f\\n💧%h+🌬️%w\\n%C\\n☔%p+precip&m";
    
    std::string response = _httpClient.get(url);
    if (_httpClient.isSuccess() && !response.empty()) 
    {
        std::string errorMsg = getErrorMessage(response);
        if (!errorMsg.empty()) 
        {
            return errorMsg;
        }
        return cleanResponse(response);
    }
    return "";
}

// Get forecast weather format - 3-day forecast in compact format
std::string WeatherCommand::getWeatherForecast(const std::string& city)
{
    std::string formattedCity = formatCityForUrl(city);
    // Use format=v2 for structured forecast output
    std::string url = "http://wttr.in/" + formattedCity + "?format=v2&narrow&T&m";
    
    std::string response = _httpClient.get(url);
    if (_httpClient.isSuccess() && !response.empty()) 
    {
        std::string errorMsg = getErrorMessage(response);
        if (!errorMsg.empty()) 
        {
            return errorMsg;
        }
        return cleanResponse(response);
    }
    return "";
}

// Get full weather format - Complete ASCII art report
std::string WeatherCommand::getWeatherFull(const std::string& city)
{
    std::string formattedCity = formatCityForUrl(city);
    // No format parameter = full ASCII report with graphics
    std::string url = "http://wttr.in/" + formattedCity + "?T&narrow&m";
    
    std::string response = _httpClient.get(url);
    if (_httpClient.isSuccess() && !response.empty()) 
    {
        std::string errorMsg = getErrorMessage(response);
        if (!errorMsg.empty()) 
        {
            return errorMsg;
        }
        return response; // Don't clean - keep ASCII art formatting
    }
    return "";
}

// Build wttr.in URL with proper formatting and parameters
std::string WeatherCommand::buildWttrUrl(const std::string& city) const
{
    // Format city name for URL (replace spaces with +)
    std::string formattedCity = formatCityForUrl(city);
    
    // WTTR.IN URL with custom format for IRC
    // Format explanation:
    // %l = location name (city)
    // %C = weather condition with emoji 
    // %t = temperature
    // %h = humidity percentage
    // %w = wind speed and direction
    // &m = metric units
    
    std::string url = "http://wttr.in/" + formattedCity + 
                     "?format=%l:+%C+%t+💧%h+💨%w&m";
    
    return url;
}

// Format city name for URL encoding - supports multiple words
std::string WeatherCommand::formatCityForUrl(const std::string& city) const
{
    std::string formatted = city;
    
    // Normalize multiple spaces to single spaces first
    size_t pos = 0;
    while ((pos = formatted.find("  ", pos)) != std::string::npos)
    {
        formatted.replace(pos, 2, " ");
    }
    
    // Replace spaces with + for URL (wttr.in prefers + over %20)
    // This handles multiple words: "new york city" → "new+york+city"
    for (size_t i = 0; i < formatted.length(); ++i)
    {
        if (formatted[i] == ' ')
        {
            formatted[i] = '+';
        }
    }
    
    return formatted;
}

// Format the weather response for IRC output
std::string WeatherCommand::formatWeatherResponse(const std::string& response) const
{
    // Clean the raw response from wttr.in
    std::string cleaned = cleanResponse(response);
    
    if (cleaned.empty())
    {
        return "❌ Invalid weather data received.";
    }
    
    // Add visual emoji prefix for better IRC presentation
    std::string formatted = "🌍 " + cleaned;
    
    Print::Debug("[Weather] Formatted response: " + formatted);
    
    return formatted;
}

// Clean response string by removing unwanted characters
std::string WeatherCommand::cleanResponse(const std::string& response) const
{
    std::string cleaned = response;
    
    // Remove carriage returns and newlines
    size_t pos = 0;
    while ((pos = cleaned.find('\r', pos)) != std::string::npos)
    {
        cleaned.erase(pos, 1);
    }
    
    pos = 0;
    while ((pos = cleaned.find('\n', pos)) != std::string::npos)
    {
        cleaned.erase(pos, 1);
    }
    
    // Remove leading and trailing whitespace
    while (!cleaned.empty() && (cleaned[0] == ' ' || cleaned[0] == '\t'))
    {
        cleaned.erase(0, 1);
    }
    
    while (!cleaned.empty() && 
           (cleaned[cleaned.length()-1] == ' ' || cleaned[cleaned.length()-1] == '\t'))
    {
        cleaned.erase(cleaned.length()-1);
    }
    
    // Replace multiple consecutive spaces with single space
    pos = 0;
    while ((pos = cleaned.find("  ", pos)) != std::string::npos)
    {
        cleaned.replace(pos, 2, " ");
    }
    
    return cleaned;
}

// Check if the API response contains error messages
std::string WeatherCommand::getErrorMessage(const std::string& response) const
{
    // Convert response to lowercase for case-insensitive comparison
    std::string lowerResponse = response;
    for (size_t i = 0; i < lowerResponse.length(); ++i)
    {
        lowerResponse[i] = tolower(lowerResponse[i]);
    }
    
    // Check for common wttr.in error messages
    if (lowerResponse.find("unknown location") != std::string::npos)
    {
        return "❌ City not found. Please check the spelling and try again.";
    }
    
    if (lowerResponse.find("error") != std::string::npos)
    {
        return "❌ Weather service error. Please try again later.";
    }
    
    if (lowerResponse.find("not found") != std::string::npos)
    {
        return "❌ Location not found. Please try a different city name.";
    }
    
    // No error detected
    return "";
}

// Validate if city name contains only allowed characters
bool WeatherCommand::isValidCity(const std::string& city) const
{
    // Check length constraints
    if (city.empty() || city.length() > 100)
    {
        return false;
    }
    
    // Check for valid characters
    // Allow: letters, spaces, dots, hyphens, apostrophes, underscores
    for (size_t i = 0; i < city.length(); ++i)
    {
        char c = city[i];
        if (!isalnum(c) && c != ' ' && c != '.' && c != '-' && c != '\'' && c != '_')
        {
            return false;
        }
    }
    
    return true;
}

// Extract only today's weather from full ASCII report
std::string WeatherCommand::extractTodayOnly(const std::string& fullReport)
{
    std::istringstream iss(fullReport);
    std::string line;
    std::string result;
    bool inTodaySection = false;
    int lineCount = 0;
    
    while (std::getline(iss, line) && lineCount < 15) // Limit lines
    { 
        // Look for current weather section (usually at the top)
        if (line.find("Weather report:") != std::string::npos ||
            line.find("°C") != std::string::npos ||
            line.find("km/h") != std::string::npos ||
            (line.find("┌") != std::string::npos && line.find("─") != std::string::npos)) 
        {
            inTodaySection = true;
        }
        
        if (inTodaySection) 
        {
            if (!line.empty() && line.length() < 200) 
            {
                result += line + "\n";
                lineCount++;
            }
            
            // Stop at next day or location info
            if (line.find("Location:") != std::string::npos ||
                line.find("tomorrow") != std::string::npos ||
                lineCount >= 10) 
            {
                break;
            }
        }
    }
    
    return result;
}

// Split long text into IRC-friendly messages
std::vector<std::string> WeatherCommand::splitIntoIrcMessages(const std::string& fullReport)
{
    std::vector<std::string> messages;
    std::istringstream iss(fullReport);
    std::string line;
    
    while (std::getline(iss, line)) 
    {
        // Skip empty lines and very long lines
        if (!line.empty() && line.length() < 400) 
        {
            // Remove excessive whitespace but keep formatting
            while (line.find("    ") != std::string::npos) 
            {
                size_t pos = line.find("    ");
                line.replace(pos, 4, "  ");
            }
            
            if (!line.empty()) 
            {
                messages.push_back(line);
            }
        }
    }
    
    return messages;
}

// Get help text for the weather command
std::string WeatherCommand::getWeatherHelp() const
{
    return "🌤️ Weather Command Help:\n"
           "Usage: !weather <city> [format]\n"
           "\n"
           "Formats:\n"
           "  !weather porto           - Simple: porto: ☀️ +16°C\n"
           "  !weather porto detailed  - More info with humidity, wind\n"
           "  !weather porto forecast  - 3-day forecast\n"
           "  !weather porto full      - Complete report with ASCII art\n"
           "\n"
           "Examples:\n"
           "  !weather london\n"
           "  !weather new york detailed\n"
           "  !weather tokyo forecast\n"
           "  !weather paris full";
}