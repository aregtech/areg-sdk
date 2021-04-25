/************************************************************************
 * \file        areg/src/component/private/IEEventConsumer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Consumer base class implementation
 *
 ************************************************************************/
#include "areg/src/component/IEEventConsumer.hpp"

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
void IEEventConsumer::ConsumerRegistered( bool /* isRegistered */ )
{
    ; // do nothing
}

bool IEEventConsumer::PreprocessEvent( CEEvent & /* eventElem */ )
{
    return true;
}
