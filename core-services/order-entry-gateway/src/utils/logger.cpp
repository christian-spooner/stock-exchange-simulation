#include "utils/logger.hpp"

Logger& Logger::get_instance() {
	static Logger instance;
	return instance;
}

Logger::Logger() {}

Logger::~Logger() {
	if (logFile.is_open()) {
		logFile.close();
	}
}

void Logger::initialize(const std::string& logFilePath, LogLevel level) {
	logLevel = level;
	logFile.open(logFilePath);
	if (!logFile.is_open()) {
		std::cerr << "Error: Failed to open log file: " << logFilePath << std::endl;
	}
}

void Logger::log(LogLevel level, const std::string& message) {
	if (static_cast<int>(level) >= static_cast<int>(logLevel)) {
		std::string levelString;
		switch (level) {
			case LogLevel::DEBUG:
				levelString = "DEBUG";
				break;
			case LogLevel::INFO:
				levelString = "INFO";
				break;
			case LogLevel::WARNING:
				levelString = "WARNING";
				break;
			case LogLevel::ERROR:
				levelString = "ERROR";
				break;
			case LogLevel::FATAL:
				levelString = "FATAL";
				break;
		}

		logFile << "[" << levelString << "] " << message << std::endl;
	}
}

Logger& logger = Logger::get_instance();
