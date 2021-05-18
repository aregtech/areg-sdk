//============================================================================
// Name        : 01-hello.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech © 2021
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include <iostream>

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
#endif // WINDOWS

//////////////////////////////////////////////////////////////////////////
// HelloThread class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A thread to run and output message
 */
class HelloThread   : public    Thread
                    , protected IEThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The thread name;
     */
    static const char * THREAD_NAME /* = "HelloThread" */;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    HelloThread( void );

    virtual ~HelloThread( void );

protected:

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
    virtual void onThreadRuns( void );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline HelloThread & self( void );
};

//////////////////////////////////////////////////////////////////////////
// HelloThread implementation
//////////////////////////////////////////////////////////////////////////
const char * HelloThread::THREAD_NAME   = "HelloThread";

HelloThread::HelloThread( void )
    : Thread            ( self(), HelloThread::THREAD_NAME )
    , IEThreadConsumer  ( )
{
}

HelloThread::~HelloThread( void )
{
}

inline HelloThread & HelloThread::self( void )
{
    return (*this);
}

void HelloThread::onThreadRuns( void )
{
    OUTPUT_DBG("The thread [ %s ] runs, going to output message", getName().getString());

    std::cout << "!!!Hello World!!!" << std::endl; // prints !!!Hello World!!!
}

//////////////////////////////////////////////////////////////////////////
// Demo
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Demo to create and destroy thread.
 */
int main()
{
    // declare thread object.
    HelloThread aThread;
    // create and start thread, wait until it is started.
    aThread.createThread(Thread::WAIT_INFINITE);
    // stop and destroy thread, clean resources. Wait until thread ends.
    aThread.destroyThread(Thread::WAIT_INFINITE);

	return 0;
}
