#pragma once

#include <stddef.h>
#include <functional>

template<typename Enum>
struct EnumClassHash {
    size_t operator()(Enum e) const {
        return std::hash<typename std::underlying_type<Enum>::type>::operator()(static_cast<typename std::underlying_type<
            Enum>::type>(e));
    }
};

struct PairHash {
 public:
    template<typename T, typename U>
    std::size_t operator()(const std::pair<T, U>& x) const {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};
