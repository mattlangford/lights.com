#pragma once

#define LOG_DEBUG(message) \
    std::cout << "[ DEBUG ] (" << __FILE__ << ":" << __LINE__ << "): " << message << "\n";

#define LOG_WARN(message) \
    std::cout << "[ WARN  ] (" << __FILE__ << ":" << __LINE__ << "): " << message << "\n";

#define LOG_ERROR(message) \
    std::cout << "[ ERROR ] (" << __FILE__ << ":" << __LINE__ << "): " << message << "\n";
