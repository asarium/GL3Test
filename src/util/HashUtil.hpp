#pragma once

#include <stddef.h>
#include <functional>

namespace std {
    template<typename T, typename U>
    struct hash<std::pair<T, U>> {
     public:
        std::size_t operator()(const std::pair<T, U>& x) const {
            return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
        }
    };
}

template<typename Enum>
struct EnumClassHash {
private:
    typedef typename std::underlying_type<Enum>::type underlying_type;

public:
    size_t operator()(Enum e) const {
        return std::hash<underlying_type>()(static_cast<underlying_type>(e));
    }
};

#define HASHABLE_ENUMCLASS(TYPE) namespace std{template<> struct hash<TYPE> : EnumClassHash<TYPE> {};}

