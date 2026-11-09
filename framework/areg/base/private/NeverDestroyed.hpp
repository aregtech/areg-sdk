#ifndef AREG_BASE_PRIVATE_NEVERDESTROYED_HPP
#define AREG_BASE_PRIVATE_NEVERDESTROYED_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/NeverDestroyed.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, object in static storage whose destructor never runs.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include <new>

namespace areg {

/**
 * \brief   Holds an object in its own storage and never runs the object's destructor.
 *          Declared as a function-local static it is built on the first call, stays
 *          usable until the process ends and registers nothing to run at exit.
 * \tparam  T   The type of the object.
 **/
template <typename T>
class NeverDestroyed
{
public:
    /**
     * \brief   Builds the object with its default constructor.
     **/
    NeverDestroyed()
    {
        ::new (_storage()) T();
    }

    /**
     * \brief   Builds the object with a callable, for a type whose constructor is not public.
     * \param   build   Called once with the address of the storage to build the object in.
     **/
    template <typename Build>
    explicit NeverDestroyed(Build build)
    {
        build(_storage());
    }

    /**
     * \brief   Returns the object.
     **/
    [[nodiscard]]
    inline T & object() noexcept
    {
        return *std::launder(static_cast<T *>(_storage()));
    }

private:
    inline void * _storage() noexcept
    {
        return static_cast<void *>(mStorage);
    }

    alignas(T) unsigned char mStorage[sizeof(T)];   //!< The storage of the object.

    AREG_NOCOPY_NOMOVE(NeverDestroyed);
};

} // namespace areg

#endif  // AREG_BASE_PRIVATE_NEVERDESTROYED_HPP
