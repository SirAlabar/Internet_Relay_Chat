#ifndef GENERAL_HPP
#define GENERAL_HPP

#ifndef DEBUG
#define DEBUG 0
#endif  // !DEBUG

#ifndef LOG
#define LOG 0
#endif	// !LOG

#include <string>
namespace Color {
	const std::string RESET   = "\033[0m";
	const std::string RED     = "\033[38;5;196m";
	const std::string ORANGE  = "\033[38;5;208m";
	const std::string YELLOW  = "\033[38;5;226m";
	const std::string GREEN   = "\033[38;5;46m";
	const std::string BLUE    = "\033[38;5;21m";
	const std::string INDIGO  = "\033[38;5;57m";
	const std::string VIOLET  = "\033[38;5;129m";
}

#endif  // !GENERAL_HPP
