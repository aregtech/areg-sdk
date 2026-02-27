#ifndef AREG_LOGGING_PRIVATE_LOGGERBASE_HPP
#define AREG_LOGGING_PRIVATE_LOGGERBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LoggerBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Logger base object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/logging/LoggingDefs.hpp"
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
     * \brief   Protected constructor. Initializes the logger and sets the provided log
     *          configuration object, which supplies methods for accessing property values.
     *
     * \param   logConfig       An instance of the log configuration object containing settings for
     *                          initialization and message output.
     **/
    explicit LoggerBase(LogConfiguration& logConfig);

    /**
     * \brief   Destructor
     **/
    virtual ~LoggerBase() = default;

//////////////////////////////////////////////////////////////////////////
// Override operations and attribute
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// LoggerBase interface overrides
/************************************************************************/

    /**
     * \brief   Initializes and opens the logger. If this method returns true, the log manager will
     *          start forwarding messages for logging. If it returns false, the log manager assumes
     *          the logger is not initialized and will not send messages for logging. The logger
     *          must be opened before any messages can be logged.
     *
     * \return  Returns true if the logger was successfully initialized and opened.
     **/
    virtual bool open_logger() = 0;

    /**
     * \brief   Closes the logger and stops logging.
     **/
    virtual void close_logger() = 0;

    /**
     * \brief   Called when message should be logged. Every logger should implement method to
     *          process logger specific logging.
     *
     * \param   log_message     The logging message to process.
     **/
    virtual void log_message( const NELogging::LogEntry & log_message ) = 0;

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    virtual bool is_logger_opened() const = 0;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Closes and reopens the logger. Returns true if the logger was successfully reopened.
     **/
    inline bool reopen_logger();

    /**
     * \brief   Returns the log configuration object.
     **/
    inline const LogConfiguration & log_configuration() const;

    /**
     * \brief   Returns the layout object to format and output messages.
     **/
    inline const LayoutManager & layout_message() const;

    /**
     * \brief   Returns the layout object to format "enter scope" messages.
     **/
    inline const LayoutManager & layout_enter_scope() const;

    /**
     * \brief   Returns the layout object to format "exit scope" messages.
     **/
    inline const LayoutManager & layout_exit_scope() const;

//////////////////////////////////////////////////////////////////////////
// Protected overrides
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates message layout objects and returns true if succeeded.
     **/
    virtual bool create_layouts();

    /**
     * \brief   Releases previously created layouts.
     **/
    virtual void release_layouts();

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
    LoggerBase() = delete;
    AREG_NOCOPY_NOMOVE( LoggerBase );
};

//////////////////////////////////////////////////////////////////////////
// LoggerBase class inline methods
//////////////////////////////////////////////////////////////////////////
inline bool LoggerBase::reopen_logger()
{
    close_logger();
    return open_logger();
}

inline const LogConfiguration & LoggerBase::log_configuration() const
{
    return mLogConfiguration;
}

inline const LayoutManager & LoggerBase::layout_message() const
{
    return mLayoutsMessage;
}

inline const LayoutManager & LoggerBase::layout_enter_scope() const
{
    return mLayoutsScopeEnter;
}

inline const LayoutManager & LoggerBase::layout_exit_scope() const
{
    return mLayoutsScopeExit;
}

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_LOGGERBASE_HPP
