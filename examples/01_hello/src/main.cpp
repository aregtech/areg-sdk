//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project demonstrates creation of a simple thread to 
//               output "Hello World!" message on console.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"

#include <iostream>

#ifdef  _WIN32
    // link with areg library, valid only for MSVC
    #pragma comment(lib, "areg.lib")
#endif // _WIN32

//! \brief   A thread to run and output message.
class HelloThread   : public    Thread
                    , protected IEThreadConsumer
{
public:
    HelloThread( void )
        : Thread( self( ), "HelloThread" ) // set consumer and the name
        , IEThreadConsumer( )
    {
    }

    virtual ~HelloThread( void ) = default;

protected:
/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    //! \brief  This callback is triggered when thread runs and fully operable.
    virtual void onThreadRuns( void ) override
    {
        std::cout << "The thread " << getName( ) << " runs, outputing message." << std::endl;
        std::cout << "!!! Hello World !!!" << std::endl;
    }

private:
    //!< The wrapper of 'this' pointer to call in constructor
    inline HelloThread & self( void )
    {
        return (*this);
    };
};

//////////////////////////////////////////////////////////////////////////
// Demo to create and destroy thread.
//////////////////////////////////////////////////////////////////////////
int main()
{
    std::cout << "Demo to create and destroy thread ..." << std::endl;

    // declare thread object.
    HelloThread aThread;
    // create and start thread, wait until it is started.
    aThread.createThread(NECommon::WAIT_INFINITE);
    // stop and destroy thread, clean resources. Wait until thread ends.
    aThread.destroyThread(NECommon::WAIT_INFINITE);

    std::cout << "Exit application!" << std::endl;
    return 0;
}
