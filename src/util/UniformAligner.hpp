#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "Assertion.hpp"

template<typename TData, size_t HeaderSize = 0>
class UniformAligner {
 private:
    static_assert(sizeof(uint8_t) == 1, "A uint8_t must be exactly one byte!");

    size_t _requiredAlignment;

    std::vector<uint8_t> _buffer;
    size_t _numElements;

    static size_t alignSize(size_t size, size_t align) {
        if (align == 0) {
            return size;
        }

        auto remainder = size % align;
        if (remainder == 0) {
            return size;
        }

        return size + align - remainder;
    }
 public:
    explicit UniformAligner(size_t requiredAlignment) : _requiredAlignment(requiredAlignment), _numElements(0) {
        // Make sure that the header space is already reserved
        resize(0);
    }

    void resize(size_t num_elements) {
        size_t final_size =
            alignSize(HeaderSize, _requiredAlignment) + alignSize(sizeof(TData), _requiredAlignment) * num_elements;

        _buffer.resize(final_size);
        _numElements = num_elements;
    }

    TData* addElement() {
        _buffer.insert(_buffer.end(), alignSize(sizeof(TData), _requiredAlignment), 0);
        ++_numElements;

        return getElement(_numElements - 1);
    }

    template<typename THeader>
    THeader* getHeader() {
        static_assert(sizeof(THeader) == HeaderSize, "Header size does not match requested header type!");

        return reinterpret_cast<THeader*>(_buffer.data());
    }

    TData* getElement(size_t index) {
        size_t offset =
            alignSize(HeaderSize, _requiredAlignment) + alignSize(sizeof(TData), _requiredAlignment) * index;

        Assertion(offset < _buffer.size(), "Invalid index specified!");

        return reinterpret_cast<TData*>(_buffer.data() + offset);
    }

    size_t getOffset(size_t index) {
        size_t offset =
            alignSize(HeaderSize, _requiredAlignment) + alignSize(sizeof(TData), _requiredAlignment) * index;

        Assertion(offset < _buffer.size(), "Invalid index specified!");

        return offset;
    }

    TData* nextElement(TData* currentEl) {
        uint8_t* current = reinterpret_cast<uint8_t*>(currentEl);

        current += alignSize(sizeof(TData), _requiredAlignment);

        return reinterpret_cast<TData*>(current);
    }

    size_t getNumElements() {
        return _numElements;
    }

    size_t getSize() {
        return _buffer.size();
    }

    void* getData() {
        return _buffer.data();
    }
};
