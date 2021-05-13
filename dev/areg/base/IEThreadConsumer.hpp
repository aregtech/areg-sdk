#ifndef AREG_BASE_IETHREADCONSUMER_HPP
#define AREG_BASE_IETHREADCONSUMER_HPP
/************************************************************************
 * \file        areg/base/IEThreadConsumer.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread Consumer interface class.
 *              Implement methods to be able to be triggered by
 *              thread class on certain events.
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
 * \brief       The Thread Consumer class required by Thread object
 *              to trigger functions on certain events. The reference
 *              to object should be passed to Thread object. Every
 *              thread object has one thread consumer.
 * 
 * \details     There is no need to re-implement running and exit functions
 *              in thread class. Instead, the Consumer class can be created
 *              and the thread object will trigger functions by reaching
 *              certain stage. For example, when Thread object is created
 *              it will trigger Created() method of consumer and when
 *              it is resumed and running, it will trigger Run() function
 *              of consumer. Developers need to implement pure virtual
 *              methods to react on certain events of thread.
 *              As soon as Consumer exits Run() function, the thread
 *              will exit. If thread needs to run in loop, implement
 *              loop mechanism in Run() method as well as implement
 *              mechanism to exit from loop. The trigger to notify
 *              system requirement to exit thread is a call of
 *              Destroying() method.
 *
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
    typedef enum E_ExitCodes
    {
          EXIT_NO_PARAM     = -2    //!< Thread failed running, it had no parameter
        , EXIT_TERMINATED   = -1    //!< Thread is abnormally terminated
        , EXIT_NORMAL       = 0     //!< Thread normally completed execution
        , EXIT_ERROR                //!< Thread exits with generic error

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
    IEThreadConsumer( void );

public:
    /**
     * \brief   Destructor
     **/
    virtual ~IEThreadConsumer( void );

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
     *          create() method should be called again.
     **/
    virtual void onThreadRuns( void ) = 0;

    /**
     * \brief	Function triggered with thread object has been created.
     *          If this function returns true, thread will continue running.
     *          If this function returns false, the thread will not run.
     * \param	threadObj	The new created Thread object, 
     *                      which contains this consumer.
     * \return	Return true if thread should run. Return false, it it should not run.
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
    IEThreadConsumer( const IEThreadConsumer & /*src*/ );
    const IEThreadConsumer & operator = ( const IEThreadConsumer & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// IEThreadConsumer inline methods
//////////////////////////////////////////////////////////////////////////

inline const char * IEThreadConsumer::getString(IEThreadConsumer::eExitCodes code)
{
    switch (code)
    {
    case IEThreadConsumer::EXIT_NO_PARAM:
        return "IEThreadConsumer::EXIT_NO_PARAM";
    case IEThreadConsumer::EXIT_TERMINATED:
        return "IEThreadConsumer::EXIT_TERMINATED";
    case IEThreadConsumer::EXIT_NORMAL:
        return "IEThreadConsumer::EXIT_NORMAL";
    case IEThreadConsumer::EXIT_ERROR:
        return "IEThreadConsumer::EXIT_ERROR";
    default:
        return "ERR: Unexpected value of type IEThreadConsumer::eExitCodes";
    }
}

#endif  // AREG_BASE_IETHREADCONSUMER_HPP
