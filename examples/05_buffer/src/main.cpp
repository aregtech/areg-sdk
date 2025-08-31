//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project demonstrates the use of a Shared Buffer object, 
//               initialization streaming, read and write, passing in 
//               multithreading environment.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/GEMacros.h"
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"

#include <iostream>

#ifdef  _MSC_VER
    // link with areg library, valid only for MSVC
    #pragma comment(lib, "areg")
#endif // _MSC_VER

//! \brief   A thread to run and output message
class HelloThread   : public    Thread
                    , protected IEThreadConsumer
{
public:
    explicit HelloThread( SharedBuffer & buffer )
        : Thread( self( ), "HelloThread" ) // set consumer and the name
        , IEThreadConsumer( )
        , mBuffer( buffer )
    {
    }

    virtual ~HelloThread( void ) = default;

protected:

/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    //! \brief  This callback is triggered when thread runs and fully operable.
    virtual void onThreadRuns( void ) override;

private:
    //!< The wrapper of 'this' pointer to call in constructor
    inline HelloThread & self( void )
    {
        return (*this);
    }

private:
    SharedBuffer &  mBuffer;
};

//////////////////////////////////////////////////////////////////////////
// HelloThread implementation
//////////////////////////////////////////////////////////////////////////

void HelloThread::onThreadRuns( void )
{
    std::cout << "The thread [ " << getName( ) << " ] runs, outputing message." << std::endl;

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

    std::cout << "The thread [ " << getName( ) << " ] completed job ..." << std::endl;
}

//////////////////////////////////////////////////////////////////////////
// Demo to write and read data from binary buffer.
//////////////////////////////////////////////////////////////////////////
int main()
{
    std::cout << "Demo to write and read data from binary buffer ..." << std::endl;

    SharedBuffer buffer;
    buffer << static_cast<int>(1234);
    buffer << static_cast<float>(M_PI);
    buffer << String("!!!Hello World!!!");

    buffer.moveToBegin();
    int   numDigit{ 0 };
    float numFloat{ 0.0 };
    String msgHello{ "" };

    buffer >> numDigit;
    buffer >> numFloat;
    buffer >> msgHello;

    // make debug output here, check values.
    std::cout << "The integer number is .: " << numDigit << std::endl;
    std::cout << "The floating number is : " << numFloat << std::endl;
    std::cout << "The string message is .: " << msgHello << std::endl;

    // declare thread object.
    HelloThread aThread(buffer);

    // create and start thread, wait until it is started.
    aThread.createThread(NECommon::WAIT_INFINITE);

    // stop and destroy thread, clean resources. Wait until thread ends.
    aThread.shutdownThread(NECommon::WAIT_INFINITE);

    std::cout << "Exit application!" << std::endl;
    return 0;
}
