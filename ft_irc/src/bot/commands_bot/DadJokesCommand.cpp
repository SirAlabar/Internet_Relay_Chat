#include <iostream>

#include "BotContext.hpp"
#include "Client.hpp"
#include "DadJokesCommand.hpp"
#include "Message.hpp"
#include "Server.hpp"

DadJokesCommand::DadJokesCommand(Server* server) : ABotCommand(server) {}

DadJokesCommand::~DadJokesCommand() {}

// private, not used
DadJokesCommand::DadJokesCommand(const DadJokesCommand& other)
    : ABotCommand(other._server)
{
}

DadJokesCommand& DadJokesCommand::operator=(const DadJokesCommand& other)
{
    if (this != &other)
    {
        _server = other._server;
    }
    return (*this);
}

// Static create method for factory
ABotCommand* DadJokesCommand::create(Server* server)
{
    return (new DadJokesCommand(server));
}

// Execute the DadJokes command
void DadJokesCommand::execute(BotContext* botctx, std::string& message)
{
    if (!botctx)
    {
        return;
    }
    (void)message;

    const std::string dadJokes[] = {
        "Why don't scientists trust atoms? Because they make up everything!",
        "I'm reading a book about anti-gravity. It's impossible to put down!",
        "Why did the scarecrow win an award? He was outstanding in his field!",
        "I told my wife she was drawing her eyebrows too high. She looked surprised.",
        "What do you call a fake noodle? An impasta!",
        "Why don't eggs tell jokes? They'd crack each other up!",
        "I used to hate facial hair, but then it grew on me.",
        "What do you call a bear with no teeth? A gummy bear!",
        "Why can't a bicycle stand up by itself? It's two tired!",
        "I'm terrified of elevators, so I'll start taking steps to avoid them.",
        "Why did the coffee file a police report? It got mugged!",
        "What do you call a fish wearing a crown? A king fish!",
        "I only know 25 letters of the alphabet. I don't know Y.",
        "What do you call a sleeping bull? A bulldozer!",
        "Why don't skeletons fight each other? They don't have the guts.",
        "What's orange and sounds like a parrot? A carrot!",
        "Why did the math book look so sad? Because it had too many problems.",
        "What do you call a pig that does karate? A pork chop!",
        "I'm afraid for the calendar. Its days are numbered.",
        "What do you call a dog magician? A labracadabrador!",
        "Why don't oysters share? Because they're shellfish!",
        "What did the ocean say to the beach? Nothing, it just waved.",
        "Why did the cookie go to the doctor? Because it felt crumbly!",
        "What do you call a dinosaur that crashes his car? Tyrannosaurus Wrecks!"};

    static const size_t dadJokesSize = sizeof(dadJokes) / sizeof(dadJokes[0]);
    int random = rand() % dadJokesSize;
    botctx->reply(dadJokes[random]);
    Print::Do("executing DadJokes command");
}
