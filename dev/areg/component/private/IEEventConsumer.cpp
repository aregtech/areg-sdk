/************************************************************************
 * \file        areg/component/private/IEEventConsumer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Consumer base class implementation
 *
 ************************************************************************/
#include "areg/component/IEEventConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// IEEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
IEEventConsumer::IEEventConsumer( void )
{
    ; // do nothing
}

IEEventConsumer::~IEEventConsumer( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
void IEEventConsumer::consumerRegistered( bool /* isRegistered */ )
{
    ; // do nothing
}

bool IEEventConsumer::preprocessEvent( Event & /* eventElem */ )
{
    return true;
}
