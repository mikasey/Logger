#pragma once
#ifndef SFGE_LOGGER_H_
#define SFGE_LOGGER_H_

#include <functional>
#include <algorithm>
#include <string>
#include <vector>
#include <deque>
#include <ctime>

namespace SFGE {
	class Logger {
	public:
		struct Log_entry {
			int _type;
			int _sender;
			std::time_t _time;
			std::string _msg;

			Log_entry(int type, int sender, std::time_t time, std::string msg);
		};
		enum {
			TYPE_DEBUG = 0b00000001, TYPE_ERROR_CRIT = 0b00000010, TYPE_ERROR = 0b00000100, TYPE_WARNING = 0b00001000, TYPE_MSG = 0b00010000, TYPE_NOTIFICATION = 0b00100000,
			SENDER_OPENGL	= 0b00000001, SENDER_OPENGL_SHADER_COMP = 0b00000010, SENDER_GLFW = 0b00000100, SENDER_OS = 0b00001000, SENDER_APP = 0b00010000, SENDER_OTHER = 0b00100000, SENDER_UNKNOWN = 0b01000000,
			OPERATION_LESS = 0b00000001, OPERATION_MORE = 0b00000010, OPERATION_EQUAL = 0b00000100, SORT_BY_TYPE = 0b00001000, SORT_BY_SENDER = 0b00010000, SORT_BY_TIME = 0b00100000,
			ALL_TRUE = 0b11111111
		};

	private:
		size_t _log_size;
		std::deque<Log_entry> _log_queue;

	public:
		void set_log_size(size_t new_size);
		size_t get_log_size() const;

		Logger(size_t log_size);

		void add_entry(const int type, const int sender, const std::string msg);

		void get_sorted_queue(std::vector<Log_entry>& sorted, std::function<bool(Log_entry, Log_entry)> comp) const;
		void get_sorted_queue(std::vector<Log_entry>& sorted, const int bits_operation = OPERATION_LESS | SORT_BY_TIME, const int bits_type = ALL_TRUE, const int bits_sender = ALL_TRUE) const;
	};
}

#endif