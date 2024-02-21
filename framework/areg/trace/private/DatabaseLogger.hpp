#ifndef AREG_TRACE_PRIVATE_DATABASELOGGER_HPP
#define AREG_TRACE_PRIVATE_DATABASELOGGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/DatabaseLogger.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, File Logger object to log message into the file
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/private/LoggerBase.hpp"
#include "areg/base/SynchObjects.hpp"

#if AREG_LOGS

/************************************************************************
 * Dependencies.
 ************************************************************************/
class IELogDatabaseEngine;

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
     * \brief   Instantiates database logger object and sets tracer configuration object,
     *          which contains methods to get property values after configuring tracer.
     *          The constructor does not instantiate the database engine. Instead, it should
     *          be manually set.
     * \param   tracerConfig    The instance of tracer configuration object, which contains configuration values,
     *                          required by logger during initialization (open) and when outputs message.
     **/
    explicit DatabaseLogger(LogConfiguration & tracerConfig);

    virtual ~DatabaseLogger(void);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the pointer to the database engine handler object,
     *          which is responsible to handle the database.
     **/
    inline const IELogDatabaseEngine * getDatabaseEngine(void) const;
    inline IELogDatabaseEngine * getDatabaseEngine(void);

    /**
     * \brief   Call to set the logging database engine object.
     *          If nullptr, no data is logged in the database.
     *          Otherwise, the log messages are forwarded to the 
     *          database engine for further processing.
     * \param   dbEngine    The pointer to the log database engine to save log messages.
     *                      If nullptr, no message is forwarded to the database engine.
     **/
    inline void setDatabaseEngine(IELogDatabaseEngine * dbEngine);

    /**
     * \brief   Returns true if the logging database engine is nut null.
     **/
    inline bool isValid(void) const;

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
    virtual bool openLogger( void ) override;

    /**
     * \brief   Called to close logger and stop logging.
     **/
    virtual void closeLogger( void ) override;

    /**
     * \brief   Called when message should be logged.
     *          Every logger should implement method to process logger specific logging.
     **/
    virtual void logMessage( const NETrace::sLogMessage & logMessage ) override;

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    virtual bool isLoggerOpened( void ) const override;

public:
    /**
     * \brief   Call to flush logs, if they are queued. Some loggers might ignore this.
     **/
    void flushLogs(void);

//////////////////////////////////////////////////////////////////////////
// Protected overrides
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates message layout objects. Returns true if succeeded.
     *          Overwrite method to change layouts.
     **/
    virtual bool createLayouts( void ) override;

    /**
     * \brief   Release previously crated layouts
     **/
    virtual void releaseLayouts( void ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    //!< The pointer to the database engine.
    IELogDatabaseEngine *   mDatabase;

    //!< Locking object.
    mutable Mutex           mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DatabaseLogger(void) = delete;
    DECLARE_NOCOPY_NOMOVE(DatabaseLogger);
};

//////////////////////////////////////////////////////////////////////////
// DatabaseLogger class inline methods.
//////////////////////////////////////////////////////////////////////////

inline const IELogDatabaseEngine * DatabaseLogger::getDatabaseEngine(void) const
{
    Lock lock(mLock);
    return mDatabase;
}

inline IELogDatabaseEngine * DatabaseLogger::getDatabaseEngine(void)
{
    Lock lock(mLock);
    return mDatabase;
}

inline void DatabaseLogger::setDatabaseEngine(IELogDatabaseEngine * dbEngine)
{
    Lock lock(mLock);
    mDatabase = dbEngine;
}

inline bool DatabaseLogger::isValid(void) const
{
    Lock lock(mLock);
    return (mDatabase != nullptr);
}

#endif  // AREG_LOGS
#endif  // AREG_TRACE_PRIVATE_DATABASELOGGER_HPP
