#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include <ctime>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "General.hpp"

class Print
{
public:
	static void Debug(const std::string& str, bool command = 0);
	static void Log(const std::string& str);
	static void StdOut(const std::string& str);
	static void StdErr(const std::string& str);
	static void Stream(std::ofstream& os, const std::string& str);
	static void Timestamp(const std::string& color);
	// enhaced-log
	static void Do(const std::string& str);
	static void Ok(const std::string& str);
	static void Warn(const std::string& str);
	static void Fail(const std::string& str);

private:
	Print();
};

template <typename T>
std::string toString(T value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}
template <typename T>
int toInt(T str)
{
	std::stringstream ss;
	ss << str;
	int value;
	try
	{
		ss >> value;
	}
	catch (std::exception& e)
	{
		return -1;
	}
	return value;
}
#endif  // PRINT_UTILS_H
