#ifndef AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTS_HPP
#define AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceManagerEvents.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Manager communication Events
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/SharedBuffer.hpp"
#include "areg/component/NEService.hpp"
#include "areg/component/TEEvent.hpp"
#include "areg/ipc/NERemoteService.hpp"

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
 * \brief   Service Manager event data object, which contains routing connection
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
    typedef enum class E_ServiceManagerCommands
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
        , CMD_RegisterConnection        //!< Requested to register connection
        , CMD_UnregisterConnection      //!< Requested to unregister connection
        , CMD_LostConnection            //!< Requested to send notifications to components that connection is lost.
        , CMD_ShutdownService           //!< Request to shutdown service.
        , CMD_TerminateComponentThread  //!< Request to terminate component thread.
        , CMD_StartComponentThread      //!< Request to start component thread.
    } eServiceManagerCommands;

    /**
     * \brief   Converts event command to string
     * \param   val     The command to convert to string
     **/
    static const char * getString( ServiceManagerEventData::eServiceManagerCommands val );

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates and returns Service Manager event data with command to stop router client connection.
     **/
    static ServiceManagerEventData stopMessageRouterClient( void );

    /**
     * \brief   creates and returns service manager event with command to stop routing connection and stop service manager.
     **/
    static ServiceManagerEventData shutdownServiceManager( void );

    /**
     * \brief   Creates and returns Service Manager event data with command to register Proxy
     **/
    static ServiceManagerEventData registerProxy( const ProxyAddress & addrProxy );

    /**
     * \brief   Creates and returns Service Manager event data with command to unregister Proxy
     * \param   addrProxy   The address of the service consumer to unregister.
     * \param   reason      The reason to unregister or disconnect the service consumer.
     **/
    static ServiceManagerEventData unregisterProxy( const ProxyAddress & addrProxy, NEService::eDisconnectReason reason );

    /**
     * \brief   Creates and returns Service Manager event data with command to register Stub
     **/
    static ServiceManagerEventData registerStub( const StubAddress & addrStub );

    /**
     * \brief   Creates and returns Service Manager event data with command to unregister Stub
     * \param   addrStub    The address of the service provider to unregister / disconnect.
     * \param   reason      The reason to unregister or disconnect the service provider.
     **/
    static ServiceManagerEventData unregisterStub( const StubAddress & addrStub, NEService::eDisconnectReason reason );

    /**
     * \brief   Creates and returns Service Manager event data with command to configure connection.
     * \param   service         The remote service, which connection should be configured.
     * \param   connectTypes    The bitwise set of connections to configure.
     * \return  Returns  event data to send to the target.
     **/
    static ServiceManagerEventData configureConnection(NERemoteService::eRemoteServices service, unsigned int connectTypes );

    /**
     * \brief   Creates and returns Service Manager event data with command to start router client connection
     * \param   service         The remote service, which connection should be started.
     * \param   connectTypes    The bitwise set of connections to start.
     * \return  Returns  event data to send to the target.
     **/
    static ServiceManagerEventData startConnection(NERemoteService::eRemoteServices service, unsigned int connectTypes);

    /**
     * \brief   Creates and returns Service Manager event data with command to start router client connection.
     * \param   ipAddress   The IP-Address of remote Routing Service.
     * \param   portNr      The port number to connect.
     **/
    static ServiceManagerEventData startNetConnection( const String & ipAddress, unsigned short portNr );

    /**
     * \brief   Creates and returns Service Manager event data with command to stop connection
     **/
    static ServiceManagerEventData stopConnection( void );

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

    /**
     * \brief   Creates an event data to terminate component thread.
     * \param   threadName  The name of component thread to terminate.
     **/
    static ServiceManagerEventData terminateComponentThread( const String & threadName );

    /**
     * \brief   Creates an event data to create component thread.
     * \param   threadName  The name of component thread to create.
     **/
    static ServiceManagerEventData createComponentThread( const String & threadName );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructor. Sets Service Manager event command
     * \param   cmdServiceManager    The command to set
     **/
    inline explicit ServiceManagerEventData( ServiceManagerEventData::eServiceManagerCommands cmdServiceManager );

    /**
     * \brief   Copy constructor.
     * \param   source  The source of data to copy.
     **/
    inline ServiceManagerEventData( const ServiceManagerEventData & source );

    /**
     * \brief   Move constructor.
     * \param   source  The source of data to move.
     **/
    inline ServiceManagerEventData( ServiceManagerEventData && source ) noexcept;

    /**
     * \brief   Destructor.
     **/
    ~ServiceManagerEventData( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies Service Manager event data with comment from given source.
     * \param   source  The source of data to copy
     **/
    inline ServiceManagerEventData & operator = ( const ServiceManagerEventData & source );

    /**
     * \brief   Moves Service Manager event data with comment from given source.
     * \param   source  The source of data to move.
     **/
    inline ServiceManagerEventData & operator = ( ServiceManagerEventData && source ) noexcept;

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
    ServiceManagerEventData( void ) = delete;
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
{
}

inline ServiceManagerEventData::ServiceManagerEventData( const ServiceManagerEventData & source )
    : mCommand  ( source.mCommand )
    , mStream   ( source.mStream )
{
}

inline ServiceManagerEventData::ServiceManagerEventData( ServiceManagerEventData && source ) noexcept
    : mCommand  ( std::move(source.mCommand) )
    , mStream   ( std::move(source.mStream) )
{
}

inline ServiceManagerEventData & ServiceManagerEventData::operator = (const ServiceManagerEventData & source)
{
    mCommand= source.mCommand;
    mStream = source.mStream;

    return (*this);
}

inline ServiceManagerEventData & ServiceManagerEventData::operator = ( ServiceManagerEventData && source ) noexcept
{
    mCommand= source.mCommand;
    mStream = std::move(source.mStream);

    return (*this);
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

inline const char * ServiceManagerEventData::getString( ServiceManagerEventData::eServiceManagerCommands val )
{
    switch ( val )
    {
    case ServiceManagerEventData::eServiceManagerCommands::CMD_StopRoutingClient:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_StopRoutingClient";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterProxy:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterProxy";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterProxy:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterProxy";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterStub:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterStub";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterStub:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterStub";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_ConfigureConnection:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_ConfigureConnection";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_StartConnection:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_StartConnection";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_StartNetConnection:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_StartNetConnection";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_StopConnection:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_StopConnection";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterConnection:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterConnection";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterConnection:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterConnection";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_LostConnection:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_LostConnection";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_ShutdownService:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_ShutdownService";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_TerminateComponentThread:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_TerminateComponentThread";
    case ServiceManagerEventData::eServiceManagerCommands::CMD_StartComponentThread:
        return "ServiceManagerEventData::eServiceManagerCommands::CMD_StartComponentThread";
    default:
        return "ERR: undefined ServiceManagerEventData::eServiceManagerCommands value!!!";
    }
}

#endif  // AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTS_HPP
