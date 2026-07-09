/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/logging/private/LoggerBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Logger interface
 ************************************************************************/
#include "areg/logging/private/LoggerBase.hpp"
#include "areg/logging/private/LogManager.hpp"

#if AREG_LOGGING

namespace areg {

LoggerBase::LoggerBase( LogConfiguration & logConfig)
    : mLogConfiguration (logConfig)
    , mLayoutsMessage   ( )
    , mLayoutsScopeEnter( )
    , mLayoutsScopeExit ( )
{
}

bool LoggerBase::create_layouts()
{
    bool result = mLayoutsMessage.create_layouts(mLogConfiguration.layout_message());
    result |= mLayoutsScopeEnter.create_layouts(mLogConfiguration.layout_enter());
    result |= mLayoutsScopeExit.create_layouts(mLogConfiguration.layout_exit());

    return result;
}

void LoggerBase::release_layouts()
{
    mLayoutsMessage.delete_layouts();
    mLayoutsScopeEnter.delete_layouts();
    mLayoutsScopeExit.delete_layouts();
}

} // namespace areg

#endif  // AREG_LOGGING
