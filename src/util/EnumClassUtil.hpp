#pragma once

#include <type_traits>
#include <stddef.h>


template<typename T>
struct BitOperationsTag {
    static constexpr bool value = false;
};

template<typename Enum>
inline typename std::enable_if<BitOperationsTag<Enum>::value, Enum>::type operator|(Enum lhs, Enum rhs) {
    typedef typename std::underlying_type<Enum>::type int_type;

    return static_cast<Enum>(static_cast<int_type>(lhs) | static_cast<int_type>(rhs));
}

template<typename Enum>
inline typename std::enable_if<BitOperationsTag<Enum>::value, Enum>::type &operator|=(Enum &lhs, Enum rhs) {
    typedef typename std::underlying_type<Enum>::type int_type;

    lhs = static_cast<Enum>(static_cast<int_type>(lhs) | static_cast<int_type>(rhs));

    return lhs;
}

template<typename Enum>
inline typename std::enable_if<BitOperationsTag<Enum>::value, bool>::type operator&(Enum lhs, Enum rhs) {
    typedef typename std::underlying_type<Enum>::type int_type;

    return (static_cast<int_type>(lhs) & static_cast<int_type>(rhs)) != int_type(0);
}
