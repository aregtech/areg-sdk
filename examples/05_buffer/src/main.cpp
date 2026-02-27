//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG. All rights reserved.
// Description : This project demonstrates the use of a Shared Buffer object, 
//               initialization streaming, read and write, passing in 
//               multithreading environment.
//============================================================================

#ifndef _USE_MATH_DEFINES
    #define _USE_MATH_DEFINES
#endif // _USE_MATH_DEFINES

#include "areg/base/GEGlobal.h"
#include "areg/base/GEMacros.h"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"

#include <iostream>
#include <cmath>    // for M_PI

#ifdef  _MSC_VER
    #pragma comment(lib, "areg")
#endif // _MSC_VER

//! \brief Thread to read buffer and output message
class HelloThread : public Thread, protected ThreadConsumer
{
public:
    explicit HelloThread(SharedBuffer& buffer)
        : Thread(*this, "HelloThread") // set consumer and name
        , mBuffer(buffer)
    {
    }

protected:
/************************************************************************/
// ThreadConsumer interface
/************************************************************************/
    void on_thread_runs() override
    {
        std::cout << "Thread [" << name() << "] started..." << std::endl;

        int32_t numDigit{};
        float numPI{};
        String strMsg{};

        mBuffer.move_to_begin();
        mBuffer >> numDigit >> numPI >> strMsg;

        std::cout << "*********************************" << std::endl;
        std::cout << "BEGIN dump buffer data .........." << std::endl;
        std::cout << "Saved integer  : " << numDigit << std::endl;
        std::cout << "Saved PI number: " << numPI << std::endl;
        std::cout << "Saved string   : " << strMsg.as_string() << std::endl;
        std::cout << "END dump buffer data ............" << std::endl;
        std::cout << "*********************************" << std::endl;

        std::cout << "Thread [" << name() << "] completed job." << std::endl;
    }

private:
    SharedBuffer& mBuffer;
};

//////////////////////////////////////////////////////////////////////////
// Demo: write and read data from binary buffer.
//////////////////////////////////////////////////////////////////////////
int main()
{
    std::cout << "Demo: write and read data from binary buffer..." << std::endl;

    SharedBuffer buffer;
    buffer << 1234 << static_cast<float>(M_PI) << String("!!!Hello World!!!");

    HelloThread aThread(buffer);

    // Start thread and wait until it starts
    aThread.create_thread(NECommon::WAIT_INFINITE);

    // Stop thread and clean resources
    aThread.shutdown_thread(NECommon::WAIT_INFINITE);

    std::cout << "Exit application!" << std::endl;
    return 0;
}
