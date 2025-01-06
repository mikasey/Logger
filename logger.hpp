#ifndef LOGGER_HPP
#define LOGGER_HPP

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

class Logger{
public:
    enum class MessageType{
        DEBUG,
        NOTICE,
        INFO,
        WARNING,
        ERROR,
        CRIT_ERROR
    };

    enum class Color{
        BLACK = 0, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, BRIGHT_GRAY, GRAY,
        BRIGHT_RED, BRIGHT_GREEN, BRIGHT_YELLOW, BRIGHT_BLUE, BRIGHT_MAGENTA,
        BRIGHT_CYAN, BRIGHT_WHITE, COUNT = 16
    };

    struct LoggerProperties{
        std::string file_name;
        bool console_output = true;
        bool file_output = true;
        bool append_mode = true;
        std::unordered_map<MessageType, Color> message_colors = {
            {MessageType::DEBUG, Color::GREEN},
            {MessageType::NOTICE, Color::BRIGHT_GRAY},
            {MessageType::INFO, Color::CYAN},
            {MessageType::WARNING, Color::YELLOW},
            {MessageType::ERROR, Color::RED},
            {MessageType::CRIT_ERROR, Color::BRIGHT_RED}
        };
    };

    struct Message{
        std::string logger_name;
        MessageType type;
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
    static void LogMessage(const std::string& logger_name, const std::string& msg_format, MessageType msg_type, ...);
    static void FlushMessages();
    static void Stop();

private:
    static std::string MessageTypeToString(MessageType type);

    static std::unordered_map<std::string, LoggerProperties> loggers;
    static std::unordered_map<std::string, std::ofstream> file_streams;
    static std::vector<Message> messages;
    static std::string logs_dir;
    static std::mutex logger_mutex;

    static const char* color_table[];
};

#endif // LOGGER_HPP
