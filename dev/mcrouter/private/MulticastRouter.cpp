/************************************************************************
 * \file        mcrouter/private/MulticastRouter.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Router, Multicast Router Service process common part
 ************************************************************************/

#include "mcrouter/MulticastRouter.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class implementation
//////////////////////////////////////////////////////////////////////////
MulticastRouter & MulticastRouter::getInstance(void)
{
    static MulticastRouter _broker;
    return _broker;
}

MulticastRouter::MulticastRouter( void )
    : mRouterState  ( NEMulticastRouterSettings::RouterStopped )
    , mServiceCmd   ( MulticastRouter::CMD_Undefined )
    , mQuitEvent    ( false, false )
    , mServiceServer( )
    , mSvcHandle    ( NULL )
    , mSeMHandle    ( NULL )
{
    ; // do nothing
}

MulticastRouter::~MulticastRouter( void )
{
    _freeResources( );
}
