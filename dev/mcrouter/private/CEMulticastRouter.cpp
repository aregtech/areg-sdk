/************************************************************************
 * \file        mcrouter/private/CEMulticastRouter.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Router, Multicast Router Service process common part
 ************************************************************************/

#include "mcrouter/CEMulticastRouter.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/CEFile.hpp"
#include "areg/base/CEProcess.hpp"
#include "areg/base/CEString.hpp"
#include "areg/base/CEWideString.hpp"
#include "areg/appbase/CEApplication.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/trace/GETrace.h"

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
