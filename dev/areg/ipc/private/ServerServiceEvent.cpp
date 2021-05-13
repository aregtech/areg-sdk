/************************************************************************
 * \file        areg/ipc/private/ServerServiceEvent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Service Event
 ************************************************************************/
#include "areg/ipc/private/ServerServiceEvent.hpp"

ServerServiceEventData::ServerServiceEventData(eServerServiceCommands cmdService)
    : mServiceCommand   ( cmdService )
    , mMessageData      ( )
{
    ; // do nothing
}

ServerServiceEventData::ServerServiceEventData(eServerServiceCommands cmdService, const RemoteMessage & msgService)
    : mServiceCommand   ( cmdService )
    , mMessageData      ( msgService )
{
    ; // do nothing
}

ServerServiceEventData::ServerServiceEventData(const ServerServiceEventData & source)
    : mServiceCommand   ( source.mServiceCommand )
    , mMessageData      ( source.mMessageData )
{
    ; // do nothing
}

ServerServiceEventData::~ServerServiceEventData(void)
{
    ; // do nothing
}

const ServerServiceEventData & ServerServiceEventData::operator = (const ServerServiceEventData & source)
{
    if ( static_cast<const ServerServiceEventData *>(this) != &source )
    {
        mServiceCommand = source.mServiceCommand;
        mMessageData    = source.mMessageData;
    }
    return (*this);
}
