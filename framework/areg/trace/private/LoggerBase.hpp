#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LoggerBase.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Logger base object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/NETrace.hpp"
#include "areg/trace/private/LayoutManager.hpp"

#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
class LogConfiguration;

//////////////////////////////////////////////////////////////////////////
// LoggerBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Logger base object. All logger objects should extend and
 *          implement methods. The interface gives possibility to open and
 *          close logger, to log messages and to reopen if settings have
 *          been changed. The object contains as well basic information and
 *          basic functionalities to create 3 layout manager objects, which
 *          are shared between loggers.
 **/
class LoggerBase
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The format of logger begin ('hello' message) to display in console
     **/
    static constexpr std::string_view   FOMAT_MESSAGE_HELLO     { "Starting logging of [ %s ] process [ %s ] with ID [ %d ]\n" };
    /**
     * \brief   The format of logger end ('bey' message) to display in console
     **/
    static constexpr std::string_view   FORMAT_MESSAGE_BYE      { "Completed logging of [ %s ] process [ %s ] with ID [ %d ]\n" };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructor. Protected
     *          Instantiates logger and sets tracer configuration object,
     *          which contains methods to get property values after
     *          configuring tracer.
     * \param   tracerConfig    The instance tracer configuration object,
     *                          which contains configuration values,
     *                          required by logger during initialization (open)
     *                          and when outputs message.
     **/
    LoggerBase( LogConfiguration & tracerConfig );

    /**
     * \brief   Destructor
     **/
    virtual ~LoggerBase( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Override operations and attribute
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// LoggerBase interface overrides
/************************************************************************/

    /**
     * \brief   Called to initialize / open logger. If method returns true, 
     *          the trace manager starts to forward messages for logging.
     *          If method returns false, the tracer manager assumes that
     *          logger is not initialized and will not send messages for logging.
     *          Before any message is logger, the logger should be opened.
     * \return  Returns true if logger succeeded initialization (open).
     **/
    virtual bool openLogger( void ) = 0;

    /**
     * \brief   Called to close logger and stop logging.
     **/
    virtual void closeLogger( void ) = 0;

    /**
     * \brief   Called when message should be logged.
     *          Every logger should implement method to process logger specific logging.
     **/
    virtual bool logMessage( const NETrace::sLogMessage & logMessage ) = 0;

    /**
     * \brief   Call to flush logs, if they are queued. Some loggers might ignore this.
     **/
    virtual void flushLogs( void ) = 0;

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    virtual bool isLoggerOpened( void ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Called when logger should be reopened.
     *          If method returns true, the trace manager will forward messages
     *          to logger to make outputs. Otherwise, the logger will be stopped
     *          receiving log messages.
     **/
    bool reopenLogger( void );

    /**
     * \brief   Return instance of trace configuration object.
     **/
    inline const LogConfiguration & getTraceConfiguration( void ) const;

    /**
     * \return  Returns the layout object to output messages.
     **/
    inline const LayoutManager & getLayoutMessage( void ) const;

    /**
     * \return  Returns the layout object to output enter scope message.
     **/
    inline const LayoutManager & getLayoutEnterScope( void ) const;

    /**
     * \return  Returns the layout object to output exit scope message.
     **/
    inline const LayoutManager & getLayoutExitScope( void ) const;

//////////////////////////////////////////////////////////////////////////
// Protected overrides
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates message layout objects. Returns true if succeeded.
     *          Overwrite method to change layouts.
     **/
    virtual bool createLayouts( void );

    /**
     * \brief   Release previously crated layouts
     **/
    virtual void releaseLayouts( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of tracer configurations object.
     **/
    LogConfiguration &  mTracerConfiguration;
    /**
     * \brief   Message layouts to create messages
     **/
    LayoutManager       mLayoutsMessage;
    /**
     * \brief   Message layouts to create "Enter scope" message
     **/
    LayoutManager       mLayoutsScopeEnter;
    /**
     * \brief   Message layouts to create "Exit scope" message
     **/
    LayoutManager       mLayoutsScopeExit;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    LoggerBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LoggerBase );
};

//////////////////////////////////////////////////////////////////////////
// LoggerBase class inline methods
//////////////////////////////////////////////////////////////////////////
inline const LogConfiguration & LoggerBase::getTraceConfiguration( void ) const
{
    return mTracerConfiguration;
}

inline const LayoutManager & LoggerBase::getLayoutMessage(void) const
{
    return mLayoutsMessage;
}

inline const LayoutManager & LoggerBase::getLayoutEnterScope(void) const
{
    return mLayoutsScopeEnter;
}

inline const LayoutManager & LoggerBase::getLayoutExitScope(void) const
{
    return mLayoutsScopeExit;
}
