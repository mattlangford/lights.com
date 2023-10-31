#pragma once

template <typename T>
T* move(T* ptr, size_t count) {
    return reinterpret_cast<T*>(realloc(ptr, sizeof(T) * count));
}

