#ifndef AREG_TRACE_PRIVATE_IETRACONFIGURATION_HPP
#define AREG_TRACE_PRIVATE_IETRACONFIGURATION_HPP
/************************************************************************
 * \file        areg/trace/private/IETraceConfiguration.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Interface of trace configuration
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class TraceProperty;

//////////////////////////////////////////////////////////////////////////
// IETraceConfiguration class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Trace configuration is an interface of configuration container.
 *          The inter has methods to get values of configured properties.
 *          All properties are read only and cannot be modified
 **/
class IETraceConfiguration
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected default constructor.
     **/
    IETraceConfiguration( void );

    /**
     * \brief   Protected destructor.
     **/
    virtual ~IETraceConfiguration( void );

//////////////////////////////////////////////////////////////////////////
// Override attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IETraceConfiguration interface overrides
/************************************************************************/

    /**
     * \brief   Returns property value of logging version.
     **/
    virtual const TraceProperty & propertyVersion( void ) const = 0;

    /**
     * \brief   Returns property value of layout format of scope enter.
     **/
    virtual const TraceProperty & propertyLayoutEnter( void ) const = 0;

    /**
     * \brief   Returns property value of layout format form message text output.
     **/
    virtual const TraceProperty & propertyLayoutMessage( void ) const = 0;

    /**
     * \brief   Returns property value of layout format of scope exit.
     **/
    virtual const TraceProperty & propertyLayoutExit( void ) const = 0;

    /**
     * \brief   Returns property value of stack size.
     **/
    virtual const TraceProperty & propertyStackSize( void ) const = 0;

    /**
     * \brief   Returns property value of debug output settings.
     **/
    virtual const TraceProperty & propertyDebugOutput( void ) const = 0;

    /**
     * \brief   Returns property value of logging status (enabled / disabled).
     **/
    virtual const TraceProperty & propertyStatus( void ) const = 0;

    /**
     * \brief   Returns property value of append data status.
     **/
    virtual const TraceProperty & propertyAppendData( void ) const = 0;

    /**
     * \brief   Returns property value of file logging settings.
     **/
    virtual const TraceProperty & propertyLogFile( void ) const = 0;

    /**
     * \brief   Returns property value of remote host name logging settings.
     **/
    virtual const TraceProperty & propertyRemoteHost( void ) const = 0;

    /**
     * \brief   Returns property value of remote host port logging settings.
     **/
    virtual const TraceProperty & propertyRemotePort( void ) const = 0;

    /**
     * \brief   Returns property value of database host name logging settings.
     **/
    virtual const TraceProperty & propertyDatabaseHost( void ) const = 0;

    /**
     * \brief   Returns property value of database name logging settings.
     **/
    virtual const TraceProperty & propertyDatabaseName( void ) const = 0;

    /**
     * \brief   Returns property value of database driver name logging settings.
     **/
    virtual const TraceProperty & propertyDatabaseDriver( void ) const = 0;

    /**
     * \brief   Returns property value of database user name logging settings.
     **/
    virtual const TraceProperty & propertyDatabaseUser( void ) const = 0;

    /**
     * \brief   Returns property value of database user password logging settings.
     **/
    virtual const TraceProperty & propertyDatabasePassword( void ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IETraceConfiguration( const IETraceConfiguration & );
    const IETraceConfiguration & operator = ( const IETraceConfiguration & );
};

#endif  // AREG_TRACE_PRIVATE_IETRACONFIGURATION_HPP
