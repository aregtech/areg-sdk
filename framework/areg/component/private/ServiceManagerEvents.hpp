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
#include "areg/component/ServiceDefs.hpp"
#include "areg/component/EventTemplate.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg { class ProxyAddress; }
namespace areg { class StubAddress; }
namespace areg { class Channel; }
namespace areg { class String; }

namespace areg
{
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
        enum class ServiceManagerCommand    : uint16_t
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
        };

        /**
         * \brief   Converts event command to string
         * \param   val     The command to convert to string
         **/
        static const char * getString( ServiceManagerEventData::ServiceManagerCommand val );

    //////////////////////////////////////////////////////////////////////////
    // Static members
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates and returns Service Manager event data with command to stop router client connection.
         **/
        static ServiceManagerEventData stopMessageRouterClient();

        /**
         * \brief   creates and returns service manager event with command to stop routing connection and stop service manager.
         **/
        static ServiceManagerEventData shutdownServiceManager();

        /**
         * \brief   Creates and returns Service Manager event data with command to register Proxy
         **/
        static ServiceManagerEventData registerProxy( const ProxyAddress & addrProxy );

        /**
         * \brief   Creates and returns Service Manager event data with command to unregister Proxy
         * \param   addrProxy   The address of the service consumer to unregister.
         * \param   reason      The reason to unregister or disconnect the service consumer.
         **/
        static ServiceManagerEventData unregisterProxy( const ProxyAddress & addrProxy, DisconnectReason reason );

        /**
         * \brief   Creates and returns Service Manager event data with command to register Stub
         **/
        static ServiceManagerEventData registerStub( const StubAddress & addrStub );

        /**
         * \brief   Creates and returns Service Manager event data with command to unregister Stub
         * \param   addrStub    The address of the service provider to unregister / disconnect.
         * \param   reason      The reason to unregister or disconnect the service provider.
         **/
        static ServiceManagerEventData unregisterStub( const StubAddress & addrStub, DisconnectReason reason );

        /**
         * \brief   Creates and returns Service Manager event data with command to configure connection.
         * \param   service         The remote service, which connection should be configured.
         * \param   connectTypes    The bitwise set of connections to configure.
         * \return  Returns  event data to send to the target.
         **/
        static ServiceManagerEventData configureConnection(RemoteServiceKind service, uint32_t connectTypes );

        /**
         * \brief   Creates and returns Service Manager event data with command to start router client connection
         * \param   service         The remote service, which connection should be started.
         * \param   connectTypes    The bitwise set of connections to start.
         * \return  Returns  event data to send to the target.
         **/
        static ServiceManagerEventData startConnection(RemoteServiceKind service, uint32_t connectTypes);

        /**
         * \brief   Creates and returns Service Manager event data with command to start router client connection.
         * \param   ipAddress   The IP-Address of remote Routing Service.
         * \param   portNr      The port number to connect.
         **/
        static ServiceManagerEventData startNetConnection( const String & ipAddress, uint16_t portNr );

        /**
         * \brief   Creates and returns Service Manager event data with command to stop connection
         **/
        static ServiceManagerEventData stopConnection();

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
        inline explicit ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand cmdServiceManager );

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
        ~ServiceManagerEventData() = default;

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
        inline const InStream & getReadStream() const;

        /**
         * \brief   Returns streaming object to write data
         **/
        inline OutStream & getWriteStream();

        /**
         * \brief   Returns Service Manager command
         **/
        inline ServiceManagerEventData::ServiceManagerCommand getCommand() const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    private:
        ServiceManagerCommand mCommand;   //!< Service Manager event data command
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

    inline const InStream & ServiceManagerEventData::getReadStream() const
    {
        return static_cast<const InStream &>(mStream);
    }

    inline OutStream & ServiceManagerEventData::getWriteStream()
    {
        return static_cast<OutStream &>(mStream);
    }

    inline ServiceManagerEventData::ServiceManagerCommand ServiceManagerEventData::getCommand() const
    {
        return mCommand;
    }

    inline const char * ServiceManagerEventData::getString( ServiceManagerEventData::ServiceManagerCommand val )
    {
        switch ( val )
        {
        case ServiceManagerEventData::ServiceManagerCommand::CMD_StopRoutingClient:
            return "ServiceManagerEventData::ServiceManagerCommand::CMD_StopRoutingClient";
        case ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProxy:
            return "ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProxy";
        case ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProxy:
            return "ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProxy";
        case ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterStub:
            return "ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterStub";
        case ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterStub:
            return "ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterStub";
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
        case ServiceManagerEventData::ServiceManagerCommand::CMD_TerminateComponentThread:
            return "ServiceManagerEventData::ServiceManagerCommand::CMD_TerminateComponentThread";
        case ServiceManagerEventData::ServiceManagerCommand::CMD_StartComponentThread:
            return "ServiceManagerEventData::ServiceManagerCommand::CMD_StartComponentThread";
        default:
            return "ERR: undefined ServiceManagerEventData::ServiceManagerCommand value!!!";
        }
    }

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTS_HPP
