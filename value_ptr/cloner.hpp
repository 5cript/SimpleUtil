#ifndef SIMPLE_UTIL_CLONER_HPP_INCLUDED
#define SIMPLE_UTIL_CLONER_HPP_INCLUDED

#include "cloneable.hpp"

#include <type_traits>
#include <utility>

namespace sutil
{
    template <typename T>
    struct default_clone {
        constexpr default_clone() noexcept = default;

        template <typename U>//,
                  //typename Eif = typename std::enable_if <std::is_base_of <T*, U*>::type>::type>
        constexpr default_clone(default_clone <U> const&) noexcept {}

        template <typename U>
        default_clone& operator=(default_clone <U> const&) { return *this; }

        T* operator()(T* other) const {
            return other->clone();
        }
    };
}

#endif // SIMPLE_UTIL_CLONER_HPP_INCLUDED
