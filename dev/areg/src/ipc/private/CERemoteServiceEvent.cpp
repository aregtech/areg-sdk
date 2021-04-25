/************************************************************************
 * \file        areg/src/ipc/private/CERemoteServiceEvent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Send message events
 ************************************************************************/

#include "areg/src/ipc/private/CERemoteServiceEvent.hpp"


CESendMessageEventData::CESendMessageEventData(void)
    : mRemoteMessage    ( )
{
    ; // do nothing
}

CESendMessageEventData::CESendMessageEventData( const CERemoteMessage & remoteMessage )
    : mRemoteMessage    ( remoteMessage )
{
    ; // do nothing
}

CESendMessageEventData::CESendMessageEventData(const CESendMessageEventData & source)
    : mRemoteMessage    ( source.mRemoteMessage )
{
    ; // do nothing
}

CESendMessageEventData::~CESendMessageEventData(void)
{
    ; // do nothing
}

const CESendMessageEventData & CESendMessageEventData::operator=(const CESendMessageEventData & source)
{
    if ( static_cast<const CESendMessageEventData *>(this) != &source )
        mRemoteMessage  = source.mRemoteMessage;

    return (*this);
}
