#pragma once
#ifndef APPLOGGER_HPP
#define APPLOGGER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <mutex>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <array>

class AppLogger{
public:
	enum class Color{
		BLACK = 0, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, GRAY, BRIGHT_BLACK,
		BRIGHT_RED, BRIGHT_GREEN, BRIGHT_YELLOW, BRIGHT_BLUE, BRIGHT_MAGENTA,
		BRIGHT_CYAN, WHITE
	};
	enum class LogLevel{
		NONE = 0,
		CRITICAL = 1,
		ERROR = 1 << 1,
		WARNING = 1 << 2,
		INFO = 1 << 3,
		NOTICE = 1 << 4,
		DEBUG = 1 << 5,

		ERROR_PLUS = CRITICAL | ERROR,
		WARNING_PLUS = ERROR_PLUS | WARNING,
		INFO_PLUS = WARNING_PLUS | INFO,
		NO_DEBUG = INFO_PLUS | NOTICE,
		ALL = 0xff
	};
	enum class MsgType{
		CRITICAL = (uint8_t)LogLevel::CRITICAL,
		ERROR = (uint8_t)LogLevel::ERROR,
		WARNING = (uint8_t)LogLevel::WARNING,
		INFO = (uint8_t)LogLevel::INFO,
		NOTICE = (uint8_t)LogLevel::NOTICE,
		DEBUG = (uint8_t)LogLevel::DEBUG
	};

	struct LoggerProperties{
		bool console_output = true;
		bool file_output = true;
		bool append_mode = true;
		std::string file_name;
		LogLevel log_level = LogLevel::ALL;
		std::unordered_map<MsgType, Color> message_colors = {
			{MsgType::DEBUG, Color::GREEN},
			{MsgType::NOTICE, Color::GRAY},
			{MsgType::INFO, Color::CYAN},
			{MsgType::WARNING, Color::YELLOW},
			{MsgType::ERROR, Color::RED},
			{MsgType::CRITICAL, Color::WHITE}
		};
		std::unordered_map<MsgType, Color> message_bg_colors = {
			{MsgType::DEBUG, Color::BLACK},
			{MsgType::NOTICE, Color::BLACK},
			{MsgType::INFO, Color::BLACK},
			{MsgType::WARNING, Color::BLACK},
			{MsgType::ERROR, Color::BLACK},
			{MsgType::CRITICAL, Color::RED}
		};
	};

	struct Message{
		std::string logger_name;
		MsgType type = MsgType::NOTICE;
		std::string body;
		std::chrono::system_clock::time_point timestamp_cpp;

		bool operator<(const Message& other) const{
			return timestamp_cpp < other.timestamp_cpp;
		}
		bool operator>(const Message& other) const{
			return timestamp_cpp > other.timestamp_cpp;
		}
	};

	static int Init(const std::string& logs_directory);
	static int AddLogger(const std::string& logger_name, const LoggerProperties& props);
	static void SetLoggerLevel(const std::string& name, LogLevel lvl);
	static inline LogLevel GetLoggerLevel(const std::string& name){ return loggers[name].log_level; };
	static void LogMessage(const std::string& logger_name, const std::string& msg_format, MsgType msg_type, ...);
	static void FlushMessages();
	static void Stop();

private:
	static std::string MessageTypeToString(MsgType type);
	static std::string FormatForConsole(std::string msg, Color fg_color, Color bg_color);
	static int MicrosoftSucksAss();

	static std::unordered_map<std::string, LoggerProperties> loggers;
	static std::unordered_map<std::string, std::ofstream> file_streams;
	static std::vector<Message> messages;
	static std::string logs_dir;
	static std::mutex logger_mutex;

	static const char* color_table[];
	static const char* bg_color_table[];
	static const char* es_reset;
};

#define LogDebug( LOG_NAME, LOG_STRING, ...)\
AppLogger::LogMessage(LOG_NAME, LOG_STRING, AppLogger::MsgType::DEBUG, ##__VA_ARGS__)
#define LogNote( LOG_NAME, LOG_STRING, ...)\
AppLogger::LogMessage(LOG_NAME, LOG_STRING, AppLogger::MsgType::NOTICE, ##__VA_ARGS__)
#define LogInfo( LOG_NAME, LOG_STRING, ...)\
AppLogger::LogMessage(LOG_NAME, LOG_STRING, AppLogger::MsgType::INFO, ##__VA_ARGS__)
#define LogWarn( LOG_NAME, LOG_STRING, ...)\
AppLogger::LogMessage(LOG_NAME, LOG_STRING, AppLogger::MsgType::WARNING, ##__VA_ARGS__)
#define LogError( LOG_NAME, LOG_STRING, ...)\
AppLogger::LogMessage(LOG_NAME, LOG_STRING, AppLogger::MsgType::ERROR, ##__VA_ARGS__)
#define LogCrit( LOG_NAME, LOG_STRING, ...)\
AppLogger::LogMessage(LOG_NAME, LOG_STRING, AppLogger::MsgType::CRITICAL, ##__VA_ARGS__)

#endif // APPLOGGER_HPP
