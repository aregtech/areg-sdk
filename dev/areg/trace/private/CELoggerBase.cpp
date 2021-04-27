/************************************************************************
 * \file        areg/trace/private/CELoggerBase.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Logger interface
 ************************************************************************/
#include "areg/trace/private/CELoggerBase.hpp"
#include "areg/trace/private/CETraceManager.hpp"

const char * const  CELoggerBase::FOMAT_MESSAGE_HELLO   = "Starting logging of process [ %s ] with ID [ %d ]\n";

const char * const  CELoggerBase::FORMAT_MESSAGE_BYE    = "Completed logging of process [ %s ] with ID [ %d ]\n";

CELoggerBase::CELoggerBase( IETraceConfiguration & tracerConfig )
    : mTracerConfiguration  ( tracerConfig )
    , mLayoutsMessage       ( )
    , mLayoutsScopeEnter    ( )
    , mLayoutsScopeExit     ( )
{
    ; // do nothing
}


CELoggerBase::~CELoggerBase(void)
{
    ; // do nothing
}

bool CELoggerBase::ReopenLogger(void)
{
    CloseLogger();
    return OpenLogger();
}

bool CELoggerBase::CreateLayouts(void)
{
    bool result = false;
    const CETraceProperty & propMessage = mTracerConfiguration.PropertyLayoutMessage();
    const CETraceProperty & propEnter   = mTracerConfiguration.PropertyLayoutEnter();
    const CETraceProperty & propExit    = mTracerConfiguration.PropertyLayoutExit();

    if (propMessage.IsValid() )
        result |= mLayoutsMessage.CreateLayouts( static_cast<const char *>(propMessage.GetValue()) );
    if ( propEnter.IsValid() )
        result |= mLayoutsScopeEnter.CreateLayouts( static_cast<const char *>(propEnter.GetValue()) );
    if ( propExit.IsValid() )
        result |= mLayoutsScopeExit.CreateLayouts( static_cast<const char *>(propExit.GetValue()) );
    return result;
}

void CELoggerBase::ReleaseLayouts(void)
{
    mLayoutsMessage.DeleteLayouts();
    mLayoutsScopeEnter.DeleteLayouts();
    mLayoutsScopeExit.DeleteLayouts();
}
