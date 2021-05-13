/************************************************************************
 * \file        areg/base/private/IEThreadConsumer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread Consumer class
 *
 ************************************************************************/
#include "areg/base/IEThreadConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// IEThreadConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
IEThreadConsumer::IEThreadConsumer( void )
{
    ; // do nothing
}

IEThreadConsumer::~IEThreadConsumer( void )
{
    ; // do nothing
}

bool IEThreadConsumer::onThreadRegistered(Thread * /*threadObj*/)
{
    return true;
}

void IEThreadConsumer::onThreadUnregistering(void)
{

}

int IEThreadConsumer::onThreadExit(void)
{
    return IEThreadConsumer::EXIT_NORMAL;
}
