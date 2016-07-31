#pragma once

#include <memory>

template<typename T>
class PointerWrapper
{
    T* _ptr;
public:
    PointerWrapper(T* ptr) : _ptr(ptr) {}
    PointerWrapper(const std::unique_ptr<T>& ptr) : _ptr(ptr.get()) {}

    T& operator* () {
        return *_ptr;
    }
    T* operator-> () {
        return _ptr;
    }
    T* operator& () {
        return _ptr;
    }

    operator T*() {
        return _ptr;
    }
};
