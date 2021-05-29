/************************************************************************
 * \file        areg/trace/private/LoggerBase.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Logger interface
 ************************************************************************/
#include "areg/trace/private/LoggerBase.hpp"
#include "areg/trace/private/TraceManager.hpp"

const char * const  LoggerBase::FOMAT_MESSAGE_HELLO   = "Starting logging of [ %s ] process [ %s ] with ID [ %d ]\n";

const char * const  LoggerBase::FORMAT_MESSAGE_BYE    = "Completed logging of [ %s ] process [ %s ] with ID [ %d ]\n";

LoggerBase::LoggerBase( IETraceConfiguration & tracerConfig )
    : mTracerConfiguration  ( tracerConfig )
    , mLayoutsMessage       ( )
    , mLayoutsScopeEnter    ( )
    , mLayoutsScopeExit     ( )
{
    ; // do nothing
}


LoggerBase::~LoggerBase(void)
{
    ; // do nothing
}

bool LoggerBase::reopenLogger(void)
{
    closeLogger();
    return openLogger();
}

bool LoggerBase::createLayouts(void)
{
    bool result = false;
    const TraceProperty & propMessage = mTracerConfiguration.propertyLayoutMessage();
    const TraceProperty & propEnter   = mTracerConfiguration.propertyLayoutEnter();
    const TraceProperty & propExit    = mTracerConfiguration.propertyLayoutExit();

    if (propMessage.isValid() )
        result |= mLayoutsMessage.createLayouts( static_cast<const char *>(propMessage.getValue()) );
    if ( propEnter.isValid() )
        result |= mLayoutsScopeEnter.createLayouts( static_cast<const char *>(propEnter.getValue()) );
    if ( propExit.isValid() )
        result |= mLayoutsScopeExit.createLayouts( static_cast<const char *>(propExit.getValue()) );
    return result;
}

void LoggerBase::releaseLayouts(void)
{
    mLayoutsMessage.deleteLayouts();
    mLayoutsScopeEnter.deleteLayouts();
    mLayoutsScopeExit.deleteLayouts();
}
