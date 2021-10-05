/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/RemoteServiceEvent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Send message events
 ************************************************************************/

#include "areg/ipc/RemoteServiceEvent.hpp"

#include <utility>

SendMessageEventData::SendMessageEventData( const RemoteMessage & remoteMessage )
    : mRemoteMessage    ( remoteMessage )
{
}

SendMessageEventData::SendMessageEventData( SendMessageEventData && source ) noexcept
    : mRemoteMessage    ( std::move(source.mRemoteMessage) )
{
}

SendMessageEventData & SendMessageEventData::operator = ( SendMessageEventData && source ) noexcept
{
    mRemoteMessage  = std::move(source.mRemoteMessage);
    return (*this);
}
