#pragma once

template <typename T>
T* move(T* ptr, size_t count) {
    return reinterpret_cast<T*>(realloc(ptr, sizeof(T) * count));
}

template <typename T>
class Optional {
public:
    Optional() {}
    Optional(T t) : set_(true), data_(t) {}

    void operator=(const T& rhs) {
        set_ = true;
        data_ = rhs;
    }

    void reset() { set_ = false;}
    T& value() { return data_; }
    bool has_value() { return set_; }

private:
    bool set_ = false;
    T data_;
};
