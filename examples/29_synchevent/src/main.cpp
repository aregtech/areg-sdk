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
//               The event synchronization primitives are not part of POSIX standard, 
//               but exists in Windows. By business logic they have similarity
//               with POSIX signals of conditional variables. Synchronization
//               events are powerful and useful objects.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/DateTime.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/Thread.hpp"

#include <iostream>
#include <string>

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
SynchEvent  gEvtReady(true, true);      //!< non-signaled, auto-reset event
SynchEvent  gEvtProcess(true, true);  //!< non-signaled, auto-reset event
std::string gData{};                 //!< A text to output

class WorkerThread : public Thread, protected IEThreadConsumer
{
public:
    WorkerThread(void)
        : Thread(static_cast<IEThreadConsumer&>(*this), "WorkerThread") // set consumer and the name
        , IEThreadConsumer()
    {
    }

protected:
    /************************************************************************/
    // IEThreadConsumer interface overrides
    /************************************************************************/
    //! \brief  This callback is triggered when thread runs and fully operable.
    virtual void onThreadRuns(void) override
    {
        TIME64 start = DateTime::getNow();
        Thread::sleep(NECommon::WAIT_1_SECOND); // simulate some work, force the event to be signaled before wait
        VERIFY(gEvtReady.lock()); // Verify that the event is signaled

        // after the wait, we own the lock
        std::chrono::nanoseconds ns{ DateTime::getNow() - start };
        std::chrono::microseconds ms{ std::chrono::duration_cast<std::chrono::microseconds>(ns) };

        std::cout << "Worker thread is processing data. Wait timeout: " << (static_cast<float>(ms.count()) / 1000.0f) << " ms\n";
        gData += " after processing";
        std::cout << "Worker thread signals data processing completed\n";
        VERIFY(gEvtProcess.setEvent()); // verify that auto-reset event is signaled
    }
};


int main()
{
    VERIFY(gEvtReady.lock(NECommon::WAIT_100_MILLISECONDS) == false);       // should timeout, since event is non-signaled
    VERIFY(gEvtProcess.lock(NECommon::WAIT_100_MILLISECONDS) == false);   // should timeout, since event is non-signaled

    gData = "Example data";
    std::cout << "29_synchevent::main() signals data ready for processing\n";
    VERIFY(gEvtReady.setEvent());   // verify and signal auto-reset event before worker thread starts and owns

    WorkerThread aThread;
    aThread.createThread(NECommon::WAIT_INFINITE);  // wait until thread is started

    // simulate some work to make sure that the worker thread signaled event before it is locked
    TIME64 start = DateTime::getNow();
    Thread::sleep(NECommon::WAIT_1_SECOND * 2);

    // make sure that the `gEvtReady` event remains in non-signaled state even when worker thread completed job.
    VERIFY(gEvtReady.lock(NECommon::WAIT_10_MILLISECONDS) == false);
    gEvtProcess.lock();   // wait for worker thread to signal

    // after the wait, we own the lock
    std::chrono::nanoseconds ns{ DateTime::getNow() - start };
    std::chrono::microseconds ms{ std::chrono::duration_cast<std::chrono::microseconds>(ns) };
    std::cout << "Back in main(), data = " << gData << ". Wait timeout: " << (static_cast<float>(ms.count()) / 1000.0f) << " ms\n";

    aThread.shutdownThread(NECommon::WAIT_INFINITE); // wait until thread is stopped
    return 0;
}
