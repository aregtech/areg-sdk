#ifndef AREG_BASE_SHAREDPRIMITIVE_HPP
#define AREG_BASE_SHAREDPRIMITIVE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SharedPrimitive.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Reference-counted wrapper for a primitive value.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include <atomic>
#include <cstdint>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SharedPrimitive class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Reference-counted wrapper for a primitive value.
 *
 *          Structured like std::shared_ptr, but the control block stores the
 *          managed value directly instead of a pointer to it. The object
 *          itself is a single pointer (8 bytes on 64-bit). One heap allocation
 *          is made per resource: a control block holding the atomic reference
 *          counter and the value together.
 *
 *          When the last copy is destroyed or reset(), the Closer function is
 *          called with the stored value to release the underlying resource.
 *
 * \tparam  Primitive               The primitive type to store (int, size_t, HANDLE, etc.).
 * \tparam  InvalidValue    Sentinel value returned by value() when the instance is
 *                          empty. Must be a compile-time constant (constexpr).
 * \tparam  Closer          Optional function called with the value when the reference
 *                          count reaches zero. Defaults to nullptr (no-op).
 *
 * \example
 * \code
 *   static void close_fd(int fd) noexcept { ::close(fd); }
 *
 *   using SharedFd = SharedPrimitive<int, -1, close_fd>;
 *
 *   SharedFd a(::open("file.txt", O_RDONLY));  // ref count = 1
 *   SharedFd b = a;                            // ref count = 2
 *   a.reset();                                 // ref count = 1, fd still open
 *   // b destroyed: ref count -> 0, close_fd(fd) called
 * \endcode
 **/
template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept = nullptr>
class SharedPrimitive
{
//////////////////////////////////////////////////////////////////////////
// Internal control block
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Control block co-locating the reference counter and the managed
     *          value in a single heap allocation. Mirrors std::shared_ptr's
     *          internal control block, but holds Primitive by value instead of Primitive*.
     **/
    struct ControlBlock
    {
        std::atomic<int32_t>    mRefCount;  //!< Shared reference counter.
        Primitive               mValue;     //!< The managed primitive value.

        inline explicit ControlBlock(Primitive value) noexcept;
    };

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates an empty instance.
     **/
    inline SharedPrimitive() noexcept;

    /**
     * \brief   Constructs with a live value. Allocates one control block
     *          containing the reference counter and the value.
     *
     * \param   value   The primitive value to manage.
     **/
    inline explicit SharedPrimitive(Primitive value);

    /**
     * \brief   Copy constructor. Shares ownership with \a other; increments
     *          the reference count. O(1), copies only the control-block pointer.
     **/
    inline SharedPrimitive(const SharedPrimitive& other) noexcept;

    /**
     * \brief   Move constructor. Transfers ownership from \a other without
     *          changing the reference count. \a other becomes empty.
     **/
    inline SharedPrimitive(SharedPrimitive&& other) noexcept;

    inline ~SharedPrimitive() noexcept;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    inline SharedPrimitive& operator = (const SharedPrimitive& other) noexcept;

    inline SharedPrimitive& operator = (SharedPrimitive&& other) noexcept;

    [[nodiscard]]
    inline explicit operator Primitive () noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the stored primitive value, or InvalidValue when empty.
     **/
    [[nodiscard]]
    inline Primitive value() const noexcept;

    /**
     * \brief   Returns true if this instance holds a live resource (ref count > 0).
     **/
    [[nodiscard]]
    inline bool has_value() const noexcept;

    /**
     * \brief   Returns the number of SharedPrimitive instances sharing this
     *          resource, or 0 if the instance is empty.
     **/
    [[nodiscard]]
    inline int32_t use_count() const noexcept;

    /**
     * \brief   Releases this instance's share of the resource. If this is the
     *          last copy, Closer is invoked and the control block is freed.
     *          The instance becomes empty after the call.
     **/
    inline void reset() noexcept;

//////////////////////////////////////////////////////////////////////////
// Private helpers
//////////////////////////////////////////////////////////////////////////
private:
    inline void _add_ref() noexcept;

    inline void _release() noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ControlBlock *  mCtrl;  //!< Pointer to the shared control block; nullptr when empty.
};

//////////////////////////////////////////////////////////////////////////
// SharedPrimitive class template implementation
//////////////////////////////////////////////////////////////////////////
template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline SharedPrimitive<Primitive, InvalidValue, Closer>::ControlBlock::ControlBlock(Primitive value) noexcept
    : mRefCount{ 1 }
    , mValue{ value }
{
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline SharedPrimitive<Primitive, InvalidValue, Closer>::SharedPrimitive() noexcept
    : mCtrl{ nullptr }
{
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline SharedPrimitive<Primitive, InvalidValue, Closer>::SharedPrimitive(Primitive value)
    : mCtrl{ new ControlBlock(value) }
{
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline SharedPrimitive<Primitive, InvalidValue, Closer>::SharedPrimitive(const SharedPrimitive& other) noexcept
    : mCtrl{ other.mCtrl }
{
    _add_ref();
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline SharedPrimitive<Primitive, InvalidValue, Closer>::SharedPrimitive(SharedPrimitive && other) noexcept
    : mCtrl{ other.mCtrl }
{
    other.mCtrl = nullptr;
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline SharedPrimitive<Primitive, InvalidValue, Closer>::~SharedPrimitive() noexcept
{
    _release();
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline SharedPrimitive<Primitive, InvalidValue, Closer>& SharedPrimitive<Primitive, InvalidValue, Closer>::operator = (const SharedPrimitive& other) noexcept
{
    if (this != &other)
    {
        _release();
        mCtrl = other.mCtrl;
        _add_ref();
    }

    return (*this);
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline SharedPrimitive<Primitive, InvalidValue, Closer>& SharedPrimitive<Primitive, InvalidValue, Closer>::operator = (SharedPrimitive && other) noexcept
{
    if (this != &other)
    {
        _release();
        mCtrl = other.mCtrl;
        other.mCtrl = nullptr;
    }

    return (*this);
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline SharedPrimitive<Primitive, InvalidValue, Closer>::operator Primitive() noexcept
{
    return (mCtrl != nullptr ? mCtrl->mValue : InvalidValue);
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline Primitive SharedPrimitive<Primitive, InvalidValue, Closer>::value() const noexcept
{
    return (mCtrl != nullptr ? mCtrl->mValue : InvalidValue);
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline bool SharedPrimitive<Primitive, InvalidValue, Closer>::has_value() const noexcept
{
    return (mCtrl != nullptr);
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline int32_t SharedPrimitive<Primitive, InvalidValue, Closer>::use_count() const noexcept
{
    return (mCtrl != nullptr ? mCtrl->mRefCount.load(std::memory_order_relaxed) : 0);
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline void SharedPrimitive<Primitive, InvalidValue, Closer>::reset() noexcept
{
    _release();
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline void SharedPrimitive<Primitive, InvalidValue, Closer>::_add_ref() noexcept
{
    if (mCtrl != nullptr)
    {
        mCtrl->mRefCount.fetch_add(1, std::memory_order_relaxed);
    }
}

template<typename Primitive, Primitive InvalidValue, void(*Closer)(Primitive) noexcept /*= nullptr*/>
inline void SharedPrimitive<Primitive, InvalidValue, Closer>::_release() noexcept
{
    if (mCtrl == nullptr)
        return;
    
    if (mCtrl->mRefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
    {
        if constexpr (Closer != nullptr)
            Closer(mCtrl->mValue);

        delete mCtrl;
    }

    mCtrl = nullptr;
}

} // namespace areg

#endif  // AREG_BASE_SHAREDPRIMITIVE_HPP
