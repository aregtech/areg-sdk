#ifndef AREG_BASE_IETHREADCONSUMER_HPP
#define AREG_BASE_IETHREADCONSUMER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/IEThreadConsumer.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread Consumer interface class.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Thread;

//////////////////////////////////////////////////////////////////////////
// IEThreadConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Thread Consumer class is required by Thread object
 *          to trigger functions on certain events. For example,
 *          when the system thread is created and registered in the
 *          the system, the consumer gets notification onThreadRegistered(),
 *          when thread is ready for cyclic run, onRun() method
 *          of the consumer is triggered. All functionalities of the
 *          thread should be written in the consumer object.
 *          For more details see description of the methods.
 **/
class AREG_API IEThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal defines and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   IEThreadConsumer::eExitCodes
     *          Defines thread exit codes.
     **/
    typedef enum class E_ExitCodes  : int8_t
    {
          ExitNoParam       = -2    //!< Thread failed running, it had no parameter
        , ExitTerminated    = -1    //!< Thread is abnormally terminated
        , ExitNormal        =  0    //!< Thread normally completed execution
        , ExitError         =  1    //!< Thread exits with generic error

    } eExitCodes;

    /**
     * \brief   Returns string value of eExistingCode types. Used for debugging.
     **/
    static inline const char * getString( IEThreadConsumer::eExitCodes code);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected default constructor
     **/
    IEThreadConsumer( void ) = default;

    /**
     * \brief   Destructor
     **/
    virtual ~IEThreadConsumer( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Callbacks
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   This callback function is called from Thread object, when it is 
     *          running and fully operable. If thread needs run in loop, the loop 
     *          should be implemented here. When consumer exits this function, 
     *          the thread will complete work. To restart thread running, 
     *          createThread() method should be called again.
     **/
    virtual void onThreadRuns( void ) = 0;

    /**
     * \brief	Function triggered with thread object has been created.
     *          If this function returns true, thread will continue running.
     *          If this function returns false, the thread will not run.
     * \param	threadObj	The new created Thread object, 
     *                      which contains this consumer.
     * \return	Return true if thread should run. Return false, it should not run.
     **/
    virtual bool onThreadRegistered( Thread * threadObj );
    /**
     * \brief   Function is triggered from thread object when it is going to be destroyed.
     *          This method might be called by system when it is going to shut down.
     *          Implement mechanism to exit thread here.
     **/
    virtual void onThreadUnregistering( void );

    /**
     * \brief   Function is called from Thread object when it is going to exit.
     *          This method is triggered after exiting from Run() function.
     * \return  Return thread exit error code.
     **/
    virtual int onThreadExit( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEThreadConsumer );
};

//////////////////////////////////////////////////////////////////////////
// IEThreadConsumer inline methods
//////////////////////////////////////////////////////////////////////////

inline const char * IEThreadConsumer::getString(IEThreadConsumer::eExitCodes code)
{
    switch (code)
    {
    case IEThreadConsumer::eExitCodes::ExitNoParam:
        return "IEThreadConsumer::ExitNoParam";
    case IEThreadConsumer::eExitCodes::ExitTerminated:
        return "IEThreadConsumer::ExitTerminated";
    case IEThreadConsumer::eExitCodes::ExitNormal:
        return "IEThreadConsumer::ExitNormal";
    case IEThreadConsumer::eExitCodes::ExitError:
        return "IEThreadConsumer::ExitError";
    default:
        return "ERR: Unexpected value of type IEThreadConsumer::eExitCodes";
    }
}

#endif  // AREG_BASE_IETHREADCONSUMER_HPP
