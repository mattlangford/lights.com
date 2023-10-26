#pragma once

template <typename T>
T* move(T* ptr, size_t count) {
    return reinterpret_cast<T*>(realloc(ptr, sizeof(T) * count));
}

/*
template <typename T>
class Vector {
public:
    void push_back(const T& t) {
        size_t last = count_
        data_ = move<T>(data_, count)
    }

    T* at(size_t index) { return index < count_ ? &data_[index] : nullptr; }
    const T* at(size_t index) const { return index < count_ ? &data_[index] : nullptr; }

private:
    size_t count_ = 0;
    T* data_ = nullptr;
}
*/
