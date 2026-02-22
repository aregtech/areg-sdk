#ifndef AREG_IPC_REMOTESERVICEDEFS_HPP
#define AREG_IPC_REMOTESERVICEDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/RemoteServiceDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Remote service namespace
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/persist/PersistenceDefs.hpp"

#include <string_view>

 /************************************************************************
  * Dependencies
  ************************************************************************/
class StubAddress;
class ProxyAddress;
class RemoteMessage;
class Channel;

//////////////////////////////////////////////////////////////////////////
// NERemoteService namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote servicing namespace with shared objects and helper methods
 *          to create fixed communication messages.
 **/
namespace NERemoteService
{
    /**
     * \brief   NERemoteService::ConnectionType
     *          Remote services connection types.
     **/
    enum class ConnectionType   : uint32_t
    {
          Undefined     = 0 //!< Undefined connection
        , Tcpip         = 1 //!< Service connection via TCP/IP
        , Udp           = 2 //!< Service connection via UDP, not implemented at the moment
        , Web           = 4 //!< Service connection via Web socket
        , SharedMemory  = 8 //!< Service connection via Shared Memory
    };

    /**
     * \brief   NERemoteService::RemoteServiceKind
     *          Remote services
     **/
    enum class RemoteServiceKind : uint32_t
    {
          Unknown = 0 //!< Undefined service.
        , Router  = 1 //!< Message routing service.
        , Logger  = 2 //!< Message logging service.
    };

    /**
     * \brief   Indicates the connect or disconnect of the remote instance.
     **/
    enum class RemoteConnectionState : uint32_t
    {
          Disconnected  = 0 //!< Remote instance is disconnected.
        , Connected     = 1 //!< Remote instance is connected.
    };

    /**
     * \brief   NERemoteService::DEFAULT_REMOTE_SERVICE_ENABLED
     *          Message router enable / disable default flag. If true, by default it is enabled.
     *          The default values are used if failed to read and parse router configuration file.
     **/
    constexpr bool              DEFAULT_REMOTE_SERVICE_ENABLED  { true };

    /**
     * \brief   Returns fixed predefined message to start server connection
     **/
    AREG_API const NEMemory::RawMessage & getMessageHelloServer();

    /**
     * \brief   Returns fixed predefined message to stop server connection
     **/
    AREG_API const NEMemory::RawMessage & getMessageByeServer();

    /**
     * \brief   Returns fixed message to notify client connection statuses such as
     *          connection accepted, connection rejected and connection is closed.
     **/
    AREG_API const NEMemory::RawMessage & getMessageNotifyClientConnection();

    /**
     * \brief   Returns fixed message to start service registration
     **/
    AREG_API const NEMemory::RawMessage & getMessageRegisterService();

    /**
     * \brief   Returns fixed message to query service the connected instances. Only log observers can query.
     **/
    AREG_API const NEMemory::RawMessage & getMessageQueryInstances();

    /**
     * \brief   Returns fixed message to notify log observers the connected instances. Only log observers receive the message.
     **/
    AREG_API const NEMemory::RawMessage & getMessageNotifyInstances();

    /**
     * \brief   Returns fixed message to register notification
     **/
    AREG_API const NEMemory::RawMessage & getMessageRegisterNotify();

    /**
     * \brief   NERemoteService::CreateConnectRequest
     *          Initializes and returns connection request message.
     * \param   source      The iD of the source that generates and sends the message.
     * \param   target      The ID of the target to send the connect message request.
     * \param   msgSource   The message source of the application to connect to service
     **/
    AREG_API RemoteMessage createConnectRequest(const ITEM_ID & source, const ITEM_ID & target, NEService::MessageSource msgSource);

    /**
     * \brief   NERemoteService::CreateDisconnectRequest
     *          Initializes and returns disconnect request message.
     * \param   source  The ID of the source that sends the disconnect message request.
     * \param   target  The ID of the target to send the disconnect message request.
     **/
    AREG_API RemoteMessage createDisconnectRequest( const ITEM_ID & source, const ITEM_ID & target );

    /**
     * \brief   NERemoteService::CreateConnectNotify
     *          Initializes and returns connection available notification message
     * \param   source  The ID of the source that sends the connect notification message.
     * \param   target  The ID of the target to send the connect notification message.
     **/
    AREG_API RemoteMessage createConnectNotify( const ITEM_ID & source, const ITEM_ID & target );

    /**
     * \brief   NERemoteService::CreateRejectNotify
     *          Initializes and returns connection rejected message
     * \param   source  The ID of the source that sends the connection rejected message.
     * \param   target  The ID of the target to send the connection rejected message.
     **/
    AREG_API RemoteMessage createRejectNotify( const ITEM_ID & source, const ITEM_ID & target );

    /**
     * \brief   NERemoteService::CreateDisconnectNotify
     *          Initializes and returns disconnect notification message sent by server to clients.
     * \param   source  The ID of the source that sends the disconnect notification message.
     * \param   target  The ID of the target to send the disconnect notification message.
     **/
    AREG_API RemoteMessage createDisconnectNotify( const ITEM_ID & source, const ITEM_ID & target );

    /**
     * \brief   NERemoteService::createRouterRegisterService
     *          Initializes and returns message to register Stub at the router.
     * \param   stub    The address of remote Stub service, which is registering.
     * \param   source  The ID of the source that sends the request message to register service provider.
     * \param   target  The ID of the target to send the request message to register service provider.
     * \see     createRouterUnregisterService
     **/
    AREG_API RemoteMessage createRouterRegisterService( const StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target);

    /**
     * \brief   NERemoteService::createRouterUnregisterService
     *          Initializes and returns message to unregister Stub at the router.
     * \param   stub    The address of remote Stub service, which is registering.
     * \param   reason  The unregister / disconnect reason.
     * \param   source  The ID of the source that sends the request message to unregister service provider.
     * \param   target  The ID of the target to send the request message to unregister service provider.
     * \see     createRouterRegisterService
     **/
    AREG_API RemoteMessage createRouterUnregisterService( const StubAddress & stub, NEService::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target);

    /**
     * \brief   NERemoteService::createRouterRegisterClient
     *          Initializes and returns message to register Proxy at the router.
     * \param   proxy   The address of remote Proxy, which is registering.
     * \param   source  The ID of the source that sends the request message to register service consumer.
     * \param   target  The ID of the target to send the request message to register service consumer.
     * \see     createRouterUnregisterClient
     **/
    AREG_API RemoteMessage createRouterRegisterClient( const ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target );

    /**
     * \brief   NERemoteService::createRouterUnregisterClient
     *          Initializes and returns message to unregister Proxy at the router.
     * \param   proxy   The address of remote Proxy, which is registering.
     * \param   reason  The unregister / disconnect reason.
     * \param   source  The ID of the source that sends the request message to unregister service consumer.
     * \param   target  The ID of the target to send the request message to unregister service consumer.
     * \see     createRouterRegisterClient
     **/
    AREG_API RemoteMessage createRouterUnregisterClient( const ProxyAddress & proxy, NEService::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target);

    /**
     * \brief   NERemoteService::createServiceRegisteredNotification
     *          Initializes and returns Stub available notification message to broadcast.
     * \param   stub    The address of remote Stub to notify registering.
     * \param   source  The ID of the source that sends the service provider is registered notification message.
     * \param   target  The ID of the target to send the service provider is registered notification message.
     * \see     createServiceUnregisteredNotification
     **/
    AREG_API RemoteMessage createServiceRegisteredNotification( const StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target );

    /**
     * \brief   NERemoteService::createServiceUnregisteredNotification
     *          Initializes and returns Stub unavailable notification message to broadcast.
     * \param   stub    The address of remote Stub to notify unregistering.
     * \param   reason  The unregister / disconnect reason.
     * \param   source  The ID of the source that sends the service provider is unregistered notification message.
     * \param   target  The ID of the target to send the service provider is unregistered notification message.
     * \see     createServiceRegisteredNotification
     **/
    AREG_API RemoteMessage createServiceUnregisteredNotification( const StubAddress & stub, NEService::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target);

    /**
     * \brief   NERemoteService::createServiceClientRegisteredNotification
     *          Initializes and returns Proxy available notification message to broadcast.
     * \param   proxy   The address of remote Proxy to notify registering.
     * \param   source  The ID of the source that sends the service consumer is registered notification message.
     * \param   target  The ID of the target to send the service consumer is registered notification message.
     * \see     createServiceClientUnregisteredNotification
     **/
    AREG_API RemoteMessage createServiceClientRegisteredNotification( const ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target);

    /**
     * \brief   NERemoteService::createServiceClientUnregisteredNotification
     *          Initializes and returns Proxy unavailable notification message to broadcast.
     * \param   proxy   The address of remote Proxy to notify unregistering.
     * \param   reason  The unregister / disconnect reason.
     * \param   source  The ID of the source that sends the service consumer is unregistered notification message.
     * \param   target  The ID of the target to send the service consumer is unregistered notification message.
     * \see     createServiceClientRegisteredNotification
     **/
    AREG_API RemoteMessage createServiceClientUnregisteredNotification( const ProxyAddress & proxy, NEService::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target);

    /**
     * \brief   NERemoteService::isMessageHelloServer
     *          Checks whether specified message is a connect request.
     * \param   msgHelloServer      The instance of remote message to check.
     * \return  Returns true if the message is connect request.
     **/
    AREG_API bool isMessageHelloServer( const RemoteMessage & msgHelloServer );

    /**
     * \brief   NERemoteService::isMessageByeServer
     *          Checks whether specified message is a disconnect request.
     * \param   msgByeServer        The instance of remote message to check.
     * \return  Returns true if the message is disconnect request.
     **/
    AREG_API bool isMessageByeServer( const RemoteMessage & msgByeServer );

    /**
     * \brief   NERemoteService::isMessagNotifyClient
     *          Checks whether specified message is client notification request.
     * \param   msgNotifyClient     The instance of remote message to check.
     * \return  Returns true if the message is a client notification request.
     **/
    AREG_API bool isMessagNotifyClient( const RemoteMessage & msgNotifyClient );

    /**
     * \brief   NERemoteService::isMessageRegisterService
     *          Checks whether specified message is service notification request.
     * \param   msgRegisterService  The instance of remote message to check.
     * \return  Returns true if the message is a service notification request.
     **/
    AREG_API bool isMessageRegisterService( const RemoteMessage & msgRegisterService );
}

//////////////////////////////////////////////////////////////////////////
// Make RemoteServiceKind and ConnectionType streamable
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_STREAMABLE(NERemoteService::RemoteServiceKind);
AREG_IMPLEMENT_STREAMABLE(NERemoteService::ConnectionType);
AREG_IMPLEMENT_STREAMABLE(NERemoteService::RemoteConnectionState);

#endif  // AREG_IPC_REMOTESERVICEDEFS_HPP
