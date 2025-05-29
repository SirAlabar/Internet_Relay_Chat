#include "MotdCommand.hpp"

MotdCommand::MotdCommand(Server* server) : ACommand(server) {}

MotdCommand::~MotdCommand() {}

MotdCommand::MotdCommand(const MotdCommand& other) : ACommand(other._server) {}

MotdCommand& MotdCommand::operator=(const MotdCommand& other)
{
	if (this != &other)
	{
		_server = other._server;
	}
	return (*this);
}

ACommand* MotdCommand::create(Server* server)
{
	return (new MotdCommand(server));
}

void MotdCommand::execute(Client* client, const Message& message)
{
	Print::Do("execute MOTD command");
    if (!validateClientRegist(client))
    {
        return;
    }
    (void)message;
    sendNumericReply(client, IRC::RPL_MOTDSTART, ":- server Message of the day -");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":+==============================================================+");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":|                   Welcome to ft_irc!                         |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":|                                                              |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":| ESSENTIAL COMMANDS                                           |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":| /join #channel          - Join a channel                     |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":| /msg nickname message   - Send private message               |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":| /list                   - Show available channels            |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":|                                                              |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":| CHANNEL OPERATORS                                            |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":| /topic #channel [topic] - View/set channel topic             |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":| /mode #chan +/-o nick   - Give/take operator privilege       |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":| /kick #chan nick [msg]  - Remove user from channel           |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":| /invite nick #channel   - Invite user to channel             |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":|                                                              |");
    sendNumericReply(client, IRC::RPL_MOTD,
                     ":+============ Created by hluiz, isilva-t & joao-pol ===========+");
    sendNumericReply(client, IRC::RPL_ENDOFMOTD,
                     ":End of MOTD command.");	
    Print::Ok("MOTD command completed");
}
