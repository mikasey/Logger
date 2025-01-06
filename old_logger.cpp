#include "old_logger.hpp"
namespace SFGE{
int Logger::Init(){
    return 0;
}

const char* Logger::color_table[Logger::color_count] = {
    "\x1b[30m", // black
    "\x1b[31m", // red
    "\x1b[32m", // green
    "\x1b[33m", // yellow
    "\x1b[34m", // blue
    "\x1b[35m", // magenta
    "\x1b[36m", // cyan
    "\x1b[37m", // bright_gray
    "\x1b[90m", // gray
    "\x1b[91m", // bright_red
    "\x1b[92m", // bright_green
    "\x1b[93m", // bright_yellow
    "\x1b[94m", // bright_blue
    "\x1b[95m", // bright_magenta
    "\x1b[96m", // bright_cyan
    "\x1b[97m", // bright_white
};

const char* Logger::dt_format = "%d.%m.%y|%H:%M:%S";
const char* Logger::log_format = "%s[%s] %s: %s \033[0m\n";
const size_t Logger::max_msg_size = 1024;
const size_t Logger::max_time_str_size = 64;

void Logger::_log_console(const char* prefix, Color color, const char* msg_format, ...){
    char msg[max_msg_size];
    char time_str[max_time_str_size];
    time_t time_raw; time(&time_raw);
    static struct tm time_tm = {};
    localtime_s(&time_tm, &time_raw);
    strftime(time_str, max_time_str_size, dt_format, &time_tm);

    // Variable args magic
    va_list args;
    va_start(args, msg_format);
    vsprintf_s(msg, msg_format, args);
    va_end(args);

    printf(log_format, color_table[color], time_str, prefix, msg);
}
}