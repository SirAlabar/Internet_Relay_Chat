#include <algorithm>
#include <iostream>
#include <sstream>

#include "UtilsFun.hpp"
#include "General.hpp"

void Print::Debug(const std::string& str, bool command)
{
    if (DEBUG || command)
    {
        Print::Timestamp(command ? Color::GREEN : Color::RED);
        std::cerr << str << std::endl;
    }
}

void Print::Log(const std::string& str)
{
    if (LOG)
    {
        Print::Timestamp(Color::BLUE);
        std::cerr << str << std::endl;
    }
}

void Print::StdOut(const std::string& str) { std::cout << str << std::endl; }

void Print::StdErr(const std::string& str) { std::cerr << str << std::endl; }

void Print::Stream(std::ofstream& os, const std::string& str) { os << str << std::endl; }

void    Print::Timestamp(const std::string& color)
{
    std::time_t	current_time_in_seconds = std::time(NULL);
    std::tm		*time_struct = std::localtime(&current_time_in_seconds);
    char originalFill = std::cout.fill();
    std::cout << color
        << "[" 
        << std::setfill('0') << std::right << std::setw(2) << time_struct->tm_hour << ":"
        << std::setfill('0') << std::right << std::setw(2) << time_struct->tm_min << ":"
        << std::setfill('0') << std::right << std::setw(2) << time_struct->tm_sec
        << "] " << Color::RESET;
    std::cout.fill(originalFill);
}

void    Print::Do(const std::string& str)
{
    if (LOG)
    {
        Print::Timestamp(Color::GREEN);
        std::cout << std::left << std::setw(35) << str ;
        std::cout << std::setw(0);
    }
}

void    Print::Ok(const std::string& str)
{
    if (LOG)
    {
        std::cout << std::setw(9) << Color::GREEN << "[  OK  ] ";
        std::cout << Color::RESET << str << std::endl;
    }
}

void    Print::Warn(const std::string& str)
{
    if (LOG)
    {
        std::cout << std::setw(9) << Color::YELLOW << "[ WARN ] ";
        std::cout << Color::RESET << str << std::endl;
    }
}

void    Print::Fail(const std::string& str)
{
    if (LOG)
    {
        std::cout << std::setw(9) << Color::RED << "[ FAIL ] ";
        std::cout << Color::RESET << str << std::endl;
    }
}

Print::Print() {}

bool    Print::hasConfig(const std::string& wanted)
{
    (void) wanted;
    return true;
}

std::string Print::getConfig(const std::string& wanted)
{
    if(wanted.empty())
    {
        return("");
    }
    const char* config_file = "config.txt";
    std::string source_file(config_file);
    std::ifstream file(config_file);
    if (!file.is_open())
    {
        return ("");
    }

    std::string line;
    while (std::getline(file, line))
    {
        line.erase(0, line.find_first_not_of("\t\n\v\f\r "));
        line.erase(line.find_last_not_of("\t\n\v\f\r ") + 1);
        if (line.empty() || line[0] == '#')
        {
            continue;
        }
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos)
        {
            continue;
        }
        std::string varName = line.substr(0, equalPos);
        if (varName != wanted)
        {
            continue ;
        }
        if(equalPos + 1 < line.length())
        {
            file.close();
            return (line.substr(equalPos + 1));
        }
        else
        {
            file.close();
            return("");
        }
    }
    file.close();
    return("");
}
