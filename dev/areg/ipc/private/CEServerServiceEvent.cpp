/************************************************************************
 * \file        areg/ipc/private/CEServerServiceEvent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Service Event
 ************************************************************************/
#include "areg/ipc/private/CEServerServiceEvent.hpp"

CEServerServiceEventData::CEServerServiceEventData(eServerServiceCommands cmdService)
    : mServiceCommand   ( cmdService )
    , mMessageData      ( )
{
    ; // do nothing
}

CEServerServiceEventData::CEServerServiceEventData(eServerServiceCommands cmdService, const CERemoteMessage & msgService)
    : mServiceCommand   ( cmdService )
    , mMessageData      ( msgService )
{
    ; // do nothing
}

CEServerServiceEventData::CEServerServiceEventData(const CEServerServiceEventData & source)
    : mServiceCommand   ( source.mServiceCommand )
    , mMessageData      ( source.mMessageData )
{
    ; // do nothing
}

CEServerServiceEventData::~CEServerServiceEventData(void)
{
    ; // do nothing
}

const CEServerServiceEventData & CEServerServiceEventData::operator = (const CEServerServiceEventData & source)
{
    if ( static_cast<const CEServerServiceEventData *>(this) != &source )
    {
        mServiceCommand = source.mServiceCommand;
        mMessageData    = source.mMessageData;
    }
    return (*this);
}
