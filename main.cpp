#include "AppLogger.hpp"

#include <thread>

void second_thread_main(){
	AppLogger::LoggerProperties log_props;
	log_props.file_output = false;
	log_props.message_colors[AppLogger::MsgType::INFO] = AppLogger::Color::BRIGHT_CYAN;
	log_props.message_colors[AppLogger::MsgType::DEBUG] = AppLogger::Color::CYAN;
	
	AppLogger::AddLogger("SecondThreadLogger", log_props);

	AppLogger::LogMessage("SecondThreadLogger", "Second thread starts", AppLogger::MsgType::INFO);

	for(int i = 1; i <= 10; i++){
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		AppLogger::LogMessage("SecondThreadLogger", "%i miliseconds passed", AppLogger::MsgType::DEBUG, 300*i);
		AppLogger::FlushMessages();
	}
}

int main(){
	AppLogger::Init("./logs");

	AppLogger::LoggerProperties app_log_props;
	app_log_props.console_output = true;
	app_log_props.file_output = true;
	app_log_props.file_name = "app.log";
	app_log_props.append_mode = true;

	if(AppLogger::AddLogger("AppLogger", app_log_props) != 0){
		std::cerr << "Failed to add logger" << std::endl;
		return 1;
	}
	LogInfo("AppLogger", "LogInfo Macro! %i lmao", 69);
	LogCrit("This Logger Does Not Exists", "Not printed");
	AppLogger::SetLoggerLevel("AppLogger", AppLogger::LogLevel::WARNING_PLUS);

	AppLogger::LoggerProperties game_log_props;
	game_log_props.console_output = true;
	game_log_props.file_output = true;
	game_log_props.file_name = "game.log";
	game_log_props.append_mode = true;
	game_log_props.message_colors[AppLogger::MsgType::DEBUG] = AppLogger::Color::MAGENTA;
	game_log_props.message_colors[AppLogger::MsgType::NOTICE] = AppLogger::Color::WHITE;
	game_log_props.message_colors[AppLogger::MsgType::INFO] = AppLogger::Color::BRIGHT_BLACK;

	if(AppLogger::AddLogger("GameLogger", game_log_props) != 0){
		std::cerr << "Failed to add logger" << std::endl;
		return 1;
	}

	AppLogger::LogMessage("AppLogger", "This is a debug message.", AppLogger::MsgType::DEBUG);                  // not displayed since SetLoggerLevel() is WARNING_PLUS
	AppLogger::LogMessage("AppLogger", "This is a notice message.", AppLogger::MsgType::NOTICE);                // also not displayed
	AppLogger::LogMessage("AppLogger", "This is an info message.", AppLogger::MsgType::INFO);                   // also not displayed
	AppLogger::LogMessage("AppLogger", "This is an info message.", AppLogger::MsgType::WARNING);                // displayed
	AppLogger::LogMessage("AppLogger", "An error occurred: %s", AppLogger::MsgType::ERROR, "Donut not found");  // also displayed
	AppLogger::LogMessage("AppLogger", "CRITICAL ERROR, DONUT NOT FOUND!!! THIS APP IS ABANDONED BY GOD, NOOOOO", AppLogger::MsgType::CRITICAL);
	AppLogger::FlushMessages();

	std::thread second_thread(second_thread_main);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	AppLogger::LogMessage("GameLogger", "Game debug test here", AppLogger::MsgType::DEBUG);
	AppLogger::LogMessage("GameLogger", "Game notice here", AppLogger::MsgType::NOTICE);
	AppLogger::LogMessage("GameLogger", "Game info is almost invisible in console, but it exists!", AppLogger::MsgType::INFO);
	AppLogger::FlushMessages();

	AppLogger::SetLoggerLevel("AppLogger", AppLogger::LogLevel::ALL);
	LogDebug("AppLogger", "Say something :P");
	while(true){
		std::string input;
		std::getline(std::cin, input);
		std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c){ return std::tolower(c); });;
		if(input.find("quit") != std::string::npos){
			AppLogger::LogMessage("GameLogger", "Exiting input cycle.", AppLogger::MsgType::WARNING);
			AppLogger::FlushMessages();
			break;
		}
		if(input.find("play") != std::string::npos){
			AppLogger::LogMessage("GameLogger", "Sorry, no actual game here.", AppLogger::MsgType::ERROR);
			AppLogger::FlushMessages();
			continue;
		}
		if(input.find("bruh") != std::string::npos){
			AppLogger::LogMessage("GameLogger", "Yeah, life is hard man...", AppLogger::MsgType::DEBUG);
			AppLogger::FlushMessages();
			continue;
		}
		AppLogger::LogMessage("GameLogger", "Continuing input cycle.", AppLogger::MsgType::NOTICE);
		AppLogger::FlushMessages();
	}
	second_thread.join();

	AppLogger::LogMessage("GameLogger", "End of program!", AppLogger::MsgType::WARNING);

	AppLogger::Stop();

	return 0;
}
