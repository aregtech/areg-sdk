/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventDispatcher.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Event Dispatcher class
 *
 ************************************************************************/
#include "areg/component/EventDispatcher.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
EventDispatcher::EventDispatcher( const String & name, uint32_t maxQeueue )
    : EventDispatcherBase   ( name, maxQeueue )
    , IEThreadConsumer      (  )
    , IEEventRouter         (  )

    , mDispatcherThread     ( nullptr )
{
}

EventDispatcher::~EventDispatcher( void )
{
    mDispatcherThread   = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class, methods
//////////////////////////////////////////////////////////////////////////
bool EventDispatcher::onThreadRegistered( Thread * threadObj )
{
    mDispatcherThread = RUNTIME_CAST(threadObj, DispatcherThread);
    ASSERT(mDispatcherThread != nullptr);

    EventDispatcherBase::removeAllEvents( );
    return EventDispatcherBase::mEventExit.resetEvent();
}

void EventDispatcher::onThreadUnregistering( void )
{
    stopDispatcher();
    mDispatcherThread   = nullptr;
}

void EventDispatcher::onThreadRuns( void )
{
    ASSERT(mDispatcherThread != nullptr);
    startDispatcher();
}

int EventDispatcher::onThreadExit( void )
{
    exitDispatcher( );
    mDispatcherThread   = nullptr;
    return static_cast<int>(IEThreadConsumer::eExitCodes::ExitNormal);
}

bool EventDispatcher::postEvent( Event& eventElem )
{
    return queueEvent(eventElem);
}
