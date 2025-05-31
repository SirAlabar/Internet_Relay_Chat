#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

#include "Socket.hpp"
#include <string>

class HTTPClient
{
private:
    Socket _socket;
    bool _success;
    int _timeout;
    std::string _lastError;

    // Private to prevent copying
    HTTPClient(const HTTPClient& other);
    HTTPClient& operator=(const HTTPClient& other);

    // Private helper methods
    bool parseUrl(const std::string& url, std::string& host, int& port, std::string& path);
    std::string buildHttpRequest(const std::string& path, const std::string& host);
    std::string parseHttpResponse(const std::string& response);
    bool connectToHost(const std::string& host, int port);
    void cleanup();
    std::string urlEncode(const std::string& str);

public:
    HTTPClient();
    HTTPClient(int timeoutSeconds);
    ~HTTPClient();

    // Main functionality
    std::string get(const std::string& url);

    // Status and configuration
    bool isSuccess() const;
    std::string getLastError() const;
    void setTimeout(int seconds);
    int getTimeout() const;

    // Static utility methods
    static std::string encodeUrl(const std::string& str);
    static bool isValidUrl(const std::string& url);
};

#endif