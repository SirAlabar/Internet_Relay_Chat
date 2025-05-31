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
