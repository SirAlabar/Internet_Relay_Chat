#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

#include <string>

/**
 * HTTPClient - Simple HTTP client for making GET requests to weather APIs
 * 
 * This class provides a basic HTTP client implementation for making
 * requests to the wttr.in weather API. It handles URL parsing,
 * connection management, and response processing.
 */
class HTTPClient
{
private:
    int _timeout;        // Request timeout in seconds
    bool _lastSuccess;   // Status of last request
    std::string _lastError; // Last error message

    // URL parsing helper methods
    bool parseUrl(const std::string& url, std::string& host, int& port, std::string& path);
    std::string sendHttpRequest(const std::string& host, int port, const std::string& path);

public:
    // Constructor and destructor
    HTTPClient();
    ~HTTPClient();

    // Configuration methods
    void setTimeout(int seconds);
    
    // HTTP request methods
    std::string get(const std::string& url);
    
    // Status checking methods
    bool isSuccess() const;
    std::string getLastError() const;
};

#endif