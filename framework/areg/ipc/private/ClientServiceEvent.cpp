/************************************************************************
 * \file        areg/ipc/private/ClientServiceEvent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Client Service Event
 ************************************************************************/

#include "areg/ipc/private/ClientServiceEvent.hpp"

ClientServiceEventData::ClientServiceEventData( ClientServiceEventData::eClientServiceCommands cmdService )
    : mServiceCommand   ( cmdService )
{
    ; // do nothing
}

ClientServiceEventData::ClientServiceEventData( const ClientServiceEventData & source )
    : mServiceCommand   ( source.mServiceCommand )
{
    ; // do nothing
}

ClientServiceEventData::~ClientServiceEventData( void )
{
    ; // do nothing
}

const ClientServiceEventData & ClientServiceEventData::operator = ( const ClientServiceEventData & source )
{
    mServiceCommand = source.mServiceCommand;
    return (*this);
}
