#include "Logger.hpp"
#include <cstdarg>
#include <algorithm>

// Color table for UNIX terminal color codes
const char* Logger::color_table[] = {
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
    "\x1b[97m"  // bright_white
};

std::unordered_map<std::string, Logger::LoggerProperties> Logger::loggers;
std::unordered_map<std::string, std::ofstream> Logger::file_streams;
std::vector<Logger::Message> Logger::messages;
std::string Logger::logs_dir;
std::mutex Logger::logger_mutex;

int Logger::Init(const std::string& logs_directory){
    logs_dir = logs_directory;
    try{
        if(!std::filesystem::exists(logs_dir)){
            std::filesystem::create_directories(logs_dir);
        }
    }
    catch(const std::exception& e){
        std::cerr << "Failed to initialize logs directory: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int Logger::AddLogger(const std::string& logger_name, const LoggerProperties& props){
    std::lock_guard<std::mutex> lock(logger_mutex);
    if(loggers.find(logger_name) != loggers.end()){
        std::cerr << "Logger already exists: " << logger_name << std::endl;
        return 1; // Logger already exists
    }

    loggers[logger_name] = props;

    if(props.file_output){
        std::ios_base::openmode mode = props.append_mode ? std::ios::app : std::ios::trunc;
        std::ofstream file(logs_dir + "/" + props.file_name, mode);
        if(!file.is_open()){
            return 2; // Failed to open file
        }
        file_streams[logger_name] = std::move(file);
    }

    return 0; // Success
}

void Logger::LogMessage(const std::string& logger_name, const std::string& msg_format, MessageType msg_type, ...){
    std::lock_guard<std::mutex> lock(logger_mutex);
    auto it = loggers.find(logger_name);
    if(it == loggers.end()){
        std::cerr << "Logger not found: " << logger_name << std::endl;
        return;
    }

    std::string formatted_message;
    va_list args;
    va_start(args, msg_type);
    size_t size = std::vsnprintf(nullptr, 0, msg_format.c_str(), args) + 1;
    va_end(args);

    if(size > 1){
        formatted_message.resize(size);
        va_start(args, msg_type);
        std::vsnprintf(&formatted_message[0], size, msg_format.c_str(), args);
        va_end(args);
    }

    Message msg;
    msg.logger_name = logger_name;
    msg.type = msg_type;
    msg.body = formatted_message;
    msg.timestamp_cpp = std::chrono::system_clock::now();

    messages.insert(std::upper_bound(messages.begin(), messages.end(), msg), msg);
}

void Logger::FlushMessages(){
    std::lock_guard<std::mutex> lock(logger_mutex);

    for(const auto& msg : messages){
        auto it = loggers.find(msg.logger_name);
        if(it == loggers.end()){
            continue;
        }

        const auto& props = it->second;
        std::ostringstream log_entry;

        char time_str[20];
        std::tm local_time;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(msg.timestamp_cpp.time_since_epoch()).count();
        std::time_t now_c = std::chrono::system_clock::to_time_t(msg.timestamp_cpp);
        if(localtime_s(&local_time, &now_c) == 0){
            std::strftime(time_str, sizeof(time_str), "%b/%d %H:%M:%S.", &local_time);
        }
        else{
            snprintf(time_str, sizeof(time_str), "%s", "Invalid Time");
        }

        log_entry << "[" << time_str << (ms % 1000) << "] [" << MessageTypeToString(msg.type) << "] " << msg.body;

        if(props.console_output){
            std::string color = color_table[static_cast<int>(props.message_colors.at(msg.type))];
            std::cout << color << log_entry.str() << "\x1b[0m" << std::endl;
        }

        if(props.file_output){
            auto stream_it = file_streams.find(msg.logger_name);
            if(stream_it != file_streams.end()){
                stream_it->second << log_entry.str() << std::endl;
            }
            else{
                std::cerr << "Failed to write to log file for logger: " << msg.logger_name << std::endl;
            }
        }
    }

    messages.clear();
}

void Logger::Stop(){
    std::lock_guard<std::mutex> lock(logger_mutex);
    for(auto& [logger_name, stream] : file_streams){
        if(stream.is_open()){
            stream.close();
        }
    }
    file_streams.clear();
}

std::string Logger::MessageTypeToString(MessageType type){
    switch(type){
    case MessageType::DEBUG: return "DEBUG";
    case MessageType::NOTICE: return "NOTICE";
    case MessageType::INFO: return "INFO";
    case MessageType::WARNING: return "WARNING";
    case MessageType::ERROR: return "ERROR";
    case MessageType::CRIT_ERROR: return "CRIT_ERROR";
    default: return "UNKNOWN";
    }
}
