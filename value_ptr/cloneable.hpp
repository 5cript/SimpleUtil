#ifndef SIMPLE_UTIL_CLONEABLE_HPP_INCLUDED
#define SIMPLE_UTIL_CLONEABLE_HPP_INCLUDED

#include <utility>

namespace sutil
{
    // use is optional
    template <typename BaseT>
    class cloneable
    {
    public:
        using clone_base = BaseT;

        virtual BaseT* clone() const = 0;
        virtual ~cloneable() = default;
    };
}

#endif // SIMPLE_UTIL_CLONEABLE_HPP_INCLUDED
