#pragma once

#include <string>

class Interface {
public:
    void handle_serial() {
        const auto str = Serial.readString().trim();
        if (str.length() == 0) return;
        for (const auto& handler : handlers_) {
            if (str.startsWith(handler.first)) {
                // Assume there is a space at the end of the command
                const auto data = str.substring(handler.first.length() + 1);
                handler.second.function(std::string(data.c_str()));
                return;
            }
        }

        Serial.println("Unknown command found!");
    }

    void help() const {
        Serial.println("Commands: ");
        for (const auto& handler : handlers_) {
            Serial.print(" - '");
            Serial.print(handler.first);
            Serial.print("': ");
            Serial.println(handler.second.description);
        }
    }

    using HandlerFunction = void(*)(const std::string&);
    void add_handler(const String& command, String description, HandlerFunction function) {
        auto& impl = handlers_[command];
        impl.function = function;
        impl.description = std::move(description);
    }

private:
    struct HandlerImpl {
        HandlerFunction function;
        String description;
    };

    std::map<String, HandlerImpl> handlers_;
};
