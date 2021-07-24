/************************************************************************
 * \file        areg/component/private/TimerManagingEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform. Event Class to manage Timer
 *
 ************************************************************************/
#include "areg/component/private/TimerManagingEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// TimerManagingEventData class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Operator
//////////////////////////////////////////////////////////////////////////
const TimerManagingEventData & TimerManagingEventData::operator = ( const TimerManagingEventData & src )
{
    if (static_cast<const TimerManagingEventData *>(this) != &src)
    {
        mTimer  = src.mTimer;
        mAction = src.mAction;
    }
    return (*this);
}
