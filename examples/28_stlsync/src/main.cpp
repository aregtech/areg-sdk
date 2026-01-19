//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : This project demonstrates the use of synchronization objects,
//               including waiting for multiple synchronization objects such 
//               as mutex and synchronization events, that differ by types 
//               such as automatic or manual reset, or initially locked or 
//               unlocked, etc, and how they may be combined in one waiting 
//               scope. For example, a thread can wait for one, many or a
//               mixture of synchronization objects such as Mutex or 
//               synchronization events.
//
//               The event synchronization primitives are not part of POSIX standard, 
//               but exists in Windows. By business logic they have similarity
//               with POSIX signals of conditional variables. Synchronization
//               events are powerful and useful objects.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/SyncObjects.hpp"

#include <iostream>
#include <string>
#include <thread>

#ifdef  _MSC_VER
    // link with areg library, valid only for MSVC
    #pragma comment(lib, "areg")
#endif // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Event synchronization object
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   This example is Areg adapted version of the conditional variable example at cppreference.com
 *          URL: https://en.cppreference.com/w/cpp/thread/condition_variable.html
 *          Compare the version of STL and Areg. Both do the same.
 **/
SyncEvent   ready(true, true);      //!< non-signaled, auto-reset event
SyncEvent   processed(true, false); //!< non-signaled, manual-reset event
std::string data{};                 //!< A text to output

void workerThread()
{
    Lock lock(ready);

    // after the wait, we own the lock
    std::cout << "Worker thread is processing data\n";
    data += " after processing";
    std::cout << "Worker thread signals data processing completed\n";
    processed.setEvent();  // manual set event
}

int main()
{
    data = "Example data";
    std::thread worker(workerThread);

    std::cout << "main() signals data ready for processing\n";
    ready.setEvent();   // signal the worker thread

    processed.lock();   // wait for worker thread to signal
    std::cout << "Back in main(), data = " << data << '\n';

    worker.join();
    return 0;
}
