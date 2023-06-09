/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ComponentInfo.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Component Info class implementation.
 *
 ************************************************************************/
#include "areg/component/private/ComponentInfo.hpp"

#include "areg/component/ComponentThread.hpp"
#include "areg/component/WorkerThread.hpp"

//////////////////////////////////////////////////////////////////////////
// ComponentInfo::_ImplWorkerThreadMap class implementation
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
ComponentInfo::ComponentInfo( ComponentThread& ownerThread, const String & roleName )
    : mComponentAddress ( ownerThread.getAddress(), roleName)
    , mMasterThread     ( ownerThread )
    , mWorkerThreadMap  ( )
{
}

//////////////////////////////////////////////////////////////////////////
// ComponentInfo class, methods
//////////////////////////////////////////////////////////////////////////
DispatcherThread * ComponentInfo::findEventConsumer( const RuntimeClassID& whichClass ) const
{
    DispatcherThread * result = nullptr;

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
        while (result == nullptr && dispThread != nullptr)
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

void ComponentInfo::registerWorkerThread( WorkerThread& workerThread )
{
    mWorkerThreadMap.registerResourceObject(workerThread.getAddress(), &workerThread);
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
