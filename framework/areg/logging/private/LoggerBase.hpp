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
namespace areg { class LogConfiguration; }

namespace areg
{
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
        explicit LoggerBase(areg::LogConfiguration& logConfig);

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
         * \brief   Initializes and opens the logger. If this method returns true,
         *          the log manager will start forwarding messages for logging.
         *          If it returns false, the log manager assumes the logger is not
         *          initialized and will not send messages for logging.
         *          The logger must be opened before any messages can be logged.
         * \return  Returns true if the logger was successfully initialized and opened.
         **/
        virtual bool openLogger() = 0;

        /**
         * \brief   Called to close logger and stop logging.
         **/
        virtual void closeLogger() = 0;

        /**
         * \brief   Called when message should be logged.
         *          Every logger should implement method to process logger specific logging.
         **/
        virtual void logMessage( const areg::LogEntry & logMessage ) = 0;

        /**
         * \brief   Returns true if logger is initialized (opened).
         **/
        virtual bool isLoggerOpened() const = 0;

    //////////////////////////////////////////////////////////////////////////
    // Operations and attributes
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Called when logger object(s) should be reopened.
         *          If method returns true, the log manager forwards messages to logger object(s).
         **/
        inline bool reopenLogger();

        /**
         * \brief   Return instance of log configuration object.
         **/
        inline const areg::LogConfiguration & getLogConfiguration() const;

        /**
         * \return  Returns the layout object to print messages.
         **/
        inline const areg::LayoutManager & getLayoutMessage() const;

        /**
         * \return  Returns the layout object to print "enter scope" message.
         **/
        inline const areg::LayoutManager & getLayoutEnterScope() const;

        /**
         * \return  Returns the layout object to output "exit scope" message.
         **/
        inline const areg::LayoutManager & getLayoutExitScope() const;

    //////////////////////////////////////////////////////////////////////////
    // Protected overrides
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Creates message layout objects. Returns true if succeeded.
         *          Overwrite method to change layouts.
         **/
        virtual bool createLayouts();

        /**
         * \brief   Release previously crated layouts
         **/
        virtual void releaseLayouts();

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   The instance of log configurations object.
         **/
        areg::LogConfiguration &  mLogConfiguration;

    private:
        /**
         * \brief   Message layouts to create messages
         **/
        areg::LayoutManager       mLayoutsMessage;
        /**
         * \brief   Message layouts to create "Enter scope" message
         **/
        areg::LayoutManager       mLayoutsScopeEnter;
        /**
         * \brief   Message layouts to create "Exit scope" message
         **/
        areg::LayoutManager       mLayoutsScopeExit;

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
    inline bool LoggerBase::reopenLogger()
    {
        closeLogger();
        return openLogger();
    }

    inline const areg::LogConfiguration & LoggerBase::getLogConfiguration() const
    {
        return mLogConfiguration;
    }

    inline const areg::LayoutManager & LoggerBase::getLayoutMessage() const
    {
        return mLayoutsMessage;
    }

    inline const areg::LayoutManager & LoggerBase::getLayoutEnterScope() const
    {
        return mLayoutsScopeEnter;
    }

    inline const areg::LayoutManager & LoggerBase::getLayoutExitScope() const
    {
        return mLayoutsScopeExit;
    }

    #endif  // AREG_LOGS
} // namespace areg
#endif  // AREG_LOGGING_PRIVATE_LOGGERBASE_HPP
