#include <iostream>
#include <thread>

#include "logger.hpp"

int main() {
    LogInfo("Simple log here.");
    LogWarning("Wait a sec...");
    LogERROR("LMAO, %i", 69);

    std::cin.get();
    return 0;
}