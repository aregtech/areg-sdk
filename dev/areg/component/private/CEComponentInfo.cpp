/************************************************************************
 * \file        areg/component/private/CEComponentInfo.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component Info class implementation.
 *
 ************************************************************************/
#include "areg/component/private/CEComponentInfo.hpp"

#include "areg/component/CEComponentThread.hpp"
#include "areg/component/CEWorkerThread.hpp"

//////////////////////////////////////////////////////////////////////////
// CEComponentInfo::_CEWorkerThreadMapImpl class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEComponentInfo::_CEWorkerThreadMap class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEComponentInfo::_CEWorkerThreadMap class, methods, overrides
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CEComponentInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEComponentInfo class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEComponentInfo::CEComponentInfo( CEComponentThread& masterThread, const char* roleName )
    : mComponentAddress (masterThread.GetThreadAddress(), roleName)
    , mMasterThread     (masterThread)
    , mWorkerThreadMap  ( )
{
    ; // do nothing
}

CEComponentInfo::~CEComponentInfo( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEComponentInfo class, methods
//////////////////////////////////////////////////////////////////////////
CEDispatcherThread * CEComponentInfo::FindEventConsumer( const CERuntimeClassID& whichClass ) const
{
    CEDispatcherThread * result = NULL;

    // firs check master thread.
    if (mMasterThread.HasRegisteredConsumer(whichClass))
    {
        result = static_cast<CEDispatcherThread *>(&mMasterThread);
    }
    else
    {
        // start checking component binded worker threads.
        mWorkerThreadMap.ResourceLock();

        CEThreadAddress Key;
        CEDispatcherThread * dispThread = static_cast<CEDispatcherThread *>(mWorkerThreadMap.GetResourceFirstKey(Key));
        while (result == NULL && dispThread != NULL)
        {
            if (dispThread->HasRegisteredConsumer(whichClass))
                result = dispThread;
            else
                dispThread = mWorkerThreadMap.GetResourceNextKey(Key);
        }

        mWorkerThreadMap.ResourceUnlock();
    }
    return result;
}

bool CEComponentInfo::RegisterWorkerThread( CEWorkerThread& workerThread )
{
    return mWorkerThreadMap.RegisterResourceObject(workerThread.GetThreadAddress(), &workerThread);
}

bool CEComponentInfo::UnregisterWorkerThread( CEWorkerThread& workerThread )
{
    return (mWorkerThreadMap.UnregisterResourceObject(workerThread.GetThreadAddress()) == &workerThread);
}

bool CEComponentInfo::IsWorkerThreadRegistered( CEWorkerThread& workerThread ) const
{
    return IsWorkerThreadAddress(workerThread.GetThreadAddress());
}

bool CEComponentInfo::IsMasterThreadAddress( const CEThreadAddress& threadAddress ) const
{
    return (threadAddress == mMasterThread.GetThreadAddress());
}
