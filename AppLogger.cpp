#include "AppLogger.hpp"
#include <cstdarg>
#include <algorithm>

// Color table for UNIX terminal color codes
const char* AppLogger::color_table[] = {
	"\x1b[30m", // black
	"\x1b[31m", // red
	"\x1b[32m", // green
	"\x1b[33m", // yellow
	"\x1b[34m", // blue
	"\x1b[35m", // magenta
	"\x1b[36m", // cyan
	"\x1b[37m", // gray
	"\x1b[90m", // bright_black
	"\x1b[91m", // bright_red
	"\x1b[92m", // bright_green
	"\x1b[93m", // bright_yellow
	"\x1b[94m", // bright_blue
	"\x1b[95m", // bright_magenta
	"\x1b[96m", // bright_cyan
	"\x1b[97m", // white
};
const char* AppLogger::bg_color_table[] = {
	"\x1b[40m", // black
	"\x1b[41m", // red
	"\x1b[42m", // green
	"\x1b[43m", // yellow
	"\x1b[44m", // blue
	"\x1b[45m", // magenta
	"\x1b[46m", // cyan
	"\x1b[47m", // gray
	"\x1b[100m",// bright_black
	"\x1b[101m",// bright_red
	"\x1b[102m",// bright_green
	"\x1b[103m",// bright_yellow
	"\x1b[104m",// bright_blue
	"\x1b[105m",// bright_magenta
	"\x1b[106m",// bright_cyan
	"\x1b[107m",// white
};
const char* AppLogger::es_reset = "\x1b[0m";

std::unordered_map<std::string, AppLogger::LoggerProperties> AppLogger::loggers;
std::unordered_map<std::string, std::ofstream> AppLogger::file_streams;
std::vector<AppLogger::Message> AppLogger::messages;
std::string AppLogger::logs_dir;
std::mutex AppLogger::logger_mutex;

int AppLogger::Init(const std::string& logs_directory){
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
	return MicrosoftSucksAss();
}

int AppLogger::AddLogger(const std::string& logger_name, const LoggerProperties& props){
	try{
		std::filesystem::path dir = logs_dir + "/" + props.file_name; dir.remove_filename();
		if(!std::filesystem::exists(dir)){
			std::filesystem::create_directories(dir);
		}
	}
	catch(const std::exception& e){
		std::cerr << "Failed to create directory for logger \"" << logger_name << "\": " << e.what() << std::endl;
		loggers[logger_name].file_output = false;
		return 1;
	}
	{
		std::lock_guard<std::mutex> lock(logger_mutex);
		if(loggers.find(logger_name) != loggers.end()){
			std::cerr << "Logger already exists: " << logger_name << std::endl;
			return 2; // Logger already exists
		}

		loggers[logger_name] = props;

		if(props.file_output){
			std::ios_base::openmode mode = props.append_mode ? std::ios::app : std::ios::trunc;
			std::ofstream file(logs_dir + "/" + props.file_name, mode);
			if(!file.is_open()){
				std::cerr << "Failed to open logger stream: " << logger_name << std::endl;
				loggers[logger_name].file_output = false;
				return 3; // Failed to open file
			}
			file_streams[logger_name] = std::move(file);
		}
	}
	return 0; // Success
}

void AppLogger::SetLoggerLevel(const std::string& name, LogLevel lvl){
	loggers[name].log_level = lvl;
}

void AppLogger::LogMessage(const std::string& logger_name, const std::string& msg_format, MsgType msg_type, ...){
	auto it = loggers.find(logger_name);
	if(it == loggers.end()){
		std::cerr << "Logger not found: " << logger_name << std::endl;
		return;
	}
	if(!((char)loggers[logger_name].log_level & (char)msg_type))
		return;

	std::string formatted_message;
	va_list args;
	va_start(args, msg_type);
	size_t size = static_cast<size_t>(std::vsnprintf(nullptr, 0, msg_format.c_str(), args)) + 1;
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

	std::lock_guard<std::mutex> lock(logger_mutex);
	messages.insert(std::upper_bound(messages.begin(), messages.end(), msg), msg);
}

std::string AppLogger::FormatForConsole(std::string msg, Color fg_color, Color bg_color){
	std::string result;
	return result + color_table[static_cast<size_t>(fg_color)] + bg_color_table[static_cast<size_t>(bg_color)] + msg + es_reset;
}

void AppLogger::FlushMessages(){
	std::vector<Message> local_messages;
	{
		std::lock_guard<std::mutex> lock(logger_mutex);
		local_messages.swap(messages);
	}

	for(const auto& msg : local_messages){
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

		log_entry << "<" << time_str << std::setw(3) << std::setfill('0') << (ms % 1000) << "> " << it->first << "[" << MessageTypeToString(msg.type) << "]: " << msg.body;

		if(props.console_output){
			std::cout << FormatForConsole(log_entry.str(), props.message_colors.at(msg.type), props.message_bg_colors.at(msg.type)) << std::endl;
		}

		if(props.file_output){
			auto stream_it = file_streams.find(msg.logger_name);
			if(stream_it != file_streams.end()){
				std::lock_guard<std::mutex> lock(logger_mutex);
				stream_it->second << log_entry.str() << std::endl;
			}
			else{
				std::cerr << "Failed to write to log file for logger: " << msg.logger_name << std::endl;
			}
		}
	}
}

void AppLogger::Stop(){
	AppLogger::FlushMessages();
	std::lock_guard<std::mutex> lock(logger_mutex);
	for(auto& [logger_name, stream] : file_streams){
		if(stream.is_open()){
			stream.close();
		}
	}
	file_streams.clear();
}

std::string AppLogger::MessageTypeToString(MsgType type){
	switch(type){
	case MsgType::DEBUG: return "DEBUG";
	case MsgType::NOTICE: return "NOTICE";
	case MsgType::INFO: return "INFO";
	case MsgType::WARNING: return "WARNING";
	case MsgType::ERROR: return "ERROR";
	case MsgType::CRITICAL: return "CRITICAL";
	default: return "UNKNOWN";
	}
}

#ifdef MS_WINDOWS
#include <windows.h>

int AppLogger::MicrosoftSucksAss(){
	// Set output mode to handle virtual terminal sequences
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if(hOut == INVALID_HANDLE_VALUE){
		return false;
	}
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	if(hIn == INVALID_HANDLE_VALUE){
		return false;
	}

	DWORD dwOriginalOutMode = 0;
	DWORD dwOriginalInMode = 0;
	if(!GetConsoleMode(hOut, &dwOriginalOutMode)){
		return false;
	}
	if(!GetConsoleMode(hIn, &dwOriginalInMode)){
		return false;
	}

	DWORD dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
	DWORD dwRequestedInModes = ENABLE_VIRTUAL_TERMINAL_INPUT;

	DWORD dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
	if(!SetConsoleMode(hOut, dwOutMode)){
		// we failed to set both modes, try to step down mode gracefully.
		dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
		if(!SetConsoleMode(hOut, dwOutMode)){
			// Failed to set any VT mode, can't do anything here.
			return -1;
		}
	}

	DWORD dwInMode = dwOriginalInMode | dwRequestedInModes;
	if(!SetConsoleMode(hIn, dwInMode)){
		// Failed to set VT input mode, can't do anything here.
		return -1;
	}
	return 0;
}
#else
int AppLogger::MicrosoftSucksAss(){
	return 0;
}
#endif //MS_WINDOWS
