#ifndef AREG_LOGGING_PRIVATE_LOGGERBASE_HPP
#define AREG_LOGGING_PRIVATE_LOGGERBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LoggerBase.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Logger base object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/logging/NELogging.hpp"
#include "areg/logging/private/LayoutManager.hpp"

#include <string_view>

#if AREG_LOGS
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
     * \brief   The format of logger end ('bye' message) to display in console
     **/
    static constexpr std::string_view   FORMAT_MESSAGE_BYE      { "Completed logging of [ %s ] process [ %s ] with ID [ %d ]\n" };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor.
     *          Initializes the logger and sets the provided log configuration object,
     *          which supplies methods for accessing property values.
     * \param   logConfig   An instance of the log configuration object containing
     *                      settings for initialization and message output.
     **/
    explicit LoggerBase(LogConfiguration& logConfig);

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
     * \brief   Initializes and opens the logger. If this method returns true,
     *          the log manager will start forwarding messages for logging.
     *          If it returns false, the log manager assumes the logger is not
     *          initialized and will not send messages for logging.
     *          The logger must be opened before any messages can be logged.
     * \return  Returns true if the logger was successfully initialized and opened.
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
    virtual void logMessage( const NELogging::sLogMessage & logMessage ) = 0;

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    virtual bool isLoggerOpened( void ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Called when logger object(s) should be reopened.
     *          If method returns true, the log manager forwards messages to logger object(s).
     **/
    inline bool reopenLogger( void );

    /**
     * \brief   Return instance of log configuration object.
     **/
    inline const LogConfiguration & getLogConfiguration( void ) const;

    /**
     * \return  Returns the layout object to print messages.
     **/
    inline const LayoutManager & getLayoutMessage( void ) const;

    /**
     * \return  Returns the layout object to print "enter scope" message.
     **/
    inline const LayoutManager & getLayoutEnterScope( void ) const;

    /**
     * \return  Returns the layout object to output "exit scope" message.
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
protected:
    /**
     * \brief   The instance of log configurations object.
     **/
    LogConfiguration &  mLogConfiguration;

private:
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
inline bool LoggerBase::reopenLogger(void)
{
    closeLogger();
    return openLogger();
}

inline const LogConfiguration & LoggerBase::getLogConfiguration( void ) const
{
    return mLogConfiguration;
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

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_LOGGERBASE_HPP
