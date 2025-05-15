#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include <iostream>
#include <fstream>
#include <string>

#define RED "\033[31m"
#define RESET "\033[0m"

class Print {
public:
    static void Debug(const std::string& str);
    static void Log(const std::string& str);
    static void StdOut(const std::string& str);
    static void StdErr(const std::string& str);
    static void Stream(std::ofstream& os, const std::string& str);

private:
    Print();
};

#endif // PRINT_UTILS_H
