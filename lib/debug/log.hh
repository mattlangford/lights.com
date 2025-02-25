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
            Serial.printf(#cond " failed '" fmt "'\n", ##__VA_ARGS__); \
            abort(); \
        } \
    } while (0)
#else
    #define CHECK(cond, fmt, ...) do { \
        if (!(cond)) { \
            LOG_ERROR(#cond " failed '" #fmt "'", ##__VA_ARGS__); \
            throw std::runtime_error("Check failed!"); \
        } \
    } while (0)
#endif