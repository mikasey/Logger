#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>
#include <ctime>
#include <cstdarg>

    // In games use Write...() macros. They can be disabled at compile time by defining SFGE_CLIENT_NO_LOGGING.
    // In engine use Log...() macros. Log() operates only on console, and does not need initialization.
    class Logger {
    public:
        Logger() = delete;
        ~Logger() = delete;

        // File logging initialization
        static int Init();

        enum Color {
            black = 0,
            red, green, yellow, blue, magenta, cyan, bright_gray, gray,
            bright_red, bright_green, bright_yellow, bright_blue, bright_magenta, bright_cyan,
            bright_white,
            color_count
        };

        // Not intended for production use, use Log() macros instead.
        static void _log_console(const char* prefex, Color color, const char* msg_format, ...);

    private:
        static const char* color_table[color_count];
        static const char* dt_format;
        static const char* log_format;
        static const size_t max_msg_size;
        static const size_t max_time_str_size;
    };

// Core logger interface, only console
#define Log(PREFIX, COLOR, MESSAGE, ...)\
    Logger::_log_console( PREFIX, COLOR, MESSAGE, ##__VA_ARGS__)

#ifdef NO_LOGGING
#define CORE_NO_LOGGING
#define CLIENT_NO_LOGGING
#endif
#ifndef CORE_NO_LOGGING
    // Engine logger interface
#define LogInfo(MESSAGE, ...) \
        Logger::_log_console("Engine <info>", Logger::green, MESSAGE, ##__VA_ARGS__)
    // Client warning loggger
#define LogWarning(MESSAGE, ...) \
        Logger::_log_console("Engine <warning>", Logger::yellow, MESSAGE, ##__VA_ARGS__)
    // Client error logger
#define LogERROR(MESSAGE, ...) \
        Logger::_log_console("Engine <ERROR>", Logger::red, MESSAGE, ##__VA_ARGS__)
#else 
#define LogInfo() 
#define LogWarning() 
#define LogERROR()
#endif // NO_LOGGING

#ifndef CLIENT_NO_LOGGING
    // Client info logger
#define WriteInfo(MESSAGE, ...) \
        Logger::_log_console("Game <info>", SFGE::Logger::bright_green, MESSAGE, ##__VA_ARGS__)
    // Client warning loggger
#define WriteWarning(MESSAGE, ...) \
        Logger::_log_console("Game <warning>", SFGE::Logger::bright_yellow, MESSAGE, ##__VA_ARGS__)
    // Client error logger
#define WriteERROR(MESSAGE, ...) \
        Logger::_log_console("Game <ERROR>", SFGE::Logger::bright_red, MESSAGE, ##__VA_ARGS__)
#else
#define WriteInfo()
#define WriteWarning()
#define WriteERROR()
#endif // CLIENT_NO_LOGGING

#endif //LOGGER_H