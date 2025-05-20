#include "Logger.h"
#include <iostream>
#include <chrono>
#include <string>
#include <ctime>


std::vector<LogEntry> Logger::messages; //need to define what was declared in the header file


std::string CurrentDateTimeToString()
{
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string output(30, '\0');
	struct tm timeinfo;
	localtime_s(&timeinfo, &now);
	std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", &timeinfo);
	return output;
}

//time_t - the "_t" usually shows it's a C data type
// strftime - can look at online documentation for what each "%" does



void Logger::Log(const std::string& message)
{
	LogEntry logEntry;
	logEntry.type = LOG_INFO;
	logEntry.message = "LOG: [" + CurrentDateTimeToString() + "]: " + message;
	std::cout << "\x1B[32m" << logEntry.message << "\033[0m" << std::endl;
	messages.push_back(logEntry);
}
// \x1B[32m - changes colour to green
// |033[0m - disable colour mentioned previously

void Logger::Err(const std::string& message)
{
	LogEntry logEntry;
	logEntry.type = LOG_ERROR;
	logEntry.message = "ERR: [" + CurrentDateTimeToString() + "]: " + message;
	messages.push_back(logEntry);
	std::cerr << "\x1B[91m" << logEntry.message << "\033[0m" << std::endl;
}
