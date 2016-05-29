#pragma once

#include <stdint.h>
#include <vector>

template<size_t StackSize = 128>
    class VariableStackArray {
    static_assert(sizeof(uint8_t) == 1, "A uint8_t must be exactly one byte!");

    uint8_t _stackData[StackSize];
    std::vector<uint8_t> _variableData;

    bool _stackAllocated;
    size_t _size;
    public:
    VariableStackArray() : _stackAllocated(true), _size(0) {
        memset(_stackData, 0, sizeof(_stackData));
    }

    void updateData(const void* data, size_t size) {
        if (size <= StackSize) {
            // Copy to stack array
            _stackAllocated = true;
            memcpy(_stackData, data, size);
        } else {
            _variableData.resize(size);
            _stackAllocated = false;

            auto begin = reinterpret_cast<const uint8_t*>(data);
            _variableData.assign(begin, begin + size);
        }
        _size = size;
    }

    void* getData() {
        if (_size == 0) {
            return nullptr;
        }

        if (_stackAllocated) {
            return _stackData;
        }
        return _variableData.data();
    }

    size_t getSize() const {
        return _size;
    }
};

