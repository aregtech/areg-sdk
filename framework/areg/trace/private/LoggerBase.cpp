/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LoggerBase.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Logger interface
 ************************************************************************/
#include "areg/trace/private/LoggerBase.hpp"
#include "areg/trace/private/TraceManager.hpp"

LoggerBase::LoggerBase( LogConfiguration & tracerConfig )
    : mLogConfiguration ( tracerConfig )
    , mLayoutsMessage   ( )
    , mLayoutsScopeEnter( )
    , mLayoutsScopeExit ( )
{
}

bool LoggerBase::createLayouts(void)
{
    bool result = mLayoutsMessage.createLayouts(mLogConfiguration.getLayoutMessage());
    result |= mLayoutsScopeEnter.createLayouts(mLogConfiguration.getLayoutEnter());
    result |= mLayoutsScopeExit.createLayouts(mLogConfiguration.getLayoutExit());

    return result;
}

void LoggerBase::releaseLayouts(void)
{
    mLayoutsMessage.deleteLayouts();
    mLayoutsScopeEnter.deleteLayouts();
    mLayoutsScopeExit.deleteLayouts();
}
