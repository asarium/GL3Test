#pragma once

#include <stddef.h>
#include <functional>

namespace std {
    template<typename Enum>
    struct hash {
        typedef typename std::underlying_type<Enum>::type underlying_type;

        typename std::enable_if<std::is_enum<Enum>::value, size_t>::type operator()(Enum e) const {
            return std::hash<underlying_type>()(static_cast<underlying_type>(e));
        }
    };

    template<typename T, typename U>
    struct hash<std::pair<T, U>> {
     public:
        std::size_t operator()(const std::pair<T, U>& x) const {
            return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
        }
    };
}
