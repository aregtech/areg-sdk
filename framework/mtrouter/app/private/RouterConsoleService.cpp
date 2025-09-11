/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mtrouter/app/private/RouterConsoleService.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing, service that outputs statistics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "mtrouter/app/private/RouterConsoleService.hpp"

#include "mtrouter/app/MultitargetRouter.hpp"

//////////////////////////////////////////////////////////////////////////
// RouterConsoleService class implementation
//////////////////////////////////////////////////////////////////////////
RouterConsoleService::RouterConsoleService( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : SystemServiceConsole( &MultitargetRouter::getInstance().getDataRateHelper(), entry, owner)
{
}
