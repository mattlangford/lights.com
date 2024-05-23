#pragma once

#include <variant>
#include <optional>

#include <ArduinoJson.h>

struct SetConfigResult {
public:
    enum class ErrorType {
        NONE,
        NO_VALUES,
        GENERIC,
    };

public:
    static SetConfigResult okay() {
        return SetConfigResult { .type=ErrorType::NONE };
    }
    static SetConfigResult no_values_set() {
        return SetConfigResult {.type=ErrorType::NO_VALUES, .message="No config values set"};
    }
    static SetConfigResult error(String message) {
        return SetConfigResult { .type=ErrorType::GENERIC, .message=message };
    }

    bool is_okay() const { return type == ErrorType::NONE; }

    template <typename T>
    bool maybe_set(const JsonObject& obj, const char* name, T& out) {
        auto field = obj[name];
        if (field.isNull()) {
            return false;
        }

        // If we haven't seen any values yet but the field is there, we're good now!
        if (type == ErrorType::NO_VALUES) {
            *this = okay();
        }

        out = field.as<T>();
        return true;
    }

    template <typename T>
    T value_or(const JsonObject& obj, const char* name, const T& fallback) {
        auto field = obj[name];
        if (field.isNull()) {
            return fallback;
        }

        // If we haven't seen any values yet but the field is there, we're good now!
        if (type == ErrorType::NO_VALUES) {
            *this = okay();
        }
        return field.as<T>();
    }

    SetConfigResult& consider(const SetConfigResult& rhs, const String& name="") {
        if (is_okay() && type == SetConfigResult::ErrorType::NO_VALUES) {
            return *this;
        }

        if (rhs.is_okay()) {
            // If there were no values, but the latest is okay we're okay too!
            if (type == SetConfigResult::ErrorType::NO_VALUES) {
                *this = rhs;
            }
            return *this;
        }

        if (type != rhs.type) {
            // If the errors are different, just give a generic error back.
            type = ErrorType::GENERIC;
        }

        if (type == ErrorType::GENERIC) {
            if (!message.length() == 0) {
                message += " " + name + ":";
            }
            message += " '" + rhs.message + "'";
        }

        return *this;
    }

    // Default to a good state
    ErrorType type = ErrorType::NONE;
    String message;
};
