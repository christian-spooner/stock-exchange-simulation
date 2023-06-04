#pragma once

#include <fstream>
#include <iostream>

enum class LogLevel { DEBUG, INFO, WARNING, ERROR, FATAL };

class Logger {
   public:
	static Logger& get_instance();

	void initialize(const std::string& logFilePath, LogLevel level);
	void log(LogLevel level, const std::string& message);

   private:
	Logger();
	~Logger();

	std::ofstream logFile;
	LogLevel logLevel;
};

extern Logger& logger;
