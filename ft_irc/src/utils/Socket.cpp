# include "Socket.hpp"

Socket::Socket() : _fd(-1), _lastError(0), _blocking(true), _addr()
{}

Socket::Socket(int domain, int type, int protocol) : _fd(-1), _lastError(0), _blocking(true), _addr()
{
    create(domain, type, protocol);
}

Socket::Socket(const Socket& other) : _fd(-1), _lastError(0), _blocking(true), _addr()
{
    *this = other;
}

Socket::Socket& operator=(const Socket& other)
{
    if (this != &other)
    {
        if (isValid())
        {
            ::close(_fd);
        }
        //Only copy the socket if its invalid, otherwise create a new one
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
    }
}

bool Socket::create(int domain, int type, int protocol)
{
    //RAII (Resource Acquisition Is Initialization) pattern
    if (isValid())
    {
        ::close(_fd);
    }
    _fd = socket(domain, type, protocol);
    _lastError = errno;

    return (isValid());
}

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



bool setNonBlocking();
bool bind(int port, const std::string& address = "");
bool listen(int backlog);
bool connect(const std::string& host, int port);

Socket accept();

ssize_t send(const std::string& data, int flags = 0);
ssize_t recv(char *buffer, size_t buffersize, int flags = 0);
bool wouldBlock() const;

int getFd() const;
bool isValid() const;
void close();
std::string getLastError() const;
};

