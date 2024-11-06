#include "logger.h"



SFGE::Logger::Log_entry::Log_entry(int type, int sender, std::time_t time, std::string msg) :
		_type(type), _sender(sender), _time(time), _msg(msg) {  }

void SFGE::Logger::set_log_size(size_t new_size) {
	// mayby check for max size, not sure
	if (new_size >= _log_size) {
		_log_size = new_size; //update array size
	}
	else {
		// remove oldest elements that are not in bounds
		_log_size = new_size; //update array size
	}
}
size_t SFGE::Logger::get_log_size() const { return _log_size; }

SFGE::Logger::Logger(size_t log_size) {
	_log_size = log_size;
}

void SFGE::Logger::add_entry(const int type, const int sender, const std::string msg) {
	std::time_t time;
	std::time(&time);
	while (_log_queue.size() >= _log_size) {
		_log_queue.pop_back();
	}
	_log_queue.emplace_front(type, sender, time, msg);

}

void SFGE::Logger::get_sorted_queue(std::vector<Log_entry>& sorted, std::function<bool(Log_entry, Log_entry)> comp) const {
	sorted.reserve(_log_size);
	for (Log_entry entry : _log_queue) {
		sorted.push_back(entry);
	}
	std::sort(sorted.begin(), sorted.end(), comp);
	return;
}

void SFGE::Logger::get_sorted_queue(std::vector<Log_entry>& sorting, const int bits_operation, const int bits_type, const int bits_sender ) const {
	sorting.reserve(_log_size);
	for (Log_entry entry : _log_queue) {
		if((entry._type & bits_type) && (entry._sender & bits_sender))
			sorting.push_back(entry);
	}
	std::function<bool(Log_entry, Log_entry)> compare_op;
	switch (bits_operation) {
		case OPERATION_LESS | SORT_BY_TIME:
			compare_op = [&](Log_entry a, Log_entry b) -> bool { return a._time < b._time; };
			break;
		case OPERATION_LESS | SORT_BY_TYPE:
			compare_op = [&](Log_entry a, Log_entry b) -> bool { return a._type < b._type; };
			break;
		case OPERATION_LESS | SORT_BY_SENDER:
			compare_op = [&](Log_entry a, Log_entry b) -> bool { return a._sender < b._sender; };
			break;
		case OPERATION_MORE | SORT_BY_TIME:
			compare_op = [&](Log_entry a, Log_entry b) -> bool { return a._time > b._time; };
			break;
		case OPERATION_MORE | SORT_BY_TYPE:
			compare_op = [&](Log_entry a, Log_entry b) -> bool { return a._type > b._type; };
			break;
		case OPERATION_MORE | SORT_BY_SENDER:
			compare_op = [&](Log_entry a, Log_entry b) -> bool { return a._sender > b._sender; };
			break;
	}
	std::sort(sorting.begin(), sorting.end(), compare_op);
	return;
}