/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LoggerBase.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Logger interface
 ************************************************************************/
#include "areg/trace/private/LoggerBase.hpp"
#include "areg/trace/private/TraceManager.hpp"

LoggerBase::LoggerBase( LogConfiguration & tracerConfig )
    : mTracerConfiguration  ( tracerConfig )
    , mLayoutsMessage       ( )
    , mLayoutsScopeEnter    ( )
    , mLayoutsScopeExit     ( )
{
}

bool LoggerBase::createLayouts(void)
{
    bool result = false;
    const TraceProperty & propMessage = mTracerConfiguration.getLayoutMessage();
    const TraceProperty & propEnter   = mTracerConfiguration.getLayoutEnter();
    const TraceProperty & propExit    = mTracerConfiguration.getLayoutExit();

    if (propMessage.isValid() )
    {
        result |= mLayoutsMessage.createLayouts( static_cast<const String&>(propMessage.getValue( )) );
    }

    if ( propEnter.isValid() )
    {
        result |= mLayoutsScopeEnter.createLayouts( static_cast<const String&>(propEnter.getValue( )) );
    }

    if ( propExit.isValid() )
    {
        result |= mLayoutsScopeExit.createLayouts( static_cast<const String&>(propExit.getValue( )) );
    }

    return result;
}

void LoggerBase::releaseLayouts(void)
{
    mLayoutsMessage.deleteLayouts();
    mLayoutsScopeEnter.deleteLayouts();
    mLayoutsScopeExit.deleteLayouts();
}
