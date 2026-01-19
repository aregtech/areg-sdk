//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : This project demonstrates creation of a simple thread to 
//               output "Hello Thread!" message on console.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"

#include <iostream>

#ifdef  _MSC_VER
    #pragma comment(lib, "areg")    // valid only for MSVC
#endif // _MSC_VER

//! \brief   A thread to run and output message.
class HelloThread : public Thread, protected IEThreadConsumer
{
public:
    HelloThread( void )
        : Thread( static_cast<IEThreadConsumer &>(*this), "HelloThread") // set consumer and the name
        , IEThreadConsumer( )
    {  }
protected:
/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    //! \brief  This callback is triggered when thread runs and fully operable.
    virtual void onThreadRuns( void ) override
    {
        std::cout << "!!! Hello Thread \'" << getName( ) << "\' !!!" << std::endl;
    }
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
    aThread.shutdownThread(NECommon::WAIT_INFINITE);

    std::cout << "Exit application!" << std::endl;
    return 0;
}
