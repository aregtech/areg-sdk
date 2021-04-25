/************************************************************************
 * \file        areg/src/component/private/CETimerManagingEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform. Event Class to manage Timer
 *
 ************************************************************************/
#include "areg/src/component/private/CETimerManagingEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// CETimerManagingEventData class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Operator
//////////////////////////////////////////////////////////////////////////
const CETimerManagingEventData & CETimerManagingEventData::operator = ( const CETimerManagingEventData & src )
{
    if (static_cast<const CETimerManagingEventData *>(this) != &src)
    {
        mTimer  = src.mTimer;
        mAction = src.mAction;
    }
    return (*this);
}
