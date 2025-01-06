#include "Logger.hpp"

#include <thread>

// Example usage
int main(){
    Logger::Init("./logs");

    Logger::LoggerProperties app_log_props;
    app_log_props.file_name = "app.log";
    app_log_props.append_mode = false;
    app_log_props.console_output = true;
    app_log_props.file_output = true;

    if(Logger::AddLogger("AppLogger", app_log_props) != 0){
        std::cerr << "Failed to add logger" << std::endl;
        return 1;
    }

    Logger::LoggerProperties game_log_props;
    game_log_props.file_name = "game.log";
    game_log_props.append_mode = true;
    game_log_props.console_output = true;
    game_log_props.file_output = true;
    game_log_props.message_colors[Logger::MessageType::DEBUG] = Logger::Color::BRIGHT_BLUE;

    if(Logger::AddLogger("GameLogger", game_log_props) != 0){
        std::cerr << "Failed to add logger" << std::endl;
        return 1;
    }

    Logger::LogMessage("AppLogger", "This is a debug message.", Logger::MessageType::DEBUG);
    Logger::LogMessage("AppLogger", "This is a notice message.", Logger::MessageType::NOTICE);
    Logger::LogMessage("AppLogger", "This is an info message.", Logger::MessageType::INFO);
    Logger::LogMessage("AppLogger", "This is an info message.", Logger::MessageType::WARNING);
    Logger::LogMessage("AppLogger", "An error occurred: %s", Logger::MessageType::ERROR, "file not found");
    std::chrono::milliseconds mills(10);
    std::this_thread::sleep_for(mills);
    Logger::LogMessage("GameLogger", "Game debug test here", Logger::MessageType::DEBUG);

    Logger::FlushMessages();
    Logger::Stop();

    return 0;
}
