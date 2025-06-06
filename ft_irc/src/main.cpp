
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Server.hpp"
#include "UtilsFun.hpp"

// Global server instance for signal handling
Server* g_server = NULL;
volatile bool g_shutdown_requested = false;

// Signal handler for clean shutdown
void sigHandler(int signum)
{
	Print::Log("\nReceived signal " + toString(signum) + ". Shutting down bot...");
	g_shutdown_requested = true;
}

int main(int argc, char* argv[])
{
	// Check command-line arguments
	if (argc != 3)
	{
		Print::StdErr("Usage: " + toString(argv[0]) + " <port> <password>");
		return 1;
	}

	// Parse port and password
	int port = atoi(argv[1]);
	std::string password = argv[2];

	// Validate port
	if (port <= 0 || port > 65535)
	{
		Print::StdErr("Invalid port number. Must be between 1 and 65535.");
		return 1;
	}

	// Setup signal handlers for clean shutdown
	signal(SIGINT, sigHandler);   // Ctrl+C
	signal(SIGTERM, sigHandler);  // kill command

	// Create and start server
	Server server;
	g_server = &server;

	Print::Do("Starting IRC server on port " + toString(port) + "              \n");

	if (!server.start(port, password))
	{
		Print::Fail("Failed to start server. Exiting...");
		return 1;
	}

	Print::Do("Server started. Waiting connections...\n");

	// Run the server main loop
	server.run();

	// If we exit the loop normally, clean up
	server.stop();
	g_server = NULL;

	return 0;
}
