#include <iostream>
#include <thread>

#include "logger.h"

int main()
{
    using namespace SFGE;

    Logger log(10);

    log.add_entry(Logger::TYPE_DEBUG, Logger::SENDER_OS, "lol debug");
    log.add_entry(Logger::TYPE_NOTIFICATION, Logger::SENDER_OS, "kek");
    log.add_entry(Logger::TYPE_WARNING, Logger::SENDER_APP, "bruh");
    log.add_entry(Logger::TYPE_DEBUG, Logger::SENDER_OPENGL, "debug");
    log.add_entry(Logger::TYPE_NOTIFICATION, Logger::SENDER_OTHER, "idk");
    log.add_entry(Logger::TYPE_WARNING, Logger::SENDER_APP, "sus");
    log.add_entry(Logger::TYPE_DEBUG, Logger::SENDER_UNKNOWN, "??? debug?");
    log.add_entry(Logger::TYPE_NOTIFICATION, Logger::SENDER_APP, "kek");
    log.add_entry(Logger::TYPE_WARNING, Logger::SENDER_UNKNOWN, "sus");

    std::vector<Logger::Log_entry> list;

    auto sorting = [](Logger::Log_entry a, Logger::Log_entry b) -> bool { return a._sender > b._sender; };
    log.get_sorted_queue(list, Logger::OPERATION_MORE | Logger::SORT_BY_TYPE, Logger::ALL_TRUE ^ Logger::TYPE_DEBUG, Logger::ALL_TRUE ^ Logger::SENDER_OTHER);

    for (Logger::Log_entry msg : list) {
        std::cout << "[" << msg._time << "]: \"" << msg._msg << "\" from " << msg._sender << std::endl;
    }

    std::cin.get();
    return 0;
}