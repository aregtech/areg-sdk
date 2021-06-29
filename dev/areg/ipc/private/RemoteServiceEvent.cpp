/************************************************************************
 * \file        areg/ipc/private/RemoteServiceEvent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Send message events
 ************************************************************************/

#include "areg/ipc/private/RemoteServiceEvent.hpp"


SendMessageEventData::SendMessageEventData(void)
    : mRemoteMessage    ( )
{
    ; // do nothing
}

SendMessageEventData::SendMessageEventData( const RemoteMessage & remoteMessage )
    : mRemoteMessage    ( remoteMessage )
{
    ; // do nothing
}

SendMessageEventData::SendMessageEventData(const SendMessageEventData & source)
    : mRemoteMessage    ( source.mRemoteMessage )
{
    ; // do nothing
}

SendMessageEventData::~SendMessageEventData(void)
{
    ; // do nothing
}

const SendMessageEventData & SendMessageEventData::operator=(const SendMessageEventData & source)
{
    if ( static_cast<const SendMessageEventData *>(this) != &source )
        mRemoteMessage  = source.mRemoteMessage;

    return (*this);
}
