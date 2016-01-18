#ifndef SIMPLE_UTIL_CLONER_HPP_INCLUDED
#define SIMPLE_UTIL_CLONER_HPP_INCLUDED

#include "cloneable.hpp"

namespace sutil
{
    template <typename T>
    struct default_clone {
        T* operator()(T const* other) const {
            return other->clone();
        }
    };
}

#endif // SIMPLE_UTIL_CLONER_HPP_INCLUDED
