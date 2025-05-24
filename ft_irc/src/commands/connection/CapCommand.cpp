#include "CapCommand.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "UtilsFun.hpp"

CapCommand::CapCommand(Server* server) : ACommand(server) {}

CapCommand::~CapCommand() {}

// Private copy constructor
CapCommand::CapCommand(const CapCommand& other) : ACommand(other._server) {}

CapCommand& CapCommand::operator=(const CapCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Static creator for factory
ACommand* CapCommand::create(Server* server) { return (new CapCommand(server)); }

void CapCommand::execute(Client* client, const Message& message)
{
    Print::Do("execute CAP command");

    if (!client)
    {
        Print::Fail("Client NULL");
        return;
    }

    std::string subcommand = message.getParams(0);
    
    if (subcommand.empty())
    {
        Print::Fail("no subcommand");
        sendErrorReply(client, 410, ":Invalid CAP command");
        return;
    }

    for (size_t i = 0; i < subcommand.size(); ++i)
    {
        subcommand[i] = toupper(subcommand[i]);
    }

    Print::Debug("CAP subcommand: " + subcommand);

    if (subcommand == "LS")
    {
        Print::Debug("Listing capabilities");
        // we don't support any capabilities
        std::string reply = ":server CAP * LS :\r\n";
        client->sendMessage(reply);
        Print::Ok("sent empty capability list");
    }
    else if (subcommand == "END")
    {
        Print::Debug("Ending capability negotiation");
        Print::Ok("capability negotiation ended");
    }
    else if (subcommand == "REQ")
    {
        Print::Debug("Client requesting capabilities");
        // Client requesting capabilities - we don't support any
        std::string reply = ":server CAP * NAK :";
        
        for (size_t i = 1; i < message.getSize(); ++i)
        {
            if (i > 1) reply += " ";
            reply += message.getParams(i);
        }
        reply += "\r\n";
        
        client->sendMessage(reply);
        Print::Warn("rejected all capability requests");
    }
    else if (subcommand == "LIST")
    {
        Print::Debug("Listing active capabilities");
        std::string reply = ":server CAP * LIST :\r\n";
        client->sendMessage(reply);
        Print::Ok("sent empty active capability list");
    }
    else
    {
        Print::Fail("unknown CAP subcommand: " + subcommand);
        sendErrorReply(client, 410, subcommand + " :Invalid CAP command");
    }
}