#ifndef AREG_BASE_THREADCONSUMER_HPP
#define AREG_BASE_THREADCONSUMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/ThreadConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread Consumer interface class.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class Thread;

//////////////////////////////////////////////////////////////////////////
// ThreadConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface for thread lifecycle callbacks. Implement this class to respond to thread
 *          creation, running, and destruction events.
 **/
class AREG_API ThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal defines and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   ThreadConsumer::ExitCode
     *          Defines thread exit codes.
     **/
    enum class ExitCode : int32_t
    {
          NoParam       = -2    //!< Thread failed running, it had no parameter
        , Terminated    = -1    //!< Thread is abnormally terminated
        , Normal        =  0    //!< Thread normally completed execution
        , Error         =  1    //!< Thread exits with generic error
    };

    //!< Return the string representation of the thread exit code.
    [[nodiscard]]
    static inline constexpr const char * as_string( ThreadConsumer::ExitCode code) noexcept;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    ThreadConsumer() = default;

    virtual ~ThreadConsumer() = default;

//////////////////////////////////////////////////////////////////////////
// Callbacks
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// ThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Called when the thread starts running and is fully operable. Implement the thread's
     *          main loop here. When this returns, the thread completes. Call start() again to restart.
     **/
    virtual void on_run() = 0;

    /**
     * \brief   Called when the thread object is created. Return true to start the thread; false to
     *          prevent running.
     *
     * \param   threadObj       The newly created Thread object.
     * \return  Return true if the thread should run; false otherwise.
     **/
    virtual bool on_thread_registered( Thread * threadObj );
    /**
     * \brief   Called when the thread is about to be destroyed. Implement exit logic here.
     **/
    virtual void on_thread_unregistering();

    /**
     * \brief   Called after the thread exits the run loop. Return the thread exit code.
     *
     * \return  Return the thread exit error code.
     **/
    virtual int32_t on_exit();

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( ThreadConsumer );
};

//////////////////////////////////////////////////////////////////////////
// ThreadConsumer inline methods
//////////////////////////////////////////////////////////////////////////

inline constexpr const char * ThreadConsumer::as_string(ThreadConsumer::ExitCode code) noexcept
{
    switch (code)
    {
    case ThreadConsumer::ExitCode::NoParam:
        return "ThreadConsumer::NoParam";
    case ThreadConsumer::ExitCode::Terminated:
        return "ThreadConsumer::Terminated";
    case ThreadConsumer::ExitCode::Normal:
        return "ThreadConsumer::Normal";
    case ThreadConsumer::ExitCode::Error:
        return "ThreadConsumer::Error";
    default:
        return "ERR: Unexpected value of type ThreadConsumer::ExitCode";
    }
}

} // namespace areg
#endif  // AREG_BASE_THREADCONSUMER_HPP
