/************************************************************************
 * \file        areg/component/private/ComponentInfo.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component Info class implementation.
 *
 ************************************************************************/
#include "areg/component/private/ComponentInfo.hpp"

#include "areg/component/ComponentThread.hpp"
#include "areg/component/WorkerThread.hpp"

//////////////////////////////////////////////////////////////////////////
// ComponentInfo::_WorkerThreadMapImpl class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ComponentInfo::_WorkerThreadMap class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ComponentInfo::_WorkerThreadMap class, methods, overrides
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ComponentInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ComponentInfo class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ComponentInfo::ComponentInfo( ComponentThread& masterThread, const char* roleName )
    : mComponentAddress (masterThread.getAddress(), roleName)
    , mMasterThread     (masterThread)
    , mWorkerThreadMap  ( )
{
    ; // do nothing
}

ComponentInfo::~ComponentInfo( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// ComponentInfo class, methods
//////////////////////////////////////////////////////////////////////////
DispatcherThread * ComponentInfo::findEventConsumer( const RuntimeClassID& whichClass ) const
{
    DispatcherThread * result = NULL;

    // firs check master thread.
    if (mMasterThread.hasRegisteredConsumer(whichClass))
    {
        result = static_cast<DispatcherThread *>(&mMasterThread);
    }
    else
    {
        // start checking component binded worker threads.
        mWorkerThreadMap.lock();

        ThreadAddress Key;
        DispatcherThread * dispThread = static_cast<DispatcherThread *>(mWorkerThreadMap.resourceFirstKey(Key));
        while (result == NULL && dispThread != NULL)
        {
            if (dispThread->hasRegisteredConsumer(whichClass))
                result = dispThread;
            else
                dispThread = mWorkerThreadMap.resourceNextKey(Key);
        }

        mWorkerThreadMap.unlock();
    }
    return result;
}

bool ComponentInfo::registerWorkerThread( WorkerThread& workerThread )
{
    return mWorkerThreadMap.registerResourceObject(workerThread.getAddress(), &workerThread);
}

bool ComponentInfo::unregisterWorkerThread( WorkerThread& workerThread )
{
    return (mWorkerThreadMap.unregisterResourceObject(workerThread.getAddress()) == &workerThread);
}

bool ComponentInfo::isWorkerThreadRegistered( WorkerThread& workerThread ) const
{
    return isWorkerThreadAddress(workerThread.getAddress());
}

bool ComponentInfo::isMasterThreadAddress( const ThreadAddress& threadAddress ) const
{
    return (threadAddress == mMasterThread.getAddress());
}
