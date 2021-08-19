//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech (c) 2021
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
    HelloThread( SharedBuffer & buffer );

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
     *          createThread() method should be called again.
     **/
    virtual void onThreadRuns( void );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Wrapper of this pointer
     **/
    inline HelloThread & self( void );

    SharedBuffer &  mBuffer;
};

//////////////////////////////////////////////////////////////////////////
// HelloThread implementation
//////////////////////////////////////////////////////////////////////////
const char * HelloThread::THREAD_NAME   = "HelloThread";

HelloThread::HelloThread( SharedBuffer & buffer )
    : Thread            ( self(), HelloThread::THREAD_NAME )
    , IEThreadConsumer  ( )
    , mBuffer           ( buffer )
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
    printf("The thread [ %s ] runs, going to output message:\n", Thread::getCurrentThreadName());

    int numDigit  = 0;
    float numPI   = 0.0;
    String strMsg = "";

    mBuffer.moveToBegin();
    mBuffer >> numDigit;
    mBuffer >> numPI;
    mBuffer >> strMsg;

    std::cout << "*********************************" << std::endl;
    std::cout << "BEGIN dump buffer data .........." << std::endl;
    std::cout << "Saved number is    : " << numDigit << std::endl;
    std::cout << "Saved PI number is : " << numPI << std::endl;
    std::cout << "Saved string is    : " << strMsg.getString() << std::endl;
    std::cout << "END dump buffer data ............" << std::endl;
    std::cout << "*********************************" << std::endl;

    printf( "The thread [ %s ] completed job...\n", getName().getString() );
}

//////////////////////////////////////////////////////////////////////////
// Demo
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Demo to create and destroy thread.
 */
int main()
{
    SharedBuffer buffer;

    buffer << static_cast<int>(1234);
    buffer << static_cast<float>(M_PI);
    buffer << String("!!!Hello World!!!");

    buffer.moveToBegin();
    int   numDigit = 0;
    float numFloat = 0.0;
    String msgHello= "";

    buffer >> numDigit;
    buffer >> numFloat;
    buffer >> msgHello;

    // make debug output here, check values.
    printf("The integer number is .: %d\n", numDigit);
    printf("The floating number is : %f\n", numFloat);
    printf("The string message is .: %s\n", msgHello.getString());

    // declare thread object.
    HelloThread aThread(buffer);

    // create and start thread, wait until it is started.
    aThread.createThread(Thread::WAIT_INFINITE);

    // stop and destroy thread, clean resources. Wait until thread ends.
    aThread.destroyThread(Thread::WAIT_INFINITE);

    std::cout << "Exit application!" << std::endl;
    return 0;
}
