#ifndef BOTCONTEXT_HPP
#define BOTCONTEXT_HPP
#pragma once
#include "Bot.hpp"
class BotContext
{
private:
    Bot* _bot;
    std::string _channel;
    std::string _originalMessage;

public:
    BotContext(Bot* bot, std::string& channel, std::string& originalMessage);
    ~BotContext();
    void reply(const std::string& message) const;
};
#endif  // !BOTCONTEXT_HPP
