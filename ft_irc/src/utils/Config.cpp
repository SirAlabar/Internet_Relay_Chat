#include "Config.hpp"

bool    Config::hasConfig(const std::string& wanted)
{
	if (getConfig(wanted) == "")
	{
		return false;
	}
	return true;
}

std::string Config::getConfig(const std::string& wanted)
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
		if (line.empty() || line[0] == '#' || line.find_first_of("\t\n\v\f\r ") != std::string::npos)
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

Config::Config() {}
