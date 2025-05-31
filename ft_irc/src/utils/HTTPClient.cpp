#include "HTTPClient.hpp"
#include "UtilsFun.hpp"
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <sstream>

HTTPClient::HTTPClient() : _success(false), _timeout(5)
{
    _lastError.clear();
}

HTTPClient::HTTPClient(int timeoutSeconds) : _success(false), _timeout(timeoutSeconds)
{
    _lastError.clear();
}

HTTPClient::~HTTPClient()
{
    cleanup();
}

// Private copy constructor and assignment operator
HTTPClient::HTTPClient(const HTTPClient& other) : _socket(other._socket), _success(false), _timeout(5)
{
    _lastError.clear();
}

HTTPClient& HTTPClient::operator=(const HTTPClient& other)
{
    if (this != &other)
    {
        cleanup();
        _socket = other._socket;
        _success = false;
        _timeout = other._timeout;
        _lastError.clear();
    }
    return *this;
}

std::string HTTPClient::get(const std::string& url)
{
    Print::Debug("[HTTP] Starting GET request to: " + url);
    
    _success = false;
    _lastError.clear();
    
    // STEP 1: Validate URL format
    if (!isValidUrl(url))
    {
        _lastError = "Invalid URL format";
        Print::Fail("[HTTP] " + _lastError);
        return "";
    }
    
    // STEP 2: Parse URL into components (host, port, path)
    std::string host, path;
    int port;
    
    if (!parseUrl(url, host, port, path))
    {
        _lastError = "Failed to parse URL";
        Print::Fail("[HTTP] " + _lastError);
        return "";
    }
    
    Print::Debug("[HTTP] Parsed - Host: " + host + " Port: " + toString(port) + " Path: " + path);
    
    // STEP 3: Establish TCP connection to server
    if (!connectToHost(host, port))
    {
        _lastError = "Failed to connect to " + host + ":" + toString(port);
        Print::Fail("[HTTP] " + _lastError);
        return "";
    }
    Print::Debug("[HTTP] Waiting for non-blocking socket to be ready...");

    // STEP 3.5: Wait for non-blocking socket to be ready for writing
    pollfd pfd;
    pfd.fd = _socket.getFd();
    pfd.events = POLLOUT;
    pfd.revents = 0;

    int pollResult = poll(&pfd, 1, _timeout * 1000);  // timeout in ms

    if (pollResult <= 0)
    {
        _lastError = "Socket not ready for writing within timeout";
        Print::Fail("[HTTP] " + _lastError);
        cleanup();
        return "";
    }

    if (!(pfd.revents & POLLOUT))
    {
        _lastError = "Socket error while waiting for write readiness";
        Print::Fail("[HTTP] " + _lastError);
        cleanup();
        return "";
    }

    Print::Ok("[HTTP] Non-blocking socket ready for writing");

    // STEP 4: Build HTTP request message
    std::string request = buildHttpRequest(path, host);
    Print::Debug("[HTTP] Request built, sending...");
    
    // STEP 5: Send HTTP request to server
    ssize_t sentBytes = _socket.send(request);
    if (sentBytes <= 0)
    {
        _lastError = "Failed to send HTTP request";
        Print::Fail("[HTTP] " + _lastError);
        cleanup();
        return "";
    }
    
    Print::Debug("[HTTP] Request sent (" + toString(sentBytes) + " bytes), receiving response...");
    
    // STEP 6: Receive HTTP response with timeout
    std::string response;
    char buffer[4096];
    bool headerReceived = false;
    int attempts = 0;
    const int maxAttempts = _timeout * 10; // 100ms intervals
    
    while (attempts < maxAttempts)
    {
        ssize_t bytesReceived = _socket.recv(buffer, sizeof(buffer) - 1, 0);
        
        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0';
            response += buffer;
            
            // Check if we have complete HTTP headers
            if (!headerReceived && response.find("\r\n\r\n") != std::string::npos)
            {
                headerReceived = true;
                Print::Debug("[HTTP] Headers received");
            }
            
            // For simple responses (like wttr.in), we can stop after headers
            // For more complex responses, you might want to parse Content-Length
            if (headerReceived && response.find("Content-Length:") == std::string::npos)
            {
                // No Content-Length header, assume response is complete
                break;
            }
        }
        else if (bytesReceived == 0)
        {
            // Connection closed by server
            Print::Debug("[HTTP] Connection closed by server");
            break;
        }
        else
        {
            // No data available, wait a bit
            usleep(100000); // 100ms
            attempts++;
        }
    }
    
    cleanup();
    
    if (response.empty())
    {
        _lastError = "No response received or timeout";
        Print::Fail("[HTTP] " + _lastError);
        return "";
    }
    
    Print::Debug("[HTTP] Response received (" + toString(response.length()) + " bytes)");
    
    // STEP 7: Parse HTTP response and extract body
    std::string body = parseHttpResponse(response);
    if (body.empty() && !_success)
    {
        Print::Fail("[HTTP] Failed to parse response");
        return "";
    }
    
    _success = true;
    Print::Ok("[HTTP] Request completed successfully");
    return body;
}

bool HTTPClient::parseUrl(const std::string& url, std::string& host, int& port, std::string& path)
{
    std::string cleanUrl = url;
    
    // STEP 2.1: Remove protocol prefix if present
    if (cleanUrl.find("http://") == 0)
    {
        cleanUrl = cleanUrl.substr(7);  // Remove "http://"
    }
    else if (cleanUrl.find("https://") == 0)
    {
        Print::Warn("[HTTP] HTTPS not supported, treating as HTTP");
        cleanUrl = cleanUrl.substr(8);  // Remove "https://"
    }
    
    // STEP 2.2: Separate host from path using first slash
    size_t slashPos = cleanUrl.find('/');
    if (slashPos == std::string::npos)
    {
        host = cleanUrl;        // No path, host only
        path = "/";            // Default root path
    }
    else
    {
        host = cleanUrl.substr(0, slashPos);      // Everything before slash
        path = cleanUrl.substr(slashPos);         // Everything from slash onwards
    }
    
    // STEP 2.3: Set default HTTP port
    port = 80;
    
    // STEP 2.4: Check for custom port in host (e.g., wttr.in:8080)
    size_t colonPos = host.find(':');
    if (colonPos != std::string::npos)
    {
        std::string portStr = host.substr(colonPos + 1);
        
        std::istringstream iss(portStr);
        int tempPort;
        if (iss >> tempPort && tempPort > 0 && tempPort <= 65535)
        {
            port = tempPort;
        }
        else
        {
            port = 80;  // Fallback to default if invalid
        }
        
        host = host.substr(0, colonPos);  // Remove port from host
    }
    
    return !host.empty() && !path.empty();
}

std::string HTTPClient::buildHttpRequest(const std::string& path, const std::string& host)
{
    std::ostringstream request;
    
    // STEP 4.1: HTTP request line (method, path, version)
    request << "GET " << path << " HTTP/1.1\r\n";
    
    // STEP 4.2: Required Host header (server identification)
    request << "Host: " << host << "\r\n";
    
    // STEP 4.3: User-Agent header
    request << "User-Agent: curl/7.0\r\n";  // ← Mudou de "ft_irc-bot/1.0" para "curl/7.0"
    
    // STEP 4.4: Accept header (content types we can handle)
    request << "Accept: text/plain, text/html, */*\r\n";
    
    // STEP 4.5: Connection header (close after response)
    request << "Connection: close\r\n";
    
    // STEP 4.6: Empty line (end of headers - required!)
    request << "\r\n";
    
    return request.str();
}

std::string HTTPClient::parseHttpResponse(const std::string& response)
{
    // STEP 7.1: Find separator between headers and body
    size_t headerEnd = response.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
    {
        _lastError = "Invalid HTTP response format";
        return "";
    }
    
    // STEP 7.2: Extract headers and body separately
    std::string headers = response.substr(0, headerEnd);
    std::string body = response.substr(headerEnd + 4);
    
    Print::Debug("[HTTP] Headers: " + headers.substr(0, headers.find('\r')));
    
    // STEP 7.3: Check HTTP status code
    if (headers.find("200 OK") != std::string::npos)
    {
        Print::Debug("[HTTP] Status: 200 OK");
    }
    else if (headers.find("404") != std::string::npos)
    {
        _lastError = "Resource not found (404)";
        Print::Warn("[HTTP] " + _lastError);
        return "";
    }
    else if (headers.find("500") != std::string::npos)
    {
        _lastError = "Server error (500)";
        Print::Warn("[HTTP] " + _lastError);
        return "";
    }
    else
    {
        Print::Warn("[HTTP] Non-200 status code, but continuing...");
    }
    
    // STEP 7.4: Clean body content (remove trailing whitespace)
    while (!body.empty() && (body[body.length()-1] == '\r' || 
                             body[body.length()-1] == '\n' || 
                             body[body.length()-1] == ' '))
    {
        body.erase(body.length()-1);
    }
    
    Print::Debug("[HTTP] Body length: " + toString(body.length()));
    if (!body.empty())
    {
        Print::Debug("[HTTP] Body preview: " + body.substr(0, std::min(body.length(), size_t(100))));
    }
    
    return body;
}

bool HTTPClient::connectToHost(const std::string& host, int port)
{
    Print::Debug("[HTTP] Resolving host: " + host);
    
    // STEP 3.1: Create TCP socket
    if (!_socket.create(AF_INET, SOCK_STREAM, 0))
    {
        _lastError = "Failed to create socket: " + _socket.getLastError();
        return false;
    }
    
    // STEP 3.2: Set socket to non-blocking for timeout control
    if (!_socket.setNonBlocking())
    {
        _lastError = "Failed to set non-blocking mode";
        return false;
    }
    
    // STEP 3.3: Establish TCP connection to server
    bool connected = _socket.connect(host, port);
    if (!connected)
    {
        _lastError = "Failed to connect: " + _socket.getLastError();
        return false;
    }
    
    Print::Debug("[HTTP] Connected to " + host + ":" + toString(port));
    return true;
}

void HTTPClient::cleanup()
{
    if (_socket.isValid())
    {
        _socket.close();
    }
}

std::string HTTPClient::urlEncode(const std::string& str)
{
    std::string encoded;
    for (size_t i = 0; i < str.length(); ++i)
    {
        char c = str[i];
        if (c == ' ')
        {
            encoded += "%20";
        }
        else if (c == '+')
        {
            encoded += "%2B";
        }
        else if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            encoded += c;
        }
        else
        {
            // Simple encoding for special characters
            std::ostringstream oss;
            oss << "%" << std::hex << (unsigned char)c;
            encoded += oss.str();
        }
    }
    return encoded;
}

// Public methods
bool HTTPClient::isSuccess() const
{
    return _success;
}

std::string HTTPClient::getLastError() const
{
    return _lastError;
}

void HTTPClient::setTimeout(int seconds)
{
    _timeout = (seconds > 0) ? seconds : 5;
}

int HTTPClient::getTimeout() const
{
    return _timeout;
}

// Static utility methods
std::string HTTPClient::encodeUrl(const std::string& str)
{
    HTTPClient temp;
    return temp.urlEncode(str);
}

bool HTTPClient::isValidUrl(const std::string& url)
{
    if (url.empty())
    {
        return false;
    }
    
    // Basic URL validation
    if (url.find("http://") == 0 || url.find("https://") == 0)
    {
        return true;
    }
    
    // Allow URLs without protocol
    if (url.find("://") == std::string::npos && url.find('.') != std::string::npos)
    {
        return true;
    }
    
    return false;
}