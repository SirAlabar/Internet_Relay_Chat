#include <iostream>
#include <sstream>

#include "UtilsFun.hpp"
#include "general.hpp"

void Print::Debug(const std::string& str)
{
    if (DEBUG) std::cerr << RED << "[DEBUG]: " << RESET << str << std::endl;
}

void Print::Log(const std::string& str)
{
    if (LOG) std::cerr << RED << "[LOG]: " << RESET << str << std::endl;
}

void Print::StdOut(const std::string& str) { std::cout << str << std::endl; }

void Print::StdErr(const std::string& str) { std::cerr << str << std::endl; }

void Print::Stream(std::ofstream& os, const std::string& str) { os << str << std::endl; }

// Private constructor definition (optional, but good practice)
Print::Print() {}
