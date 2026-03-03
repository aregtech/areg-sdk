#ifndef AREG_LOGGING_PRIVATE_DATABASELOGGER_HPP
#define AREG_LOGGING_PRIVATE_DATABASELOGGER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/DatabaseLogger.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, File Logger object to log message into the file
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/logging/private/LoggerBase.hpp"

#include "areg/base/SyncPrimitives.hpp"

#if AREG_LOGGING

/************************************************************************
 * Dependencies.
 ************************************************************************/
namespace areg {
    class LogDatabaseEngine;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// DatabaseLogger class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The database logging class, which is responsible to log messages
 *          into the database. The database engine, which is responsible to handle
 *          the database, should be manually set. This class is forwarding
 *          the log messages to database engine handle for further processing.
 **/
class DatabaseLogger : public LoggerBase
{
public:
    /**
     * \brief   Initializes the logger and sets the provided log configuration object, which
     *          supplies methods for accessing property values. The constructor will not
     *          automatically start database engine, it should be manually started.
     *
     * \param   logConfig       An instance of the log configuration object containing settings for
     *                          initialization and message output.
     **/
    explicit DatabaseLogger(LogConfiguration & logConfig);

    virtual ~DatabaseLogger();

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the pointer to the database engine handler object, which is responsible to
     *          handle the database.
     **/
    inline const LogDatabaseEngine * database_engine() const;
    /**
     * \brief   Returns the pointer to the database engine handler object, which is responsible to
     *          handle the database.
     * \note    Non-const overload. Allows modification of the database engine.
     **/
    inline LogDatabaseEngine * database_engine();

    /**
     * \brief   Call to set the logging database engine object. If nullptr, no data is logged in the
     *          database. Otherwise, the log messages are forwarded to the database engine for
     *          further processing.
     *
     * \param   dbEngine    The pointer to the log database engine to save log messages. If nullptr,
     *                      no message is forwarded to the database engine.
     **/
    inline void set_database_engine(LogDatabaseEngine * dbEngine);

    /**
     * \brief   Returns true if the logging database engine is not null.
     **/
    inline bool is_valid() const;

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
    bool open_logger() override;

    /**
     * \brief   Closes the logger and stops logging.
     **/
    void close_logger() override;

    /**
     * \brief   Called when message should be logged. Every logger should implement method to
     *          process logger specific logging.
     *
     * \param   logMessage     The logging message to process.
     **/
    void log_message( const areg::LogEntry & logMessage) override;

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    bool is_logger_opened() const override;

public:
    /**
     * \brief   Flushes queued logs. Some loggers might ignore this.
     **/
    void flush_logs();

//////////////////////////////////////////////////////////////////////////
// Protected overrides
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates message layout objects and returns true if succeeded.
     **/
    bool create_layouts() override;

    /**
     * \brief   Releases previously created layouts.
     **/
    void release_layouts() override;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    //!< The pointer to the database engine.
    LogDatabaseEngine *   mDatabase;

    //!< Locking object.
    mutable Mutex           mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DatabaseLogger() = delete;
    AREG_NOCOPY_NOMOVE(DatabaseLogger);
};

//////////////////////////////////////////////////////////////////////////
// DatabaseLogger class inline methods.
//////////////////////////////////////////////////////////////////////////

inline const LogDatabaseEngine * DatabaseLogger::database_engine() const
{
    Lock lock(mLock);
    return mDatabase;
}

inline LogDatabaseEngine * DatabaseLogger::database_engine()
{
    Lock lock(mLock);
    return mDatabase;
}

inline void DatabaseLogger::set_database_engine(LogDatabaseEngine * dbEngine)
{
    Lock lock(mLock);
    mDatabase = dbEngine;
}

inline bool DatabaseLogger::is_valid() const
{
    Lock lock(mLock);
    return (mDatabase != nullptr);
}

} // namespace areg

#endif  // AREG_LOGGING
#endif  // AREG_LOGGING_PRIVATE_DATABASELOGGER_HPP
