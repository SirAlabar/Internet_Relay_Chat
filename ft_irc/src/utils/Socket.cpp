#include "Socket.hpp"
#include "UtilsFun.hpp"

Socket::Socket() : _fd(-1), _lastError(0), _blocking(true), _addr() {}

// Constructor with domain, type, protocol
Socket::Socket(int domain, int type, int protocol)
	: _fd(-1), _lastError(0), _blocking(true), _addr()
{
	create(domain, type, protocol);
}

Socket::Socket(const Socket& other) : _fd(-1), _lastError(0), _blocking(true), _addr()
{
	*this = other;
}

Socket& Socket::operator=(const Socket& other)
{
	if (this != &other)
	{
		if (isValid())
		{
			::close(_fd);
		}
		// Only copy the socket if its invalid, otherwise create a new one
		if (other.isValid())
		{
			_blocking = other._blocking;
			_addr = other._addr;
			create(AF_INET, SOCK_STREAM, 0);
			if (!_blocking)
			{
				setNonBlocking();
			}
		}
		else
		{
			_fd = -1;
			_blocking = other._blocking;
			_addr = other._addr;
		}
		_lastError = other._lastError;
	}
	return (*this);
}

Socket::~Socket()
{
	if (isValid())
	{
		::close(_fd);
		_fd = -1;
	}
}

// Create a socket
bool Socket::create(int domain, int type, int protocol)
{
	// RAII (Resource Acquisition Is Initialization) pattern
	if (isValid())
	{
		::close(_fd);
	}
	_fd = socket(domain, type, protocol);
	_lastError = errno;

	return (isValid());
}

// Set socket options
bool Socket::setOption(int level, int optname, const void* optval, socklen_t optlen)
{
	if (!isValid())
	{
		return (false);
	}

	int result = setsockopt(_fd, level, optname, optval, optlen);
	_lastError = errno;

	return (result == 0);
}

bool Socket::setNonBlocking()
{
	if (!isValid())
	{
		Print::StdErr("ERROR: Cannot set non-blocking mode on invalid socket");
		// std::cerr << "ERROR: Cannot set non-blocking mode on invalid socket" <<
		// std::endl;
		return (false);
	}

	Print::Debug("Setting socket " + toString(_fd) + " to non-blocking mode");
	// std::cout << "DEBUG: Setting socket " << _fd << " to non-blocking mode" <<
	// std::endl;

	int flags = fcntl(_fd, F_GETFL, 0);  // get socket fd flags
	if (flags == -1)
	{
		_lastError = errno;

		Print::StdErr("ERROR: fcntl(F_GETFL) failed: " + toString(strerror(_lastError)));
		// std::cerr << "ERROR: fcntl(F_GETFL) failed: " << strerror(_lastError)
		// << std::endl;

		return (false);
	}

	flags |= O_NONBLOCK;
	int result = fcntl(_fd, F_SETFL, flags);  // set non-bloking
	_lastError = errno;

	if (result == -1)
	{
		Print::StdErr("ERROR: fcntl(F_SETFL) failed: " + toString(strerror(_lastError)));
		// std::cerr << "ERROR: fcntl(F_SETFL) failed: " << strerror(_lastError)
		// 	  << std::endl;
		return (false);
	}

	Print::Debug("DEBUG: Successfully set socket " + toString(_fd) +
				 " to non-blocking mode");
	// std::cout << "DEBUG: Successfully set socket " << _fd << " to non-blocking mode"
	//    << std::endl;

	_blocking = false;
	return (true);
}

// Bind socket to address and port
bool Socket::bind(int port, const std::string& address)
{
	if (!isValid())
	{
		return (false);
	}

	_addr.sin_family = AF_INET;
	_addr.sin_port =
		htons(port);  // Host to Network Short (16-bit) for Architectures suport
	if (address.empty())
	{
		_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Host to Network Long (32-bit)
	}
	else
	{
		_addr.sin_addr.s_addr = inet_addr(address.c_str());
		if (_addr.sin_addr.s_addr == INADDR_NONE)
		{
			_lastError = errno;
			return (false);
		}
	}
	int result = ::bind(_fd, (struct sockaddr*)&_addr, sizeof(_addr));
	_lastError = errno;

	return (result != -1);
}

// Listen for connections
bool Socket::listen(int backlog)
{
	if (!isValid())
	{
		return (false);
	}

	int result = ::listen(_fd, backlog);
	_lastError = errno;

	return (result != -1);
}

// Accept a connection
Socket Socket::accept()
{
	Socket newSocket;

	if (!isValid())
	{
		return (newSocket);
	}
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);

	int clientFd = ::accept(_fd, (struct sockaddr*)&clientAddr, &clientAddrLen);
	_lastError = errno;
	if (clientFd >= 0)
	{
		newSocket._fd = clientFd;
		newSocket._addr = clientAddr;
		if (!_blocking)
		{
			newSocket.setNonBlocking();
		}
	}
	return (newSocket);
}

// Connect to a remote host
bool Socket::connect(const std::string& host, int port)
{
	if (!isValid())
	{
		return (false);
	}
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	// Try to resolve host as an IP address first
	_addr.sin_addr.s_addr = inet_addr(host.c_str());
	// If not an IP address, try to resolve as hostname
	if (_addr.sin_addr.s_addr == INADDR_NONE)
	{
		struct hostent* hostEntity = gethostbyname(host.c_str());
		if (hostEntity == NULL)
		{
			_lastError = h_errno;
			return (false);
		}
		_addr.sin_addr = *((struct in_addr*)hostEntity->h_addr);
	}
	int result = ::connect(_fd, (struct sockaddr*)&_addr, sizeof(_addr));
	_lastError = errno;
	// For non-blocking sockets, connect will return -1 with EINPROGRESS
	if (result == -1 && !_blocking && (errno == EINPROGRESS || errno == EWOULDBLOCK))
	{
		return (true);  // Connection in progress, client should check later
	}

	return (result != -1);
}

// Send data
ssize_t Socket::send(const std::string& data, int flags)
{
	if (!isValid())
	{
		return (-1);
	}

	ssize_t result = ::send(_fd, data.c_str(), data.size(), flags);
	_lastError = errno;

	return (result);
}

// Receive data
ssize_t Socket::recv(char* buffer, size_t buffersize, int flags)
{
	if (!isValid())
	{
		return (-1);
	}

	ssize_t result = ::recv(_fd, buffer, buffersize, flags);
	_lastError = errno;

	return (result);
}

// Get socket file descriptor
int Socket::getFd() const { return (_fd); }

// Check if socket is valid
bool Socket::isValid() const { return (_fd != -1); }

// Close the socket
void Socket::close()
{
	if (isValid())
	{
		::close(_fd);
		_fd = -1;
	}
}

// Get last error message
std::string Socket::getLastError() const { return (std::string(strerror(_lastError))); }
