#ifndef AREG_TRACE_PRIVATE_CELOGGERBASE_HPP
#define AREG_TRACE_PRIVATE_CELOGGERBASE_HPP
/************************************************************************
 * \file        areg/trace/private/CELoggerBase.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Logger base object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/NETrace.hpp"
#include "areg/trace/private/CELayoutManager.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IETraceConfiguration;

//////////////////////////////////////////////////////////////////////////
// CELoggerBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Logger base object. All logger objects should extend and
 *          implement methods. The interface gives possibility to open and
 *          close logger, to log messages and to reopen if settings have
 *          been changed. The object contains as well basic information and
 *          basic functionalities to create 3 layout manager objects, which
 *          are shared between loggers.
 **/
class CELoggerBase
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The format of logger begin ('hello' message) to display in console
     **/
    static const char * const  FOMAT_MESSAGE_HELLO      /*= "Starting logging of process [ %s ] with ID [ %d ]"*/;
    /**
     * \brief   The format of logger end ('bey' message) to display in console
     **/
    static const char * const  FORMAT_MESSAGE_BYE       /*= "Completed logging of process [ %s ] with ID [ %d ]"*/;

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
    CELoggerBase( IETraceConfiguration & tracerConfig );

public:
    /**
     * \brief   Destructor
     **/
    virtual ~CELoggerBase( void );

//////////////////////////////////////////////////////////////////////////
// Override operations and attribute
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// CELoggerBase interface overrides
/************************************************************************/

    /**
     * \brief   Called to initialize / open logger. If method returns true, 
     *          the trace manager starts to forward messages for logging.
     *          If method returns false, the tracer manager assumes that
     *          logger is not initialized and will not send messages for logging.
     *          Before any message is logger, the logger should be opened.
     * \return  Returns true if logger succeeded initialization (open).
     **/
    virtual bool OpenLogger( void ) = 0;

    /**
     * \brief   Called to close logger and stop logging.
     **/
    virtual void CloseLogger( void ) = 0;

    /**
     * \brief   Called when message should be logged.
     *          Every logger should implement method to process logger specific logging.
     **/
    virtual bool LogMessage( const NETrace::sLogMessage & logMessage ) = 0;

    /**
     * \brief   Call to flush logs, if they are queued. Some loggers might ignore this.
     **/
    virtual void FlushLogs( void ) = 0;

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    virtual bool IsLoggerOpened( void ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Called when logger should be reopened.
     *          If method returns true, the trace manager will forward messages
     *          to logger to make outputs. Otherwise, the logger will be stopped
     *          receiving log messages.
     **/
    bool ReopenLogger( void );

    /**
     * \brief   Return instance of trace configuration object.
     **/
    inline const IETraceConfiguration & GetTraceConfiguration( void ) const;

    /**
     * \return  Returns the layout object to output messages.
     **/
    inline const CELayoutManager & GetLayoutMessage( void ) const;

    /**
     * \return  Returns the layout object to output enter scope message.
     **/
    inline const CELayoutManager & GetLayoutEnterScope( void ) const;

    /**
     * \return  Returns the layout object to output exit scope message.
     **/
    inline const CELayoutManager & GetLayoutExitScope( void ) const;

//////////////////////////////////////////////////////////////////////////
// Protected overrides
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates message layout objects. Returns true if succeeded.
     *          Overwrite method to change layouts.
     **/
    virtual bool CreateLayouts( void );

    /**
     * \brief   Release previously crated layouts
     **/
    virtual void ReleaseLayouts( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of tracer configurations object.
     **/
    IETraceConfiguration &  mTracerConfiguration;
    /**
     * \brief   Message layouts to create messages
     **/
    CELayoutManager         mLayoutsMessage;
    /**
     * \brief   Message layouts to create "Enter scope" message
     **/
    CELayoutManager         mLayoutsScopeEnter;
    /**
     * \brief   Message layouts to create "Exit scope" message
     **/
    CELayoutManager         mLayoutsScopeExit;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    CELoggerBase( void );
    CELoggerBase( const CELoggerBase & );
    const CELoggerBase & operator = ( const CELoggerBase & );
};

//////////////////////////////////////////////////////////////////////////
// CELoggerBase class inline methods
//////////////////////////////////////////////////////////////////////////
inline const IETraceConfiguration & CELoggerBase::GetTraceConfiguration( void ) const
{   return mTracerConfiguration;    }

inline const CELayoutManager & CELoggerBase::GetLayoutMessage(void) const
{   return mLayoutsMessage;         }

inline const CELayoutManager & CELoggerBase::GetLayoutEnterScope(void) const
{   return mLayoutsScopeEnter;      }

inline const CELayoutManager & CELoggerBase::GetLayoutExitScope(void) const
{   return mLayoutsScopeExit;       }

#endif  // AREG_TRACE_PRIVATE_CELOGGERBASE_HPP
