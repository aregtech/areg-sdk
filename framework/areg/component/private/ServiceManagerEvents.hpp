#ifndef AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTS_HPP
#define AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/ServiceManagerEvents.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Manager communication Events
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SharedBuffer.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/component/EventTemplate.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
namespace areg {

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
 * \brief   Encapsulates service manager event data with routing connection commands and associated
 *          command-specific data.
 **/
class AREG_API ServiceManagerEventData
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The Service Manager event commands
     **/
    enum class ServiceManagerCommand    : uint16_t
    {
          CMD_StopRoutingClient         //!< Requested to stop Service Manager client connection
        , CMD_RegisterProxy             //!< Requested to register Proxy
        , CMD_UnregisterProxy           //!< Requested to unregister Proxy
        , CMD_RegisterProvider          //!< Requested to register Provider
        , CMD_UnregisterProvider        //!< Requested to unregister Provider
        , CMD_ConfigureConnection       //!< Requested to configure connection
        , CMD_StartConnection           //!< Requested to start connection, the data is configuration file
        , CMD_StartNetConnection        //!< Requested to start network connection, the data is IP-address and port number
        , CMD_StopConnection            //!< Requested to stop connection
        , CMD_RegisterConnection        //!< Requested to register connection
        , CMD_UnregisterConnection      //!< Requested to unregister connection
        , CMD_LostConnection            //!< Requested to send notifications to components that connection is lost.
        , CMD_ShutdownService           //!< Request to shutdown service.
        , CMD_LoadModel                 //!< Request to load a model, or all models.
        , CMD_UnloadModel               //!< Request to unload a model, or all models.
        , CMD_ThreadRunning             //!< A component thread reports that its components started.
        , CMD_ThreadStopped             //!< A component thread reports that it exited.
        , CMD_WatchdogExpired           //!< The watchdog of a thread expired.
        , CMD_DeadlineExpired           //!< The lifecycle deadline guard expired.
        , CMD_UserState                 //!< Request to set the user state.
    };

    /**
     * \brief   Converts a service manager command to its string representation.
     **/
    static constexpr const char * as_string( ServiceManagerEventData::ServiceManagerCommand val ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates event data with the command to stop the router client connection.
     **/
    static ServiceManagerEventData stop_router_client();

    /**
     * \brief   Creates event data with the command to stop routing and shut down the service manager.
     **/
    static ServiceManagerEventData shutdown_service_manager();

    /**
     * \brief   Creates event data with the command to register a proxy.
     *
     * \param   addrProxy       The proxy address to register.
     **/
    static ServiceManagerEventData register_proxy( const ProxyAddress & addrProxy );

    /**
     * \brief   Creates event data with the command to unregister a proxy.
     *
     * \param   addrProxy       The proxy address to unregister.
     * \param   reason          The reason for unregistering or disconnecting the proxy.
     **/
    static ServiceManagerEventData unregister_proxy( const ProxyAddress & addrProxy, areg::DisconnectReason reason );

    /**
     * \brief   Creates event data with the command to register a stub.
     *
     * \param   addrStub    The stub address to register.
     **/
    static ServiceManagerEventData register_stub( const StubAddress & addrStub );

    /**
     * \brief   Creates event data with the command to unregister a stub.
     *
     * \param   addrStub    The stub address to unregister.
     * \param   reason      The reason for unregistering or disconnecting the stub.
     **/
    static ServiceManagerEventData unregister_stub( const StubAddress & addrStub, areg::DisconnectReason reason );

    /**
     * \brief   Creates event data with the command to configure a remote service connection.
     *
     * \param   service         The remote service to configure.
     * \param   connectTypes    The bitwise set of connection types to configure.
     * \return  Returns event data to send to the target.
     **/
    static ServiceManagerEventData configure_connection(areg::RemoteServiceKind service, uint32_t connectTypes );

    /**
     * \brief   Creates event data with the command to start a router client connection.
     *
     * \param   service         The remote service for which to start the connection.
     * \param   connectTypes    The bitwise set of connection types to start.
     * \return  Returns event data to send to the target.
     **/
    static ServiceManagerEventData start_connection(areg::RemoteServiceKind service, uint32_t connectTypes);

    /**
     * \brief   Creates event data with the command to start a router client connection to a remote service.
     *
     * \param   ipAddress       The IP address of the remote routing service.
     * \param   portNr          The port number to connect to.
     **/
    static ServiceManagerEventData start_net_connection( const String & ipAddress, uint16_t portNr );

    /**
     * \brief   Creates event data with the command to stop the connection.
     **/
    static ServiceManagerEventData stop_connection();

    /**
     * \brief   Creates event data with the command to register a channel connection.
     *
     * \param   channel     The channel information for the registered connection.
     **/
    static ServiceManagerEventData register_connection( const Channel & channel );

    /**
     * \brief   Creates event data with the command to unregister a channel connection.
     *
     * \param   channel     The channel information for the unregistered connection.
     **/
    static ServiceManagerEventData unregister_connection( const Channel & channel );

    /**
     * \brief   Creates event data with the command to report a lost connection.
     *
     * \param   channel     The channel information for the lost connection.
     **/
    static ServiceManagerEventData lost_connection( const Channel & channel );

    /**
     * \brief   Creates event data to load or unload a model.
     *
     * \param   load        True to load the model, false to unload it.
     * \param   modelName   The name of the model; empty for all models.
     * \param   request     The address of the request the caller waits for, or 0 if it does not wait.
     **/
    static ServiceManagerEventData model_request( bool load, const String & modelName, uint64_t request );

    /**
     * \brief   Creates event data of a component thread that reports its state.
     *
     * \param   running     True if the thread runs, false if it exited.
     * \param   threadName  The name of the component thread.
     * \param   instance    The address of the component thread object.
     **/
    static ServiceManagerEventData thread_report( bool running, const String & threadName, uint64_t instance );

    /**
     * \brief   Creates event data of an expired watchdog.
     *
     * \param   threadName  The name of the component thread the watchdog belongs to.
     * \param   instance    The address of the component thread object.
     * \param   guardedName The name of the thread whose event took too long: the component thread or its worker.
     **/
    static ServiceManagerEventData watchdog_expired( const String & threadName, uint64_t instance, const String & guardedName );

    /**
     * \brief   Creates event data of the expired lifecycle deadline guard.
     **/
    static ServiceManagerEventData deadline_expired();

    /**
     * \brief   Creates event data to set the user state.
     * \param   userState   The new user state.
     **/
    static ServiceManagerEventData user_state( uint32_t userState );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates event data with the specified service manager command.
     * \param   cmdServiceManager       The service manager command.
     **/
    inline explicit ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand cmdServiceManager );

    inline ServiceManagerEventData( const ServiceManagerEventData & source );

    inline ServiceManagerEventData( ServiceManagerEventData && source ) noexcept;

    ~ServiceManagerEventData() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Assigns service manager event data from the given source.
     *
     * \param   source      The source of data to copy.
     **/
    inline ServiceManagerEventData & operator = ( const ServiceManagerEventData & source );

    /**
     * \brief   Moves service manager event data from the given source.
     *
     * \param   source      The source of data to move.
     **/
    inline ServiceManagerEventData & operator = ( ServiceManagerEventData && source ) noexcept;

    /**
     * \brief   Returns a stream object for reading event data.
     **/
    inline const InStream & read_stream() const;

    /**
     * \brief   Returns a stream object for writing event data.
     **/
    inline OutStream & write_stream();

    /**
     * \brief   Returns the service manager command.
     **/
    inline ServiceManagerEventData::ServiceManagerCommand command() const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ServiceManagerCommand   mCommand;   //!< Service Manager event data command
    SharedBuffer            mStream;    //!< Service Manager event data streaming object to write and read data.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceManagerEventData() = delete;
};

//////////////////////////////////////////////////////////////////////////
// Service Manager event and consumer declaration
//////////////////////////////////////////////////////////////////////////
AREG_DECLARE_EVENT(ServiceManagerEventData, ServiceManagerEvent, ServiceManagerEventConsumer)

//////////////////////////////////////////////////////////////////////////
// ServiceManagerEventData inline methods
//////////////////////////////////////////////////////////////////////////

inline ServiceManagerEventData::ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand cmdServiceManager )
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

inline const InStream & ServiceManagerEventData::read_stream() const
{
    return static_cast<const InStream &>(mStream);
}

inline OutStream & ServiceManagerEventData::write_stream()
{
    return static_cast<OutStream &>(mStream);
}

inline ServiceManagerEventData::ServiceManagerCommand ServiceManagerEventData::command() const
{
    return mCommand;
}

inline constexpr const char * ServiceManagerEventData::as_string( ServiceManagerEventData::ServiceManagerCommand val ) noexcept
{
    switch ( val )
    {
    case ServiceManagerEventData::ServiceManagerCommand::CMD_StopRoutingClient:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_StopRoutingClient";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProxy:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProxy";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProxy:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProxy";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProvider:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProvider";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProvider:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProvider";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_ConfigureConnection:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_ConfigureConnection";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_StartConnection:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_StartConnection";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_StartNetConnection:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_StartNetConnection";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_StopConnection:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_StopConnection";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterConnection:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterConnection";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterConnection:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterConnection";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_LostConnection:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_LostConnection";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_ShutdownService:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_ShutdownService";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_LoadModel:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_LoadModel";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_UnloadModel:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_UnloadModel";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_ThreadRunning:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_ThreadRunning";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_ThreadStopped:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_ThreadStopped";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_WatchdogExpired:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_WatchdogExpired";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_DeadlineExpired:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_DeadlineExpired";
    case ServiceManagerEventData::ServiceManagerCommand::CMD_UserState:
        return "ServiceManagerEventData::ServiceManagerCommand::CMD_UserState";
    default:
        return "ERR: undefined ServiceManagerEventData::ServiceManagerCommand value!!!";
    }
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTS_HPP
