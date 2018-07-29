#pragma once

#define LOG_DEBUG(message) \
    std::cerr << "[ DEBUG ] (" << __FILE__ << ":" << __LINE__ << "): " << message << "\n";

#define LOG_WARN(message) \
    std::cerr << "[ WARN  ] (" << __FILE__ << ":" << __LINE__ << "): " << message << "\n";

#define LOG_ERROR(message) \
    std::cerr << "[ ERROR ] (" << __FILE__ << ":" << __LINE__ << "): " << message << "\n";
