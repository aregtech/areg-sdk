#ifndef AREG_COMPONENT_PRIVATE_SORTEDEVENTSTACK_HPP
#define AREG_COMPONENT_PRIVATE_SORTEDEVENTSTACK_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/SortedEventStack.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, sorted event stack objects for internal and external event queues.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEStack.hpp"
#include "areg/component/Event.hpp"

class RuntimeClassID;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

/**
 * \brief   Sorted stack to store events by priority.
 **/
class SortedEventStack  : protected TELockStack<Event *>
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
public:
    typedef enum E_MessageStackType
    {
          MessageStackInternal
        , MessageStackExternal
    } eMessageStackType;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    explicit SortedEventStack(SortedEventStack::eMessageStackType stackType);

    ~SortedEventStack(void);

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    void deleteAllEvents(void);

    uint32_t deleteAllLowerPriority(Event::eEventPriority eventPrio);

    uint32_t deleteAllExceptClass(const RuntimeClassID& eventClassId);

    uint32_t deleteAllMatchPriority(Event::eEventPriority eventPrio);

    uint32_t deleteAllMatchClass(const RuntimeClassID& eventClassId);

    uint32_t pushEvent(Event * newEvent);

    uint32_t popEvent(Event** stackEvent);

    inline bool isEmpty(void) const;

    inline uint32_t getCount(void) const;

    inline eMessageStackType getStackType(void) const;

    inline bool lockStack(void);

    inline void unlockStack(void);

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline void _pushAtEnd(Event * newEvent);

    inline void _pushAfterPrio(Event * newEvent, Event::eEventPriority eventPrio);

    inline void _pushBeforePrio(Event * newEvent, Event::eEventPriority eventPrio);

    inline void _pushAtBegin(Event * newEvent);

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
private:
    const eMessageStackType   mStackType;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    SortedEventStack(void) = delete;
    DECLARE_NOCOPY_NOMOVE(SortedEventStack);
};

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER


inline bool SortedEventStack::isEmpty(void) const
{
    Lock lock(mSynchObject);
    return mValueList.empty();
}

inline uint32_t SortedEventStack::getCount(void) const
{
    return static_cast<uint32_t>(mValueList.size());
}

inline SortedEventStack::eMessageStackType SortedEventStack::getStackType(void) const
{
    return mStackType;
}

inline bool SortedEventStack::lockStack(void)
{
    return lock();
}

inline void SortedEventStack::unlockStack(void)
{
    unlock();
}

#endif  // AREG_COMPONENT_PRIVATE_SORTEDEVENTSTACK_HPP
