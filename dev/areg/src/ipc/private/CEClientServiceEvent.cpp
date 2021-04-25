/************************************************************************
 * \file        areg/src/ipc/private/CEClientServiceEvent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Client Service Event
 ************************************************************************/

#include "areg/src/ipc/private/CEClientServiceEvent.hpp"

CEClientServiceEventData::CEClientServiceEventData( CEClientServiceEventData::eClientServiceCommands cmdService )
    : mServiceCommand   ( cmdService )
{
    ; // do nothing
}

CEClientServiceEventData::CEClientServiceEventData( const CEClientServiceEventData & source )
    : mServiceCommand   ( source.mServiceCommand )
{
    ; // do nothing
}

CEClientServiceEventData::~CEClientServiceEventData( void )
{
    ; // do nothing
}

const CEClientServiceEventData & CEClientServiceEventData::operator = ( const CEClientServiceEventData & source )
{
    mServiceCommand = source.mServiceCommand;
    return (*this);
}
