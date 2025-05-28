#ifndef GENERAL_HPP
#define GENERAL_HPP

#ifndef DEBUG
#define DEBUG 0
#endif	// !DEBUG

#ifndef LOG
#define LOG 1
#endif	// !LOG

#include <string>
namespace Color {
	const std::string RESET   = "\033[0m";
	const std::string RED     = "\033[38;5;196m";
	const std::string ORANGE  = "\033[38;5;208m";
	const std::string YELLOW  = "\033[38;5;226m";
	const std::string GREEN   = "\033[38;5;46m";
	const std::string BLUE    = "\033[38;5;21m";
	const std::string INDIGO  = "\033[38;5;57m";
	const std::string VIOLET  = "\033[38;5;129m";
}

namespace IRC
{
    // Welcome messages (001-004)
    const int RPL_WELCOME           = 001;
    const int RPL_YOURHOST          = 002;
    const int RPL_CREATED           = 003;
    const int RPL_MYINFO            = 004;

    // Channel information replies
    const int RPL_CHANNELMODEIS     = 324;
    const int RPL_NOTOPIC           = 331;
    const int RPL_TOPIC             = 332;
    const int RPL_INVITING          = 341;
    const int RPL_NAMREPLY          = 353;
    const int RPL_ENDOFNAMES        = 366;

    // Connection/Target errors
    const int ERR_NOSUCHNICK        = 401;
    const int ERR_NOSUCHCHANNEL     = 403;
    const int ERR_CANNOTSENDTOCHAN  = 404;
    const int ERR_UNKNOWNCOMMAND    = 421;

    // Nickname errors
    const int ERR_NONICKNAMEGIVEN   = 431;
    const int ERR_ERRONEUSNICKNAME  = 432;
    const int ERR_NICKNAMEINUSE     = 433;

    // User/Channel errors
    const int ERR_USERNOTINCHANNEL  = 441;
    const int ERR_NOTONCHANNEL      = 442;
    const int ERR_USERONCHANNEL     = 443;

    // Authentication errors
    const int ERR_NOTREGISTERED     = 451;

    // Command parameter errors
    const int ERR_NEEDMOREPARAMS    = 461;
    const int ERR_ALREADYREGISTRED  = 462;
    const int ERR_PASSWDMISMATCH    = 464;
    const int ERR_KEYSET            = 467;

    // Channel operation errors
    const int ERR_CHANNELISFULL     = 471;
    const int ERR_UNKNOWNMODE       = 472;
    const int ERR_INVITEONLYCHAN    = 473;
    const int ERR_BADCHANNELKEY     = 475;

    // Operator privilege errors
    const int ERR_CHANOPRIVSNEEDED  = 482;
    const int ERR_INVALIDLIMIT      = 696;

    //MOTD related replyes
    const int RPL_MOTDSTART         = 375;
    const int RPL_MOTD              = 372;
    const int RPL_ENDOFMOTD         = 376;
    const int ERR_NOMOTD            = 422;
}

#endif	// !GENERAL_HPP
