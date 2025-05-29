#ifndef PRINT_DATA_COMMAND_HPP
#define PRINT_DATA_COMMAND_HPP

#include "ACommand.hpp"

class Client;
class Server;
class Message;

class PrintdataCommand : public ACommand
{
private:
    // Private to prevent copying
    PrintdataCommand(const PrintdataCommand& other);
    PrintdataCommand& operator=(const PrintdataCommand& other);

public:
    PrintdataCommand(Server* server);
    virtual ~PrintdataCommand();

    // Execute the JOIN command
    virtual void execute(Client* client, const Message& message);

    // Static creator for factory
    static ACommand* create(Server* server);
};

#endif
