/************************************************************************
 * \file        mcrouter/private/CEMulticastRouter.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Router, Multicast Router Service process common part
 ************************************************************************/

#include "mcrouter/src/CEMulticastRouter.hpp"
#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/CEFile.hpp"
#include "areg/src/base/CEProcess.hpp"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/CEWideString.hpp"
#include "areg/src/appbase/CEApplication.hpp"
#include "areg/src/appbase/NEApplication.hpp"
#include "areg/src/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CEMulticastRouter class implementation
//////////////////////////////////////////////////////////////////////////
CEMulticastRouter & CEMulticastRouter::GetMulticastRouter(void)
{
    static CEMulticastRouter _broker;
    return _broker;
}

CEMulticastRouter::CEMulticastRouter( void )
    : mRouterState  ( NEMulticastRouterSettings::RouterStopped )
    , mServiceCmd   ( CEMulticastRouter::CMD_Undefined )
    , mQuitEvent    ( false, false )
    , mServiceServer( )
    , mSvcHandle    ( NULL )
    , mSeMHandle    ( NULL )
{
    ; // do nothing
}

CEMulticastRouter::~CEMulticastRouter( void )
{
    _freeResources( );
}
