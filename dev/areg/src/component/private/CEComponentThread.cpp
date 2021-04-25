/************************************************************************
 * \file        areg/src/component/private/CEComponentThread.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component Thread.
 *
 ************************************************************************/
#include "areg/src/component/CEComponentThread.hpp"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/component/CEComponentLoader.hpp"
#include "areg/src/component/NERegistry.hpp"

//////////////////////////////////////////////////////////////////////////
// CEComponentThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement runtime
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME(CEComponentThread, CEDispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Implement static methods
//////////////////////////////////////////////////////////////////////////
CEComponent* CEComponentThread::GetCurrentComponent( void )
{
    CEComponentThread* comThread = CEComponentThread::_getCurrentComponentThread();
    return (comThread != NULL ? comThread->mCurrentComponent : NULL);
}

bool CEComponentThread::SetCurrentComponent( CEComponent* curComponent )
{
    CEComponentThread* comThread = CEComponentThread::_getCurrentComponentThread();
    if (comThread != NULL)
        comThread->mCurrentComponent = curComponent;
    return (comThread != NULL);
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEComponentThread::CEComponentThread( const char* threadName )
    : CEDispatcherThread    ( threadName )

    , mCurrentComponent     ( NULL )
    , mListComponent        ( )
{
    ; // do nothing
}

CEComponentThread::~CEComponentThread( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
bool CEComponentThread::PostEvent( CEEvent& eventElem )
{
    return CEEventDispatcher::PostEvent(eventElem);
}

bool CEComponentThread::RunDispatcher( void )
{
    bool result = false;
    if (CreateComponents() > 0)
    {
        StartComponents();
        result = CEDispatcherThread::RunDispatcher();
    }

    ShutdownComponents();
    return result;
}

int CEComponentThread::CreateComponents( void )
{
    OUTPUT_DBG("Starting to create components in thread [ %s ].", GetThreadName().GetBuffer());
    int result = 0;
    const NERegistry::CEComponentList& comList = CEComponentLoader::GetThreadComponentList(GetThreadName());
    if (comList.IsValid())
    {
        for (int i = 0; i < comList.GetSize(); ++ i)
        {
            const NERegistry::CEComponentEntry& entry = comList[i];
            if (entry.IsValid() && entry.mFuncCreate != NULL)
            {
                CEComponent *comObj = CEComponent::LoadComponent(entry, self());
                if (comObj != NULL)
                {
                    OUTPUT_DBG("Component [ %s ] in thread [ %s ] has been created...", comObj->GetRoleName().GetBuffer(), GetThreadName().GetBuffer());
                    mListComponent.AddTail(comObj);
                    result ++;
                }
                else
                {
                    OUTPUT_ERR("Failed creating component [ %s ] in UNKNOWN NULL thread");
                }
            }
        }
    }
    return result;
}

void CEComponentThread::DestroyComponents( void )
{
    OUTPUT_DBG("Going to destroy components in thread [ %s ]. There are [ %d ] components in the thread.", GetThreadName().GetBuffer(), mListComponent.GetSize());
    while (mListComponent.IsEmpty() == false)
    {
        CEComponent* comObj = mListComponent.RemoveTail();
        if (comObj != NULL)
        {
            OUTPUT_DBG("Destroying component [ %s ] in thread [ %s ]...", comObj->GetRoleName().GetBuffer(), GetThreadName().GetBuffer());
            const NERegistry::CEComponentEntry& entry = CEComponentLoader::SearchComponentEntry(comObj->GetRoleName(), GetThreadName());
            if (entry.IsValid() && entry.mFuncDelete != NULL)
                CEComponent::UnloadComponent(*comObj, entry);
            else
                delete comObj;
        }
    }
}

void CEComponentThread::StartComponents( void )
{
    OUTPUT_DBG("Starting components in thread [ %s ].", GetThreadName().GetBuffer());
    LISTPOS pos = mListComponent.GetTailPosition();
    while (pos != NULL)
    {
        CEComponent* comObj = mListComponent.GetPrev(pos);
        ASSERT(comObj != NULL);
        comObj->StartupComponent(self());
        OUTPUT_DBG("Component [ %s ] in thread [ %s ] has been started...", comObj->GetRoleName().GetBuffer(), GetThreadName().GetBuffer());
    }
}

void CEComponentThread::ShutdownComponents( void )
{
    OUTPUT_DBG("Going to shutdown components in thread [ %s ].", GetThreadName().GetBuffer());
    LISTPOS pos = mListComponent.GetHeadPosition();
    while (pos != NULL)
    {
        CEComponent* comObj = mListComponent.GetNext(pos);
        ASSERT(comObj != NULL);
        comObj->ShutdownComponent(self());
        OUTPUT_DBG("Shutdown component [ %s ] in thread [ %s ]...", comObj->GetRoleName().GetBuffer(), GetThreadName().GetBuffer());
    }
}

CEDispatcherThread* CEComponentThread::GetEventConsumerThread( const CERuntimeClassID& whichClass )
{
    CEDispatcherThread* result = HasRegisteredConsumer(whichClass) ? static_cast<CEDispatcherThread *>(this) : NULL;
    if (result == NULL)
    {
        LISTPOS pos = mListComponent.GetHeadPosition();
        while (pos != NULL && result == NULL)
        {
            CEComponent* comObj = mListComponent.GetNext(pos);
            ASSERT(comObj != NULL);
            result = comObj->FindEventConsumer(whichClass);
        }
    }
    return result;
}

CEThread::eCompletionStatus CEComponentThread::DestroyThread( unsigned int waitForStopMs /*= CEThread::DO_NOT_WAIT*/ )
{
    CEThread::eCompletionStatus result = CEDispatcherThread::DestroyThread(waitForStopMs);
    DestroyComponents();
    return result;
}

void CEComponentThread::ShutdownThread( void )
{
    LISTPOS pos = mListComponent.GetHeadPosition();
    while (pos != NULL)
    {
        CEComponent* comObj = mListComponent.GetNext(pos);
        ASSERT(comObj != NULL);
        comObj->NotifyComponentShutdown(self());
        OUTPUT_DBG("The component [ %s ] is notified thread [ %s ] is going to shutdown!", comObj->GetRoleName().GetBuffer(), GetThreadName().GetBuffer());
    }
    CEDispatcherThread::ShutdownThread();
}

bool CEComponentThread::CompletionWait( unsigned int waitForCompleteMs /*= CEThread::WAIT_INFINITE */ )
{
    LISTPOS pos = mListComponent.GetHeadPosition();
    while ( pos != NULL )
    {
        CEComponent* comObj = mListComponent.GetNext(pos);
        ASSERT(comObj != NULL);
        comObj->WaitComponentCompletion(waitForCompleteMs);
    }

    return CEDispatcherThread::CompletionWait(waitForCompleteMs);
}
