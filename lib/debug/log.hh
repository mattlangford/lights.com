#ifdef ARDUINO
    #include <Arduino.h>

    #define LOG_INFO(fmt, ...)  Serial.printf("[INFO] " fmt "\n", ##__VA_ARGS__)
    #define LOG_WARN(fmt, ...)  Serial.printf("[WARN] " fmt "\n", ##__VA_ARGS__)
    #define LOG_ERROR(fmt, ...) Serial.printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#else
    #include <stdio.h>
    #define LOG_INFO(fmt, ...)  printf("[INFO] " fmt "\n", ##__VA_ARGS__)
    #define LOG_WARN(fmt, ...)  printf("[WARN] " fmt "\n", ##__VA_ARGS__)
    #define LOG_ERROR(fmt, ...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#endif

#ifdef ARDUINO
    #include <Arduino.h>
    #define CHECK(cond, fmt, ...) do { \
        if (!(cond)) { \
            Serial.printf("[CHECK FAILED] " fmt "\n", ##__VA_ARGS__); \
            abort(); \
        } \
    } while (0)
#else
    #include <string>
    #include <format>
    #include <sstream>
    #define CHECK(cond, fmt, ...) do { \
        if (!(cond)) { \
            std::stringstream ss; \
            ss << #cond << " check failed with: " << std::format(fmt, ##__VA_ARGS__); \
            throw std::runtime_error(ss.str()); \
        } \
    } while (0)
#endif