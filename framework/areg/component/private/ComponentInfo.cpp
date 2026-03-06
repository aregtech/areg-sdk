/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ComponentInfo.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component Info class implementation.
 *
 ************************************************************************/
#include "areg/component/private/ComponentInfo.hpp"

#include "areg/component/ComponentThread.hpp"
#include "areg/component/WorkerThread.hpp"
namespace areg {

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
    : mComponentAddress ( ownerThread.address(), roleName)
    , mMasterThread     ( ownerThread )
    , mWorkerThreadMap  ( )
{
}

//////////////////////////////////////////////////////////////////////////
// ComponentInfo class, methods
//////////////////////////////////////////////////////////////////////////
DispatcherThread * ComponentInfo::find_event_consumer( const RuntimeClassID& whichClass ) const noexcept
{
    DispatcherThread * result = nullptr;

    // firs check master thread.
    if (mMasterThread.has_registered_consumer(whichClass))
    {
        result = static_cast<DispatcherThread *>(&mMasterThread);
    }
    else
    {
        // start checking component binded worker threads.
        mWorkerThreadMap.lock();

        ThreadAddress Key;
        DispatcherThread * dispThread = static_cast<DispatcherThread *>(mWorkerThreadMap.resource_first_key(Key));
        while (result == nullptr && dispThread != nullptr)
        {
            if (dispThread->has_registered_consumer(whichClass))
                result = dispThread;
            else
                dispThread = mWorkerThreadMap.resource_next_key(Key);
        }

        mWorkerThreadMap.unlock();
    }
    return result;
}

void ComponentInfo::register_worker_thread( WorkerThread& workerThread )
{
    mWorkerThreadMap.register_resource_object(workerThread.address(), &workerThread);
}

bool ComponentInfo::unregister_worker_thread( WorkerThread& workerThread )
{
    return (mWorkerThreadMap.unregister_resource_object(workerThread.address()) == &workerThread);
}

bool ComponentInfo::is_worker_registered( WorkerThread& workerThread ) const
{
    return is_worker_thread(workerThread.address());
}

bool ComponentInfo::is_master_thread( const ThreadAddress& threadAddress ) const
{
    return (threadAddress == mMasterThread.address());
}

} // namespace areg
