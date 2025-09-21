//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project demonstrates the use of synchronization objects,
//               including waiting for multiple synchronization objects such 
//               as mutex and synchronization events, that differ by types 
//               such as automatic or manual reset, or initially locked or 
//               unlocked, etc, and how they may be combined in one waiting 
//               scope. For example, a thread can wait for one, many or a
//               mixture of synchronization objects such as Mutex or 
//               synchronization events.
//
//               The synchronization events are not part of POSIX standard, 
//               but exists in Windows. By business logic they have similarity
//               with POSIX signals of conditional variables. Synchronization
//               events are powerful and useful objects.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/SynchObjects.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#ifdef  _MSC_VER
    // link with areg library, valid only for MSVC
    #pragma comment(lib, "areg")
#endif // _MSC_VER

using namespace std::chrono_literals;
using chrono_clock  = std::chrono::high_resolution_clock;
using duration_ms   = std::chrono::duration<double, std::milli>;

//////////////////////////////////////////////////////////////////////////
// Event synchronization object
//////////////////////////////////////////////////////////////////////////
SynchEvent  gEvent(false, true);    //!< non-signaled, auto-reset event
std::string data{};                 //!< A text to output
constexpr auto sleepMs{ 500ms };    //!< A timeout to put thread to the sleep

void workerThread()
{
    const auto start = chrono_clock::now();
    Lock lock(gEvent);

    // after the wait, we own the lock
    duration_ms elapsed = chrono_clock::now() - start;
    std::cout << "Worker thread is processing data, event waiting timeout " << elapsed.count() << " ms\n";
    data += " after processing";

    std::cout << "Worker thread signals data processing completed, put worker thread to sleep for " << sleepMs.count() << " ms\n";
    std::this_thread::sleep_for(sleepMs);

    gEvent.setEvent();  // manual set event
}

int main()
{
    data = "Example data";
    std::thread worker(workerThread);

    gEvent.lock();
    std::cout << "main() signals data ready for processing, put thread to sleep for " << sleepMs.count() << " ms\n";

    std::this_thread::sleep_for(sleepMs);
    gEvent.setEvent();

    const auto start = chrono_clock::now();
    gEvent.lock();
    duration_ms elapsed = chrono_clock::now() - start;
    std::cout << "Back in main(), data = " << data << ", event waiting timeout " << elapsed.count() << " ms\n";

    worker.join();
}
