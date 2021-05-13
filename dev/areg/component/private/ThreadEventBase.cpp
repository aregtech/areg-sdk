/************************************************************************
 * \file        areg/component/private/ThreadEventBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Multithreading Event base class.
 *
 ************************************************************************/
#include "areg/component/private/ThreadEventBase.hpp"

//////////////////////////////////////////////////////////////////////////
// ThreadEventBase class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(ThreadEventBase, Event)

ThreadEventBase::ThreadEventBase( Event::eEventType eventType )
    : Event   (eventType)
{
    ; // do nothing
}

ThreadEventBase::~ThreadEventBase( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// ThreadEventConsumerBase class implementation
//////////////////////////////////////////////////////////////////////////

ThreadEventConsumerBase::ThreadEventConsumerBase( void )
    : IEEventConsumer  ( )
{
    ; // do nothing
}

ThreadEventConsumerBase::~ThreadEventConsumerBase( void )
{
    ; // do nothing
}
