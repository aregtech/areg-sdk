#ifndef AREG_TRACE_PRIVATE_IETRACECONFIGURATION_HPP
#define AREG_TRACE_PRIVATE_IETRACECONFIGURATION_HPP
/************************************************************************
 * \file        areg/src/trace/private/IETraceConfiguration.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Interface of trace configuration
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CETraceProperty;

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
    virtual const CETraceProperty & PropertyVersion( void ) const = 0;

    /**
     * \brief   Returns property value of layout format of scope enter.
     **/
    virtual const CETraceProperty & PropertyLayoutEnter( void ) const = 0;

    /**
     * \brief   Returns property value of layout format form message text output.
     **/
    virtual const CETraceProperty & PropertyLayoutMessage( void ) const = 0;

    /**
     * \brief   Returns property value of layout format of scope exit.
     **/
    virtual const CETraceProperty & PropertyLayoutExit( void ) const = 0;

    /**
     * \brief   Returns property value of stack size.
     **/
    virtual const CETraceProperty & PropertyStackSize( void ) const = 0;

    /**
     * \brief   Returns property value of debug output settings.
     **/
    virtual const CETraceProperty & PropertyDebugOutput( void ) const = 0;

    /**
     * \brief   Returns property value of logging status (enabled / disabled).
     **/
    virtual const CETraceProperty & PropertyStatus( void ) const = 0;

    /**
     * \brief   Returns property value of append data status.
     **/
    virtual const CETraceProperty & PropertyAppendData( void ) const = 0;

    /**
     * \brief   Returns property value of file logging settings.
     **/
    virtual const CETraceProperty & PropertyLogFile( void ) const = 0;

    /**
     * \brief   Returns property value of remote host name logging settings.
     **/
    virtual const CETraceProperty & PropertyRemoteHost( void ) const = 0;

    /**
     * \brief   Returns property value of remote host port logging settings.
     **/
    virtual const CETraceProperty & PropertyRemotePort( void ) const = 0;

    /**
     * \brief   Returns property value of database host name logging settings.
     **/
    virtual const CETraceProperty & PropertyDatabaseHost( void ) const = 0;

    /**
     * \brief   Returns property value of database name logging settings.
     **/
    virtual const CETraceProperty & PropertyDatabaseName( void ) const = 0;

    /**
     * \brief   Returns property value of database driver name logging settings.
     **/
    virtual const CETraceProperty & PropertyDatabaseDriver( void ) const = 0;

    /**
     * \brief   Returns property value of database user name logging settings.
     **/
    virtual const CETraceProperty & PropertyDatabaseUser( void ) const = 0;

    /**
     * \brief   Returns property value of database user password logging settings.
     **/
    virtual const CETraceProperty & PropertyDatabasePassword( void ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IETraceConfiguration( const IETraceConfiguration & );
    const IETraceConfiguration & operator = ( const IETraceConfiguration & );
};

#endif  // AREG_TRACE_PRIVATE_IETRACECONFIGURATION_HPP
