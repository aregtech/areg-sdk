/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventDispatcher.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Dispatcher class
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
EventDispatcher::EventDispatcher( const areg::String & name, uint32_t maxQeueue )
    : EventDispatcherBase   ( name, maxQeueue )
    , ThreadConsumer      (  )
    , EventRouter         (  )

    , mDispatcherThread     ( nullptr )
{
}

EventDispatcher::~EventDispatcher()
{
    mDispatcherThread   = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class, methods
//////////////////////////////////////////////////////////////////////////
bool EventDispatcher::onThreadRegistered( Thread * threadObj )
{
    mDispatcherThread = AREG_RUNTIME_CAST(threadObj, DispatcherThread);
    ASSERT(mDispatcherThread != nullptr);

    EventDispatcherBase::removeAllEvents( );
    return EventDispatcherBase::mEventExit.resetEvent();
}

void EventDispatcher::onThreadUnregistering()
{
    stopDispatcher();
    mDispatcherThread   = nullptr;
}

void EventDispatcher::onThreadRuns()
{
    ASSERT(mDispatcherThread != nullptr);
    startDispatcher();
}

int32_t EventDispatcher::onThreadExit()
{
    exitDispatcher( );
    mDispatcherThread   = nullptr;
    return static_cast<int32_t>(ThreadConsumer::ExitCode::Normal);
}

bool EventDispatcher::postEvent( Event& eventElem )
{
    return queueEvent(eventElem);
}
