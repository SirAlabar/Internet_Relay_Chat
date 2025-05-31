#include "Bot.hpp"
#include "BotContext.hpp"

BotContext::BotContext(Bot* bot, std::string& channel, std::string& originalMessage)
	: _bot(bot), _channel(channel), _originalMessage(originalMessage)
{
}

BotContext::~BotContext() {}

void BotContext::reply(const std::string& message) const
{
	if (_bot && !_channel.empty())
	{
		std::string response = "PRIVMSG " + _channel + " :" + message + "\r\n";
		_bot->sendMessage(response);
	}
};
