#ifndef AREG_IPC_PRIVATE_NECONNECTION_HPP
#define AREG_IPC_PRIVATE_NECONNECTION_HPP

/************************************************************************
 * \file        areg/ipc/private/NEConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Connection constants.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NEMemory.hpp"
#include "areg/component/CETimer.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/ipc/NERemoteService.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEStubAddress;
class CEProxyAddress;
class CERemoteMessage;
class CEChannel;

//////////////////////////////////////////////////////////////////////////
// NEConnection namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The connection namespace with shared objects and helper methods
 *          to create fixed communication messages.
 **/
namespace NEConnection
{
    /**
     * \brief   NEConnection::MessageHelloServer
     *          Fixed predefined message to start server connection
     **/
    extern const NEMemory::sRemoteMessage      MessageHelloServer;
    /**
     * \brief   NEConnection::MessageByeServer
     *          Fixed predefined message to stop server connection
     **/
    extern const NEMemory::sRemoteMessage      MessageByeServer;
    /**
     * \brief   NEConnection::MessageAcceptClient
     *          Fixed message to notify client connection acceptance
     **/
    extern const NEMemory::sRemoteMessage      MessageAcceptClient;
    /**
     * \brief   NEConnection::MessageRejectClient
     *          Fixed message to notify client connected rejection
     **/
    extern const NEMemory::sRemoteMessage      MessageRejectClient;
    /**
     * \brief   NEConnection::MessageByeClient
     *          Fixed message to notify client disconnect
     **/
    extern const NEMemory::sRemoteMessage      MessageByeClient;
    /**
     * \brief   NEConnection::MessageRegisterService
     *          Fixed message to start service registration
     **/
    extern const NEMemory::sRemoteMessage      MessageRegisterService;
    /**
     * \brief   NEConnection::MessageQueryService
     *          Fixed message to query available services
     **/
    extern const NEMemory::sRemoteMessage      MessageQueryService;
    /**
     * \brief   NEConnection::MessageRegisterNotify
     *          Fixed message to register notification
     **/
    extern const NEMemory::sRemoteMessage      MessageRegisterNotify;

    /**
     * \brief   NEConnection::CLIENT_SEND_MESSAGE_THREAD
     *          Fixed name of client message sender thread
     **/
    const char * const      CLIENT_SEND_MESSAGE_THREAD      = "CLIENT_SEND_MESSAGE_THREAD";
    /**
     * \brief   NEConnection::CLIENT_RECEIVE_MESSAGE_THREAD
     *          Fixed name of client message receiver thread
     **/
    const char * const      CLIENT_RECEIVE_MESSAGE_THREAD   = "CLIENT_RECEIVE_MESSAGE_THREAD";
    /**
     * \brief   NEConnection::CLIENT_DISPATCH_MESSAGE_THREAD
     *          Fixed name of client message dispatcher thread
     **/
    const char * const      CLIENT_DISPATCH_MESSAGE_THREAD  = "CLIENT_DISPATCH_MESSAGE_THREAD";
    /**
     * \brief   NEConnection::SERVER_SEND_MESSAGE_THREAD
     *          Fixed name of server message sender thread
     **/
    const char * const      SERVER_SEND_MESSAGE_THREAD      = "SERVER_SEND_MESSAGE_THREAD";
    /**
     * \brief   NEConnection::SERVER_RECEIVE_MESSAGE_THREAD
     *          Fixed name of server message receiver thread
     **/
    const char * const      SERVER_RECEIVE_MESSAGE_THREAD   = "SERVER_RECEIVE_MESSAGE_THREAD";
    /**
     * \brief   NEConnection::SERVER_DISPATCH_MESSAGE_THREAD
     *          Fixed name of server message dispatcher thread
     **/
    const char * const      SERVER_DISPATCH_MESSAGE_THREAD  = "SERVER_DISPATCH_MESSAGE_THREAD";
    /**
     * \brief   NEConnection::CLIENT_CONNECT_TIMER_NAME
     *          Fixed name of client connection retry timer name
     **/
    const char * const      CLIENT_CONNECT_TIMER_NAME       = "CLIENT_CONNECT_TIMER_NAME";
    /**
     * \brief   NEConnection::SERVER_CONNECT_TIMER_NAME
     *          Fixed name of server connection retry timer name
     **/
    const char * const      SERVER_CONNECT_TIMER_NAME       = "SERVER_CONNECT_TIMER_NAME";
    /**
     * \brief   NEConnection::DEFAULT_BROKER_CONFIG_FILE
     *          Default path of message broker configuration file name
     **/
    const char * const      DEFAULT_BROKER_CONFIG_FILE      = NERemoteService::DEFAULT_CONFIG_FILE;
    /**
     * \brief   NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT
     *          Default connect retry timer timeout value in milliseconds
     **/
    const unsigned int      DEFAULT_RETRY_CONNECT_TIMEOUT   = CETimer::TIMEOUT_500_MS;  // 500 ms
    /**
     * \brief   NEConnection::DEFAULT_REMOTE_SERVICE_HOST
     *          Default IP-address of remote message broker.
     *          The default values are used if failed to read and parse broker configuration file.
     **/
    const char * const      DEFAULT_REMOTE_SERVICE_HOST     = NERemoteService::DEFAULT_BROKER_HOST;
    /**
     * \brief   NEConnection::DEFAULT_REMOTE_SERVICE_PORT
     *          Default connection port number of message broker
     *          The default values are used if failed to read and parse broker configuration file.
     **/
    const unsigned short    DEFAULT_REMOTE_SERVICE_PORT     = NERemoteService::DEFAULT_BROKER_PORT;
    /**
     * \brief   NEConnection::DEFAULT_REMOVE_SERVICE_ENABLED
     *          Message broker enable / disable default flag. If true, by default it is enabled.
     *          The default values are used if failed to read and parse broker configuration file.
     **/
    const bool              DEFAULT_REMOVE_SERVICE_ENABLED  = false;

    /**
     * \brief   NEConnection::CreateConnectRequest
     *          Initializes connection request message.
     **/
    CERemoteMessage CreateConnectRequest( void );
    /**
     * \brief   NEConnection::CreateDisconnectRequest
     *          Initializes disconnect request message.
     * \param   cookie  The cookie generated by broker for the client, which is set in message
     **/
    CERemoteMessage CreateDisconnectRequest( ITEM_ID cookie );
    /**
     * \brief   NEConnection::CreateConnectNotify
     *          Initializes connection notification message
     * \param   cookie  The cookie generated by broker for the client, which is set in message
     **/
    CERemoteMessage CreateConnectNotify( ITEM_ID cookie );
    /**
     * \brief   NEConnection::CreateRejectNotify
     *          Initializes connection rejected message
     * \param   cookie  The cookie generated by broker for the client, which is set in message
     **/
    CERemoteMessage CreateRejectNotify( ITEM_ID cookie );
    /**
     * \brief   NEConnection::CreateDisconnectNotify
     *          Initializes disconnect notification message sent by server to clients.
     * \param   cookie  The cookie generated by broker for the client, which is set in message
     **/
    CERemoteMessage CreateDisconnectNotify( ITEM_ID cookie );
    /**
     * \brief   NEConnection::CreateBrokerRegisterService
     *          Initializes Stub service registering message for the broker.
     * \param   stub    The address of remote Stub service, which is registering.
     * \param   source  The ID of source, which initiates message.
     **/
    CERemoteMessage CreateBrokerRegisterService( const CEStubAddress & stub, ITEM_ID source );
    /**
     * \brief   NEConnection::CreateBrokerUnregisterService
     *          Initializes Stub service unregistering message for the broker.
     * \param   stub    The address of remote Stub service, which is registering.
     * \param   source  The ID of source, which initiates message.
     **/
    CERemoteMessage CreateBrokerUnregisterService( const CEStubAddress & stub, ITEM_ID source );
    /**
     * \brief   NEConnection::CreateBrokerRegisterClient
     *          Initializes Proxy client service registering message for the broker.
     * \param   proxy   The address of remote Proxy, which is registering.
     * \param   source  The ID of source, which initiates message.
     **/
    CERemoteMessage CreateBrokerRegisterClient( const CEProxyAddress & proxy, ITEM_ID source );
    /**
     * \brief   NEConnection::CreateBrokerUnregisterClient
     *          Initializes Proxy client service unregistering message for the broker.
     * \param   proxy   The address of remote Proxy, which is registering.
     * \param   source  The ID of source, which initiates message.
     **/
    CERemoteMessage CreateBrokerUnregisterClient( const CEProxyAddress & proxy, ITEM_ID source );
    /**
     * \brief   NEConnection::CreateServiceRegisteredNotification
     *          Initializes Stub service registering notification message to broadcast.
     * \param   stub    The address of remote Stub to notify registering.
     * \param   target  The ID of target to notify message.
     **/
    CERemoteMessage CreateServiceRegisteredNotification( const CEStubAddress & stub, ITEM_ID target );
    /**
     * \brief   NEConnection::CreateServiceUnregisteredNotification
     *          Initializes Stub service unregistering notification message to broadcast.
     * \param   stub    The address of remote Stub to notify unregistering.
     * \param   target  The ID of target to notify message.
     **/
    CERemoteMessage CreateServiceUnregisteredNotification( const CEStubAddress & stub, ITEM_ID target );
    /**
     * \brief   NEConnection::CreateServiceClientRegisteredNotification
     *          Initializes Proxy client service registering notification message to broadcast.
     * \param   proxy   The address of remote Proxy to notify registering.
     * \param   target  The ID of target to notify message.
     **/
    CERemoteMessage CreateServiceClientRegisteredNotification( const CEProxyAddress & proxy, ITEM_ID target );
    /**
     * \brief   NEConnection::CreateServiceClientUnregisteredNotification
     *          Initializes Proxy client service unregistering notification message to broadcast.
     * \param   proxy   The address of remote Proxy to notify unregistering.
     * \param   target  The ID of target to notify message.
     **/
    CERemoteMessage CreateServiceClientUnregisteredNotification( const CEProxyAddress & proxy, ITEM_ID target );
    /**
     * \brief   NEConnection::IsMessageHelloServer
     *          Checks whether specified message is a connect request.
     * \param   msgHelloServer      The instance of remote message to check.
     * \return  Returns true if the message is connect request.
     **/
    bool IsMessageHelloServer( const CERemoteMessage & msgHelloServer );
    /**
     * \brief   NEConnection::IsMessageByeServer
     *          Checks whether specified message is a disconnect request.
     * \param   msgByeServer        The instance of remote message to check.
     * \return  Returns true if the message is disconnect request.
     **/
    bool IsMessageByeServer( const CERemoteMessage & msgByeServer );
    /**
     * \brief   NEConnection::IsMessagNotifyClient
     *          Checks whether specified message is client notification request.
     * \param   msgNotifyClient     The instance of remote message to check.
     * \return  Returns true if the message is a client notification request.
     **/
    bool IsMessagNotifyClient( const CERemoteMessage & msgNotifyClient );
    /**
     * \brief   NEConnection::IsMessagNotifyClient
     *          Checks whether specified message is service notification request.
     * \param   msgRegisterService  The instance of remote message to check.
     * \return  Returns true if the message is a service notification request.
     **/
    bool IsMessageRegisterService( const CERemoteMessage & msgRegisterService );
}

#endif  // AREG_IPC_PRIVATE_NECONNECTION_HPP
