#include "Server.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cstring>

// Global server instance for signal handling
Server* g_server = NULL;

// Signal handler for clean shutdown
void sigHandler(int signum)
{
    std::cout << "\nReceived signal " << signum << ". Shutting down server..." << std::endl;
    if (g_server)
    {
        g_server->stop();
    }
    exit(signum);
}

int main(int argc, char* argv[])
{
    // Check command-line arguments
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    
    // Parse port and password
    int port = atoi(argv[1]);
    std::string password = argv[2];
    
    // Validate port
    if (port <= 0 || port > 65535)
    {
        std::cerr << "Invalid port number. Must be between 1 and 65535." << std::endl;
        return 1;
    }
    
    // Setup signal handlers for clean shutdown
    signal(SIGINT, sigHandler);  // Ctrl+C
    signal(SIGTERM, sigHandler); // kill command
    
    // Create and start server
    Server server;
    g_server = &server;
    
    std::cout << "Starting IRC server on port " << port << std::endl;
    
    if (!server.start(port, password))
    {
        std::cerr << "Failed to start server. Exiting." << std::endl;
        return 1;
    }
    
    std::cout << "Server started successfully. Waiting for connections..." << std::endl;
    
    // Run the server main loop
    server.run();
    
    // If we exit the loop normally, clean up
    server.stop();
    
    return 0;
}