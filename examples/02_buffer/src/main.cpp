//============================================================================
// Name        : 02-shared-buffer.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech © 2021
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/GEMacros.h"
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/base/SharedBuffer.hpp"
#include <iostream>

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
#endif // WINDOWS

/**
 * \brief   A shared buffer object. Not thread safe. Used to demonstrate serialization.
 */
static SharedBuffer theSharedBuffer;

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
    OUTPUT_DBG("The thread [ %s ] runs, going to output message", Thread::getCurrentThreadName());

    int numDigit = 0;
    float numPI = 0.0;
    String strMsg = "";

    theSharedBuffer.moveToBegin();
    theSharedBuffer >> numDigit;
    theSharedBuffer >> numPI;
    theSharedBuffer >> strMsg;

    std::cout << "*********************************" << std::endl;
    std::cout << "BEGIN dump buffer data .........." << std::endl;
    std::cout << "Saved number is    : " << numDigit << std::endl;
    std::cout << "Saved PI number is : " << numPI << std::endl;
    std::cout << "Saved string is    : " << strMsg.getString() << std::endl;
    std::cout << "END dump buffer data ............" << std::endl;
    std::cout << "*********************************" << std::endl;
}

//////////////////////////////////////////////////////////////////////////
// Demo
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Demo to create and destroy thread.
 */
int main()
{
    theSharedBuffer.moveToBegin();
    theSharedBuffer << static_cast<int>(1234);
    theSharedBuffer << static_cast<float>(M_PI);
    theSharedBuffer << String("!!!Hello World!!!");

    theSharedBuffer.moveToBegin();
    int   numDigit = 0;
    float numFloat = 0.0;
    String msgHello= "";

    theSharedBuffer >> numDigit;
    theSharedBuffer >> numFloat;
    theSharedBuffer >> msgHello;

    // make debug output here, check values.
    OUTPUT_DBG("The digital number is .: %d", numDigit);
    OUTPUT_DBG("The floating number is : %f", numFloat);
    OUTPUT_DBG("The hello message is ..: %s", msgHello.getString());

    // declare thread object.
    HelloThread aThread;

    // create and start thread, wait until it is started.
    OUTPUT_INFO("Starting Thread [ %s ]...", aThread.getName().getString());
    aThread.createThread(Thread::WAIT_INFINITE);

    // stop and destroy thread, clean resources. Wait until thread ends.
    OUTPUT_INFO("Destroying Thread [ %s ]...", aThread.getName().getString());
    aThread.destroyThread(Thread::WAIT_INFINITE);

    std::cout << "Exit application!" << std::endl;
    return 0;
}
