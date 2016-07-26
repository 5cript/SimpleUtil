#ifndef SIMPLE_UTIL_VALUE_PTR_HPP_INCLUDED
#define SIMPLE_UTIL_VALUE_PTR_HPP_INCLUDED

#include "cloner.hpp"

#include <type_traits>
#include <memory>
#include <cassert>

namespace sutil
{
    /**
     *  A value_ptr shall fill the need for a smart pointer that clones the pointee.
     *  The pointee must provide a clone function or a cloner must be provided.
     */
    template <typename T, typename ClonerT = default_clone <T>, typename DeleterT = std::default_delete <T> >
    class value_ptr
    {
    public:
        using pointer = T*;
        using element_type = T;
        using deleter_type = DeleterT;
        using cloner_type = ClonerT;

        /**
         *  Creates an invalid value_ptr that has ownership of nothing.
         */
        constexpr value_ptr() noexcept
            : m_(nullptr, cloner_type(), deleter_type())
        {
            static_assert(!std::is_pointer<deleter_type>::value,
               "constructed with null function pointer deleter");

            static_assert(!std::is_pointer<cloner_type>::value,
               "constructed with null function pointer cloner");
        }

        /**
         *  Creates an invalid value_ptr that has ownership of nothing.
         */
        constexpr value_ptr(std::nullptr_t) noexcept
            : m_(nullptr, cloner_type(), deleter_type())
        {
            static_assert(!std::is_pointer<deleter_type>::value,
               "constructed with null function pointer deleter");

            static_assert(!std::is_pointer<cloner_type>::value,
               "constructed with null function pointer cloner");
        }

        /**
         *  Creates a new value_ptr from a raw owning pointer and aquires ownership.
         *
         *  @param ptr The pointer to take ownership of.
         */
        explicit value_ptr(T* ptr) noexcept
            : m_(ptr, cloner_type(), deleter_type())
        {
            static_assert(!std::is_pointer<deleter_type>::value,
               "constructed with null function pointer deleter");

            static_assert(!std::is_pointer<cloner_type>::value,
               "constructed with null function pointer cloner");
        }

        /**
         *  Creates a new value_ptr from a raw owning pointer and aquires ownership.
         *  Also sets a deleter function.
         *
         *  @param ptr The pointer to take ownership of.
         *  @param d A deleter.
         */
        value_ptr(T* ptr, typename std::conditional <std::is_reference <deleter_type>::value,
                                                     deleter_type, const deleter_type&>::type d) noexcept
            : m_(ptr, cloner_type(), d)
        {
            static_assert(!std::is_pointer<cloner_type>::value,
               "constructed with null function pointer cloner");
        }

        /**
         *  Creates a new value_ptr from a raw owning pointer and aquires ownership.
         *  Also sets a deleter function.
         *
         *  @param ptr The pointer to take ownership of.
         *  @param d A deleter.
         */
        value_ptr(T* ptr, typename std::remove_reference <deleter_type>::type&& d) noexcept
            : m_(std::move(ptr), cloner_type(), std::move(d))
        {
            static_assert(!std::is_pointer<cloner_type>::value,
               "constructed with null function pointer cloner");
        }

        /**
         *  Creates a new value_ptr from a raw owning pointer and aquires ownership.
         *  Also sets a cloner function.
         *
         *  @param ptr The pointer to take ownership of.
         *  @param c A cloner.
         */
        value_ptr(T* ptr, typename std::conditional <std::is_reference <cloner_type>::value,
                                                     cloner_type, const cloner_type&>::type c) noexcept
            : m_(ptr, c, deleter_type())
        {
            static_assert(!std::is_pointer<deleter_type>::value,
               "constructed with null function pointer deleter");
        }

        /**
         *  Creates a new value_ptr from a raw owning pointer and aquires ownership.
         *  Also sets a cloner function.
         *
         *  @param ptr The pointer to take ownership of.
         *  @param c A cloner.
         */
        value_ptr(T* ptr, typename std::remove_reference <cloner_type>::type&& c) noexcept
            : m_(std::move(ptr), std::move(c), deleter_type())
        {
            static_assert(!std::is_pointer<deleter_type>::value,
               "constructed with null function pointer cloner");
        }



        /**
         *  Creates a new value_ptr from a raw owning pointer and aquires ownership.
         *  Also sets a deleter function and a cloner function.
         *
         *  @param ptr The pointer to take ownership of.
         *  @param d A deleter.
         *  @param c A cloner.
         */
        value_ptr(T* ptr,
                  typename std::conditional <std::is_reference <deleter_type>::value,
                                             deleter_type, const deleter_type&>::type d,
                  typename std::conditional <std::is_reference <cloner_type>::value,
                                             cloner_type, const cloner_type&>::type c) noexcept
            : m_(ptr, c, d)
        {
        }

        /**
         *  Creates a new value_ptr from a raw owning pointer and aquires ownership.
         *  Also sets a deleter function and a cloner function.
         *
         *  @param ptr The pointer to take ownership of.
         *  @param d A deleter.
         */
        value_ptr(T* ptr,
                  typename std::remove_reference <cloner_type>::type&& d,
                  typename std::remove_reference <cloner_type>::type&& c) noexcept
            : m_(std::move(ptr), std::move(c), std::move(d))
        {
        }

        /**
         *  Moves a value ptr over. destroys previously held pointee.
         */
        value_ptr(value_ptr&& v) noexcept
        {
            reset(v.release());
            get_deleter() = std::move(v.get_deleter());
            get_cloner() = std::move(v.get_cloner());
        }

        /**
         *  Copies the pointee. Cannot be noexcept, because clone may throw.
         *  The value_ptr remains unaltered if clone throws.
         */
        value_ptr(value_ptr const& v)
            : m_(clone(v.get()), v.get_cloner(), v.get_deleter())
        {
        }

        /**
         *  Copies the pointee. Cannot be noexcept, because clone may throw.
         *  The value_ptr remains unaltered if clone throws.
         */
        template <typename U, typename ClonerU, typename DeleterU>
        value_ptr(value_ptr <U, ClonerU, DeleterU> const& v)
            : m_(clone(v.get()), v.get_cloner(), v.get_deleter())
        {
        }

        /**
         *  Move assignment.
         */
        value_ptr& operator=(value_ptr&& v)
        {
            reset(v.release());
            get_deleter() = std::move(v.get_deleter());
            get_cloner() = std::move(v.get_cloner());
            return *this;
        }

        /**
         *  Move assignment
         */
        template <typename U, typename ClonerU, typename DeleterU>
        value_ptr& operator=(value_ptr <U, ClonerU, DeleterU>&& v)
        {
            reset(v.release());
            get_deleter() = std::move(v.get_deleter());
            get_cloner() = std::move(v.get_cloner());
            return *this;
        }

        /**
         *  "Clone" assignment.
         */
        value_ptr& operator=(value_ptr const& v)
        {
            reset(clone(v.get()));
            get_deleter() = v.get_deleter();
            get_cloner() = v.get_cloner();
            return *this;
        }

        /**
         *  "Clone" assignment
         */
        template <typename U, typename ClonerU, typename DeleterU>
        value_ptr& operator=(value_ptr <U, ClonerU, DeleterU> const& v)
        {
            reset(clone(v.get()));
            get_deleter() = v.get_deleter();
            get_cloner() = v.get_cloner();
            return *this;
        }

        /**
         *  Replacing assignment, frees previously held object.
         *  Alternative to reset(ptr).
         */
        value_ptr& operator=(T* ptr)
        {
            reset(ptr);
            return *this;
        }

        /**
         *  Convenient dereferencing operator.
         */
        typename std::add_lvalue_reference <element_type>::type operator*() const
        {
            // assert(get() != 0);
            return *get();
        }

        /**
         *  Convenient arrow operator.
         */
        pointer operator->() const
        {
            return get();
        }

        /**
         *  Retrieves the held pointee.
         */
        pointer get() const
        {
            return std::get <0> (m_);
        }

        /**
         *  Retrieves a reference to the deleter.
         */
        typename std::add_lvalue_reference <deleter_type>::type
        get_deleter() noexcept
        {
            return std::get <2> (m_);
        }

        /**
         *  Retrieves a reference to the deleter.
         */
        typename std::add_lvalue_reference <typename std::add_const <deleter_type>::type>::type
        get_deleter() const noexcept
        {
            return std::get <2> (m_);
        }

        typename std::add_lvalue_reference <cloner_type>::type
        get_cloner() noexcept
        {
            return std::get <1> (m_);
        }

        /**
         *  Retrieves a reference to the cloner.
         */
        typename std::add_lvalue_reference <typename std::add_const <cloner_type>::type>::type
        get_cloner() const noexcept
        {
            return std::get <1> (m_);
        }

        /**
         *  Resets the value_ptr with a new object. calls the deleter on the old object.
         */
        void reset(pointer p = pointer())
        {
            if (p != get())
            {
                get_deleter()(get()); // delete held object.
                std::get <0> (m_) = p;
            }
        }

        /**
         *  Is set? Does it hold something?
         */
        explicit operator bool() const
        {
            return get() == nullptr ? false : true;
        }

        /**
         *  Gets the owned object and disengages ownership.
         *  (I am not responsible for deleting it anymore, do it yourself).
         */
        pointer release()
        {
            pointer p = get();
            std::get <0> (m_) = nullptr;
            return p;
        }

        /**
         *  Swaps the pointee's.
         */
        void swap(value_ptr&& v)
        {
            using std::swap;
            swap(m_, v.m_);
        }

        /**
         *  Destructor.
         */
        ~value_ptr()
        {
            reset();
        }

    private:
        template <typename PtrT>
        pointer clone(PtrT p) {
            return p ? get_cloner()(p) : nullptr;
        }

    private:
        std::tuple <T*, ClonerT, DeleterT> m_;
    };

    template <typename T, typename ClonerT = sutil::default_clone <T>, typename DeleterT = std::default_delete <T>, typename... List>
    value_ptr <T, ClonerT, DeleterT> make_value(List&&... list)
    {
        return value_ptr <T, ClonerT, DeleterT> (new T(std::forward <List> (list)...));
    }

}

#endif // SIMPLE_UTIL_VALUE_PTR_HPP_INCLUDED
