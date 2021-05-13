#ifndef AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTS_HPP
#define AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTS_HPP
/************************************************************************
 * \file        areg/component/private/ServiceManagerEvents.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Manager communication Events
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"
#include "areg/base/SharedBuffer.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class ProxyAddress;
class StubAddress;
class Channel;
class String;

//////////////////////////////////////////////////////////////////////////
// ServiceManagerEventData class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Service Manager event data object, which contains broker
 *          commands to execute and appropriate data for command
 **/
class ServiceManagerEventData
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The Service Manager event commands
     **/
    typedef enum E_ServiceManagerCommands
    {
          CMD_StopRoutingClient         //!< Requested to stop Service Manager client connection
        , CMD_RegisterProxy             //!< Requested to register Proxy
        , CMD_UnregisterProxy           //!< Requested to unregister Proxy
        , CMD_RegisterStub              //!< Requested to register Stub
        , CMD_UnregisterStub            //!< Requested to unregister Stub
        , CMD_ConfigureConnection       //!< Requested to configure connection
        , CMD_StartConnection           //!< Requested to start connection, the data is configuration file
        , CMD_StartNetConnection        //!< Requested to start network connection, the data is IP-address and port number
        , CMD_StopConnection            //!< Requested to stop connection
        , CMD_SetEnableService          //!< Requested to enables or disables remote service
        , CMD_RegisterConnection        //!< Requested to register connection
        , CMD_UnregisterConnection      //!< Requested to unregister connection
        , CMD_LostConnection            //!< Requested to send notifications to components that connection is lost.
    } eServiceManagerCommands;

    /**
     * \brief   Converts event command to string
     * \param   cmdBroker   The command to convert
     **/
    static const char * getString( ServiceManagerEventData::eServiceManagerCommands cmdBroker );

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates and returns Service Manager event data with command to stop Broker client connection
     **/
    static ServiceManagerEventData stopMessageRouterClient( void );

    /**
     * \brief   Creates and returns Service Manager event data with command to register Proxy
     **/
    static ServiceManagerEventData registerProxy( const ProxyAddress & addrProxy );

    /**
     * \brief   Creates and returns Service Manager event data with command to unregister Proxy
     **/
    static ServiceManagerEventData unregisterProxy( const ProxyAddress & addrProxy );

    /**
     * \brief   Creates and returns Service Manager event data with command to register Stub
     **/
    static ServiceManagerEventData registerStub( const StubAddress & addrStub );

    /**
     * \brief   Creates and returns Service Manager event data with command to unregister Stub
     **/
    static ServiceManagerEventData unregisterStub( const StubAddress & addrStub );

    /**
     * \brief   Creates and returns Service Manager event data with command to configure connection
     **/
    static ServiceManagerEventData configureConnection( const String & configFile );

    /**
     * \brief   Creates and returns Service Manager event data with command to start Broker client connection
     **/
    static ServiceManagerEventData startConnection( const String & configFile );

    /**
     * \brief   Creates and returns Service Manager event data with command to start Broker client connection.
     * \param   ipAddress   The IP-Address of remote Routing Service.
     * \param   portNr      The port number to connect.
     **/
    static ServiceManagerEventData startNetConnection( const String & ipAddress, unsigned short portNr );

    /**
     * \brief   Creates and returns Service Manager event data with command to stop connection
     **/
    static ServiceManagerEventData stopConnection( void );

    /**
     * \brief   Creates Service Manager event data with command to enable or disable remote service.
     *          If disabled, services cannot be accessed outside of process.
     *          If remote service is already running and it is requested to disable, it will be stopped.
     * \param  enable   If true, the remote services are accessed from outside of process.
     *                  If false, all services run and accessed only within process.
     * \return  Returns Service Manager Event data object with command to enable or disable remote service.
     **/
    static ServiceManagerEventData enableRemoteService( bool enable );

    /**
     * \brief   Creates and returns Service Manager event data with command to register connection
     * \param   channel     The Channel information to set for registered connection
     **/
    static ServiceManagerEventData registerConnection( const Channel & channel );

    /**
     * \brief   Creates and returns Service Manager event data with command to unregister connection
     * \param   channel     The Channel information to set for unregistered connection
     **/
    static ServiceManagerEventData unregisterConnection( const Channel & channel );

    /**
     * \brief   Creates and returns Service Manager event data with command of lost connection
     * \param   channel     The Channel information to set for lost connection
     **/
    static ServiceManagerEventData lostConnection( const Channel & channel );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructor. Sets Service Manager event command
     * \param   cmdServiceManager    The command to set
     **/
    inline ServiceManagerEventData( ServiceManagerEventData::eServiceManagerCommands cmdServiceManager );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    inline ServiceManagerEventData( const ServiceManagerEventData & source );

    /**
     * \brief   Destructor.
     **/
    inline ~ServiceManagerEventData( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies Service Manager event data with comment from given source.
     * \param   source  The source of data to copy
     **/
    inline void operator = ( const ServiceManagerEventData & source );

    /**
     * \brief   Returns streaming object to read data
     **/
    inline const IEInStream & getReadStream( void ) const;

    /**
     * \brief   Returns streaming object to write data
     **/
    inline IEOutStream & getWriteStream( void );

    /**
     * \brief   Returns Service Manager command
     **/
    inline ServiceManagerEventData::eServiceManagerCommands getCommand( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    eServiceManagerCommands mCommand;   //!< Service Manager event data command
    SharedBuffer            mStream;    //!< Service Manager event data streaming object to write and read data.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceManagerEventData( void );
};

//////////////////////////////////////////////////////////////////////////
// Service Manager event and consumer declaration
//////////////////////////////////////////////////////////////////////////
DECLARE_EVENT(ServiceManagerEventData, ServiceManagerEvent, IEServiceManagerEventConsumer)

//////////////////////////////////////////////////////////////////////////
// ServiceManagerEventData inline methods
//////////////////////////////////////////////////////////////////////////
inline ServiceManagerEventData::ServiceManagerEventData( ServiceManagerEventData::eServiceManagerCommands cmdServiceManager )
    : mCommand  ( cmdServiceManager )
    , mStream   ( )
{   ;   }

inline ServiceManagerEventData::ServiceManagerEventData( const ServiceManagerEventData & source )
    : mCommand  ( source.mCommand )
    , mStream   ( source.mStream )
{   ;   }

inline ServiceManagerEventData::~ServiceManagerEventData( void )
{   ;   }

inline void ServiceManagerEventData::operator = (const ServiceManagerEventData & source)
{
    mCommand= source.mCommand;
    mStream = source.mStream;
}

inline const IEInStream & ServiceManagerEventData::getReadStream( void ) const
{
    return static_cast<const IEInStream &>(mStream);
}

inline IEOutStream & ServiceManagerEventData::getWriteStream( void )
{
    return static_cast<IEOutStream &>(mStream);
}

inline ServiceManagerEventData::eServiceManagerCommands ServiceManagerEventData::getCommand( void ) const
{
    return mCommand;
}

inline const char * ServiceManagerEventData::getString( ServiceManagerEventData::eServiceManagerCommands cmdBroker )
{
    switch ( cmdBroker )
    {
    case ServiceManagerEventData::CMD_StopRoutingClient:
        return "ServiceManagerEventData::CMD_StopRoutingClient";
    case ServiceManagerEventData::CMD_RegisterProxy:
        return "ServiceManagerEventData::CMD_RegisterProxy";
    case ServiceManagerEventData::CMD_UnregisterProxy:
        return "ServiceManagerEventData::CMD_UnregisterProxy";
    case ServiceManagerEventData::CMD_RegisterStub:
        return "ServiceManagerEventData::CMD_RegisterStub";
    case ServiceManagerEventData::CMD_UnregisterStub:
        return "ServiceManagerEventData::CMD_UnregisterStub";
    case ServiceManagerEventData::CMD_ConfigureConnection:
        return "ServiceManagerEventData::CMD_ConfigureConnection";
    case ServiceManagerEventData::CMD_StartConnection:
        return "ServiceManagerEventData::CMD_StartConnection";
    case ServiceManagerEventData::CMD_StartNetConnection:
        return "ServiceManagerEventData::CMD_StartNetConnection";
    case ServiceManagerEventData::CMD_StopConnection:
        return "ServiceManagerEventData::CMD_StopConnection";
    case ServiceManagerEventData::CMD_SetEnableService:
        return "ServiceManagerEventData::CMD_SetEnableService";
    case ServiceManagerEventData::CMD_RegisterConnection:
        return "ServiceManagerEventData::CMD_RegisterConnection";
    case ServiceManagerEventData::CMD_UnregisterConnection:
        return "ServiceManagerEventData::CMD_UnregisterConnection";
    case ServiceManagerEventData::CMD_LostConnection:
        return "ServiceManagerEventData::CMD_LostConnection";
    default:
        return "ERR: undefined ServiceManagerEventData::eServiceManagerCommands value!!!";
    }
}

#endif  // AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTS_HPP
