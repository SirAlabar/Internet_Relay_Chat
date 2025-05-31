#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <arpa/inet.h>   // (inet_addr, inet_ntoa)
#include <fcntl.h>       // (fcntl)
#include <netdb.h>       // (getaddrinfo, gethostbyname)
#include <netinet/in.h>  // (sockaddr_in)
#include <poll.h>        // (poll())
#include <sys/socket.h>  // (socker,bind, listen, accept, connect)
#include <unistd.h>      // (close, read, write)

#include <cerrno>
#include <cstring>  // (memset, strcpy)~
#include <iostream>
#include <string>
#include <vector>  // STL container

class Socket
{
private:
	int _fd;
	int _lastError;
	bool _blocking;
	struct sockaddr_in _addr;

public:
	Socket();
	Socket(int domain, int type, int protocol);
	Socket(const Socket& other);
	Socket& operator=(const Socket& other);
	~Socket();

	bool create(int domain, int type, int protocol);
	bool setOption(int level, int optname, const void* optval, socklen_t optlen);
	bool setNonBlocking();
	bool bind(int port, const std::string& address = "");
	bool listen(int backlog);
	bool connect(const std::string& host, int port);
	Socket accept();
	ssize_t send(const std::string& data, int flags = 0);
	ssize_t recv(char* buffer, size_t buffersize, int flags = 0);

	int getFd() const;
	bool isValid() const;
	void close();
	std::string getLastError() const;
};

#endif
