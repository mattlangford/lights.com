#ifdef ARDUINO
    #include <Arduino.h>

    #define LINFO(fmt, ...)  Serial.printf("[INFO] " fmt "\n", ##__VA_ARGS__)
    #define LWARN(fmt, ...)  Serial.printf("[WARN] " fmt "\n", ##__VA_ARGS__)
    #define LERROR(fmt, ...) Serial.printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#else
    #include <stdio.h>
    #define LINFO(fmt, ...)  printf("[INFO] " fmt "\n", ##__VA_ARGS__)
    #define LWARN(fmt, ...)  printf("[WARN] " fmt "\n", ##__VA_ARGS__)
    #define LERROR(fmt, ...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
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
            LERROR(#cond " failed '" #fmt "'", ##__VA_ARGS__); \
            throw std::runtime_error("Check failed!"); \
        } \
    } while (0)
#endif