#ifndef CONFIG_H
#define CONFIG_H

#include <algorithm>
#include <iostream>
#include <sstream>
#include "General.hpp"
#include "UtilsFun.hpp"
#include <string>

class Config
{
private:
    Config();
    
public:
    static bool hasConfig(const std::string& wanted);
    static std::string  getConfig(const std::string& wanted);

};

#endif
