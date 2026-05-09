#ifndef AREG_BASE_SEMAPHORE_HPP
#define AREG_BASE_SEMAPHORE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/lockable/Semaphore.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              Semaphore           - Semaphore synchronization object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/lockable/lockable.h"

#include <atomic>

namespace areg {
//////////////////////////////////////////////////////////////////////////
// class Semaphore declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Synchronization semaphore with a configurable count. Maintains a count between zero and
 *          a maximum. Used to control access to a limited resource by multiple threads.
 **/
class AREG_API Semaphore final  : public Lockable
{
//////////////////////////////////////////////////////////////////////////
// Friend objects
//////////////////////////////////////////////////////////////////////////
friend class MultiLock;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a semaphore with the specified maximum count and initial count.
     *
     * \param   maxCount        The maximum number of threads allowed to acquire the semaphore
     *                          simultaneously.
     * \param   initCount       The initial count. Must be between 0 and maxCount. A count of 0
     *                          means the semaphore is initially locked. Default is 0.
     **/
    explicit Semaphore( int32_t maxCount, int32_t initCount = 0 );

    virtual ~Semaphore();

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Acquires the semaphore by decrementing its count. Blocks if count is zero until it
     *          is incremented or timeout expires.
     *
     * \param   timeout     The timeout in milliseconds. Use areg::WAIT_INFINITE to wait
     *                      indefinitely.
     * \return  Returns true if successfully acquired; false if timeout expired.
     **/
    inline bool lock( uint32_t timeout = areg::WAIT_INFINITE ) final;

    /**
     * \brief   Releases the semaphore by incrementing its count.
     *
     * \return  Returns true if successfully incremented.
     **/
    inline bool unlock() final;

    /**
     * \brief   Not implemented for semaphore. Always returns false.
     *
     * \return  Returns false.
     **/
    inline bool try_lock() final;

//////////////////////////////////////////////////////////////////////////
//// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the maximum count of the semaphore.
     **/
    [[nodiscard]]
    inline uint32_t max_count() const noexcept;

    /**
     * \brief   Returns the current count of the semaphore.
     **/
    [[nodiscard]]
    inline uint32_t current_count() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden or OS specific methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS-specific implementation to create the semaphore.
     **/
    void _os_create_semaphore();

    /**
     * \brief   OS-specific implementation to release semaphore resources.
     **/
    void _os_release_semaphore();

    /**
     * \brief   OS-specific implementation to acquire the semaphore. Blocks if already locked, or
     *          returns false on timeout.
     *
     * \param   timeout     The timeout in milliseconds to wait for semaphore availability.
     * \return  Returns true if successfully acquired.
     **/
    bool _os_lock( uint32_t timeout );

    /**
     * \brief   OS-specific implementation to release the semaphore. Allows waiting threads to
     *          acquire it.
     *
     * \return  Returns true if successfully released.
     **/
    bool _os_unlock();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   
     **/
    const uint32_t      mMaxCount;  //!< Maximum lock count number of semaphore

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    std::atomic_uint    mCurrCount; //!< Current lock count number of semaphore
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    Semaphore() = delete;
    AREG_NOCOPY_NOMOVE( Semaphore );
};

//////////////////////////////////////////////////////////////////////////
// Semaphore class inline functions
//////////////////////////////////////////////////////////////////////////
inline uint32_t Semaphore::max_count() const noexcept
{
    return mMaxCount;
}

inline uint32_t Semaphore::current_count() const noexcept
{
    return mCurrCount.load();
}
}

#endif // AREG_BASE_SEMAPHORE_HPP
