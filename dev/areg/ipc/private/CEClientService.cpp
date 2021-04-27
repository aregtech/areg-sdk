/************************************************************************
 * \file        areg/ipc/private/CEClientService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Connection client implementation
 ************************************************************************/
#include "areg/ipc/CEClientService.hpp"

#include "areg/ipc/CEConnectionConfiguration.hpp"
#include "areg/ipc/IERemoteServiceConsumer.hpp"
#include "areg/ipc/private/NEConnection.hpp"
#include "areg/ipc/private/CERemoteServiceEvent.hpp"
#include "areg/component/CEDispatcherThread.hpp"
#include "areg/component/CEStreamableEvent.hpp"
#include "areg/component/CERemoteEventFactory.hpp"
#include "areg/component/CERequestEvents.hpp"
#include "areg/component/CEResponseEvents.hpp"
#include "areg/component/NEService.hpp"
#include "areg/base/CEProcess.hpp"
#include "areg/base/CEFile.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_StartRemotingService);
DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_StopRemotingService);
DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_ProcessEvent);
DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_startConnection);
DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_stopConnection);
DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_FailedSendMessage);
DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_FailedReceiveMessage);
DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_FailedProcessMessage);
DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_ProcessReceivedMessage);
DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_ProcessRemoteRequestEvent);
DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_ProcessRemoteNotifyRequestEvent);
DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_ProcessRemoteResponseEvent);
DEF_TRACE_SCOPE(areg_ipc_private_CEClientService_RunDispatcher);

const NERemoteService::eServiceConnection   CEClientService::CONNECT_TYPE   = NERemoteService::ConnectionTcpip;

CEClientService::CEClientService( IERemoteServiceConsumer & serviceConsumer )
    : IERemoteService              ( )
    , CEDispatcherThread           ( NEConnection::CLIENT_DISPATCH_MESSAGE_THREAD )
    , IERemoteServiceHandler       ( )
    , IEClientServiceEventConsumer ( )
    , IERemoteEventConsumer        ( )
    , IETimerConsumer              ( )

    , mClientConnection ( )
    , mServiceConsumer  ( serviceConsumer )
    , mTimerConnect     ( static_cast<IETimerConsumer &>(self()), NEConnection::CLIENT_CONNECT_TIMER_NAME)
    , mThreadReceive    ( static_cast<IERemoteServiceHandler &>(self()), mClientConnection )
    , mThreadSend       ( static_cast<IERemoteServiceHandler &>(self()), mClientConnection )
    , mIsServiceEnabled ( NEConnection::DEFAULT_REMOVE_SERVICE_ENABLED )    // TODO: by default, should be false and read out from configuration file.
    , mConfigFile       ( "" )
    , mChannel          ( )
    , mConnectionState  ( ConnectionStopped )
    , mLock             ( )
{
    ; // do nothing
}

CEClientService::~CEClientService(void)
{
    ; // do nothing
}

bool CEClientService::ServiceConfigure( const char * configFile )
{
    CELock lock( mLock );
    CEConnectionConfiguration configConnect;
    if ( configConnect.LoadConfiguration( configFile ) )
    {
        mConfigFile             = configConnect.GetConfigFileName( );
        mIsServiceEnabled       = configConnect.GetConnectionEnabled( CONNECT_TYPE );
        CEString hostName       = configConnect.GetConnectionHost( CONNECT_TYPE );
        unsigned short hostPort = configConnect.GetConnectionPort( CONNECT_TYPE );

        return mClientConnection.SetAddress( hostName, hostPort );
    }
    else
    {
        return mClientConnection.SetAddress( NEConnection::DEFAULT_REMOTE_SERVICE_HOST, NEConnection::DEFAULT_REMOTE_SERVICE_PORT );
    }
}

void CEClientService::SetRouterServiceAddress( const char * hostName, unsigned short portNr )
{
    CELock lock( mLock );
    mClientConnection.SetAddress( hostName, portNr );
}

bool CEClientService::StartRemotingService(void)
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_StartRemotingService);
    CELock lock( mLock );
    bool result = true;
    if ( mClientConnection.IsValid() == false && IsRunning() == false )
    {
        if ( IsServiceEnabled() )
        {
            lock.Unlock();

            TRACE_DBG("The remote servicing is enabled, going to start remote servicing");
            result = CreateThread(CEThread::WAIT_INFINITE) && WaitForDispatcherStart(IESynchObject::WAIT_INFINITE);

            lock.Lock();
        }
        else
        {
            TRACE_DBG("The remote servicing is disabled, nothing to start");
            result = true;
        }
    }
    else
    {
        TRACE_DBG("Ignoring to start remote servicing. The client connection is [ %s ], the servicing thread is [ %s ]"
                    , mClientConnection.IsValid() ? "VALID" : "INVALID"
                    , IsRunning() ? "RUNNING" : "NOT RUNNING");
        ASSERT(IsRunning());
    }

    return result;
}

void CEClientService::StopRemotingService(void)
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_StopRemotingService);
    TRACE_DBG( "Stopping remote servicing client connection, current state is [ %s ]", IsRunning() ? "RUNNING" : "NOT RUNNING" );
    if ( IsRunning() )
    {
        CEClientServiceEvent::SendEvent( CEClientServiceEventData(CEClientServiceEventData::CMD_StopService), static_cast<IEClientServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );
        CEDispatcherThread::SetExitEvent();
        CompletionWait(CEThread::WAIT_INFINITE);
        DestroyThread( CEThread::DO_NOT_WAIT );
    }
}

bool CEClientService::IsServiceStarted(void) const
{
    CELock lock( mLock );
    return isStarted();
}

bool CEClientService::IsServiceConfigured( void ) const
{
    CELock lock(mLock);
    return mClientConnection.GetAddress().IsValid();
}

bool CEClientService::IsServiceEnabled(void) const
{
    CELock lock(mLock);
    return mIsServiceEnabled;
}

void CEClientService::EnableService( bool enable )
{
    CELock lock( mLock );
    if ( IsRunning() )
    {
        if ( enable == false )
            StopRemotingService();
    }
    mIsServiceEnabled = enable;
}

bool CEClientService::RegisterService( const CEStubAddress & stubService )
{
    CELock lock( mLock );
    bool result = false;
    if ( isStarted() )
    {
        result = CESendMessageEvent::SendEvent(  CESendMessageEventData(NEConnection::CreateBrokerRegisterService(stubService, mClientConnection.GetCookie()))
                                               , static_cast<IESendMessageEventConsumer &>(mThreadSend)
                                               , static_cast<CEDispatcherThread &>(mThreadSend) );
    }
    return result;
}

void CEClientService::UnregisterService(const CEStubAddress & stubService)
{
    CELock lock( mLock );
    if ( isStarted() )
    {
        CESendMessageEvent::SendEvent(  CESendMessageEventData(NEConnection::CreateBrokerUnregisterService(stubService, mClientConnection.GetCookie()))
                                        , static_cast<IESendMessageEventConsumer &>(mThreadSend)
                                        , static_cast<CEDispatcherThread &>(mThreadSend) );
    }
}

bool CEClientService::RegisterServiceClient(const CEProxyAddress & proxyService)
{
    CELock lock( mLock );
    bool result = false;
    if ( isStarted() )
    {
        result = CESendMessageEvent::SendEvent( CESendMessageEventData(NEConnection::CreateBrokerRegisterClient(proxyService, mClientConnection.GetCookie()))
                                                , static_cast<IESendMessageEventConsumer &>(mThreadSend)
                                                , static_cast<CEDispatcherThread &>(mThreadSend) );
    }
    return result;
}

void CEClientService::UnregisterServiceClient(const CEProxyAddress & proxyService)
{
    CELock lock( mLock );
    if ( isStarted() )
    {
        CESendMessageEvent::SendEvent(  CESendMessageEventData(NEConnection::CreateBrokerUnregisterClient(proxyService, mClientConnection.GetCookie()))
                                        , static_cast<IESendMessageEventConsumer &>(mThreadSend)
                                        , static_cast<CEDispatcherThread &>(mThreadSend) );
    }
}

void CEClientService::ProcessTimer(CETimer & timer)
{
    if ( &timer == &mTimerConnect )
        startConnection();
}

void CEClientService::ProcessEvent( const CEClientServiceEventData & data )
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_ProcessEvent);
    
    CEClientServiceEventData::eClientServiceCommands cmdService = data.GetCommand();
    TRACE_DBG("Client service is executing command [ %s ]", CEClientServiceEventData::GetString(cmdService));

    switch ( cmdService )
    {
    case CEClientServiceEventData::CMD_StartService:
        {
            TRACE_DBG("Starting remove servicing");;
            mChannel.SetCookie( NEService::COOKIE_LOCAL );
            mChannel.SetSource( NEService::SOURCE_UNKNOWN );
            mChannel.SetTarget( NEService::TARGET_UNKNOWN );
            startConnection();
        }
        break;

    case CEClientServiceEventData::CMD_StopService:
        {
            TRACE_DBG("Stopping remote servicing");
            setConnectionState(CEClientService::ConnectionStopping);
            CEChannel channel = mChannel;
            mChannel.SetCookie( NEService::COOKIE_UNKNOWN );
            mChannel.SetSource( NEService::SOURCE_UNKNOWN );
            mChannel.SetTarget( NEService::TARGET_UNKNOWN );
            stopConnection();
            mThreadReceive.CompletionWait( CEThread::WAIT_INFINITE );
            mThreadSend.CompletionWait( CEThread::WAIT_INFINITE );
            mServiceConsumer.RemoteServiceStopped( channel );

            mThreadReceive.DestroyThread( CEThread::DO_NOT_WAIT );
            mThreadSend.DestroyThread( CEThread::DO_NOT_WAIT );
        }
        break;

    case CEClientServiceEventData::CMD_ServiceStarted:
        {
            ASSERT(isStarted());
            if ( mClientConnection.GetCookie() != NEService::COOKIE_LOCAL )
            {
                TRACE_DBG("Client service succeeded to start, client cookie is [ %p ]", mClientConnection.GetCookie());
                setConnectionState(CEClientService::ConnectionStarted);
                mChannel.SetCookie( mClientConnection.GetCookie() );
                mChannel.SetSource( GetCurrentThreadId() );
                mChannel.SetTarget( NEService::TARGET_LOCAL );
                mServiceConsumer.RemoteServiceStarted(mChannel);
            }
        }
        break;

    case CEClientServiceEventData::CMD_ServiceStopped:
        {
            TRACE_DBG("Client service is stopped. Resetting cookie");
            setConnectionState(CEClientService::ConnectionStopping);
            CEChannel channel = mChannel;
            mChannel.SetCookie( NEService::COOKIE_UNKNOWN );
            mChannel.SetSource( NEService::SOURCE_UNKNOWN );
            mChannel.SetTarget( NEService::TARGET_UNKNOWN );
            stopConnection();

            mThreadReceive.CompletionWait( CEThread::WAIT_INFINITE );
            mThreadSend.CompletionWait( CEThread::WAIT_INFINITE );
            mServiceConsumer.RemoteServiceStopped( channel );
        }
        break;

    case CEClientServiceEventData::CMD_ServiceLost:
    {
        TRACE_WARN("Client service is lost connection. Resetting cookie and trying to restart");
        CEChannel channel = mChannel;
        mChannel.SetCookie( NEService::COOKIE_UNKNOWN );
        mChannel.SetSource( NEService::SOURCE_UNKNOWN );
        mChannel.SetTarget( NEService::TARGET_UNKNOWN );
        stopConnection();

        mThreadReceive.CompletionWait( CEThread::WAIT_INFINITE );
        mThreadSend.CompletionWait( CEThread::WAIT_INFINITE );
        mServiceConsumer.RemoteServiceStopped( channel );

        if (getConnectionState() == CEClientService::ConnectionStarted)
        {
            TRACE_DBG("Restarting lost connection with remote service");
            setConnectionState(CEClientService::ConnectionStarting);
            startConnection();
        }
    }
    break;

    default:
        ASSERT(false);
        break;
    }
}

inline bool CEClientService::isStarted(void) const
{
    ITEM_ID cookie = mClientConnection.GetCookie();
    return (mClientConnection.IsValid() && (cookie != NEService::COOKIE_LOCAL) && (cookie != NEService::COOKIE_UNKNOWN));
}

inline void CEClientService::setConnectionState(const CEClientService::eConnectionState newState)
{
    mConnectionState    = newState;
}

inline CEClientService::eConnectionState CEClientService::getConnectionState( void ) const
{
    return mConnectionState;
}

inline bool CEClientService::startConnection( void )
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_startConnection);

    ASSERT(mClientConnection.GetAddress().IsValid());
    ASSERT(mClientConnection.IsValid() == false);
    ASSERT(mThreadReceive.IsRunning() == false);
    ASSERT(mThreadSend.IsRunning() == false);

    bool result = false;
    mTimerConnect.StopTimer();

    if ( mClientConnection.CreateSocket() )
    {
        if ( mThreadReceive.CreateThread( CEThread::WAIT_INFINITE ) && mThreadSend.CreateThread( CEThread::WAIT_INFINITE ) )
        {
            VERIFY( mThreadReceive.WaitForDispatcherStart( IESynchObject::WAIT_INFINITE ) );
            VERIFY( mThreadSend.WaitForDispatcherStart( IESynchObject::WAIT_INFINITE ) );
            TRACE_DBG("Client service starting connection to broker service.");
            result = mClientConnection.RequestConnectServer();
        }
    }
    if ( result == false )
    {
        TRACE_WARN("Client service failed to start connection, going to repeat connection in [ %u ] ms", NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT);
        mThreadSend.DestroyThread( CEThread::DO_NOT_WAIT );
        mThreadReceive.DestroyThread( CEThread::DO_NOT_WAIT );
        mClientConnection.CloseSocket();
        mTimerConnect.StartTimer( NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, static_cast<CEDispatcherThread &>(self()), 1);
    }

    return result;
}

inline void CEClientService::stopConnection(void)
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_stopConnection);
    TRACE_WARN("Stopping client service connection");
    mTimerConnect.StopTimer();

    mClientConnection.RequestDisconnectServer();
    mClientConnection.DisableReceive();
    mClientConnection.SetCookie( NEService::COOKIE_UNKNOWN );

    mThreadReceive.SetExitEvent();
    mThreadSend.SetExitEvent();

    mThreadReceive.DestroyThread( CEThread::DO_NOT_WAIT );
    mThreadSend.DestroyThread( CEThread::DO_NOT_WAIT );
}

void CEClientService::FailedSendMessage(const CERemoteMessage & msgFailed)
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_FailedSendMessage);
    TRACE_WARN("Failed to send message [ %p ] to target [ %p ], source is [ %p ]", msgFailed.GetMessageId(), msgFailed.GetTarget(), msgFailed.GetSource());

    CEStreamableEvent * eventError = CERemoteEventFactory::CreateRequestFailedEvent(msgFailed, mChannel);
    if ( eventError != NULL )
        eventError->SendEvent();
}

void CEClientService::FailedReceiveMessage(SOCKETHANDLE whichSource)
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_FailedReceiveMessage);
    TRACE_WARN("Failed to receive message from socket [ %u ], current client socket is [ %u ]. Going to stop service",  whichSource, mClientConnection.GetSocketHandle());
    if ( whichSource == mClientConnection.GetSocketHandle() )
    {
        CEClientServiceEvent::SendEvent( CEClientServiceEventData(CEClientServiceEventData::CMD_ServiceLost), static_cast<IEClientServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );
        // CEDispatcherThread::SetExitEvent();
    }
}

void CEClientService::FailedProcessMessage( const CERemoteMessage & msgUnprocessed )
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_FailedProcessMessage);
    TRACE_DBG("The message [ %p ] for target [ %p ] and from source [ %p ] is unprocessed, going to create failed event", msgUnprocessed.GetMessageId(), msgUnprocessed.GetTarget(), msgUnprocessed.GetSource());
    CEStreamableEvent * eventError = CERemoteEventFactory::CreateRequestFailedEvent(msgUnprocessed, mChannel);
    if ( eventError != NULL )
    {
        CERemoteMessage data;
        if ( CERemoteEventFactory::CreateStreamFromEvent( data, *eventError, mChannel) )
            CESendMessageEvent::SendEvent( CESendMessageEventData(data), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend) );
    }
}

void CEClientService::ProcessReceivedMessage( const CERemoteMessage & msgReceived, const NESocket::CEInterlockedValue & addrHost, SOCKETHANDLE whichSource )
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_ProcessReceivedMessage);
    if ( msgReceived.IsValid() && whichSource != NESocket::InvalidSocketHandle )
    {
        NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgReceived.GetMessageId());
        NEMemory::eMessageResult result = static_cast<NEMemory::eMessageResult>(msgReceived.GetResult());
        TRACE_DBG("Processing received message [ %s ] ( ID = %p ), received message is valid, message result [ %s ]", NEService::GetString( msgId), msgId, NEMemory::GetString(result));
        switch ( msgId )
        {
        case NEService::SI_ROUTER_NOTIFY:
            {
                NEService::eServiceConnection connection = NEService::ServiceConnectionUnknown;
                ITEM_ID cookie = NEService::COOKIE_UNKNOWN;
                msgReceived >> connection;
                msgReceived >> cookie;
                TRACE_DBG("Broker connection notification. Connection status [ %s ], cookie [ %p ]", NEService::GetString(connection), cookie);

                if ( (msgReceived.GetResult() == static_cast<unsigned int>(NEMemory::ResultSucceed)) && (connection == NEService::ServiceConnected))
                {
                    CELock lock(mLock);
                    ASSERT(cookie == msgReceived.GetTarget());
                    mClientConnection.SetCookie(cookie);
                    CEClientServiceEvent::SendEvent( CEClientServiceEventData(CEClientServiceEventData::CMD_ServiceStarted), static_cast<IEClientServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );
                }
                else if ( (static_cast<unsigned int>(connection) & static_cast<unsigned int>(NEService::ServiceConnected)) == 0 )
                {
                    CEClientServiceEvent::SendEvent( CEClientServiceEventData(CEClientServiceEventData::CMD_ServiceStopped), static_cast<IEClientServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );
                }
                else
                {
                    ; // ignore
                }
            }
            break;

        case NEService::SI_ROUTER_REGISTER_NOTIFY:
            {
                ASSERT( mClientConnection.GetCookie() == msgReceived.GetTarget() );
                NEService::eServiceRequestType reqType;
                msgReceived >> reqType;
                TRACE_DBG("Service Broker registration notification of type [ %s ]", NEService::GetString(reqType));

                switch ( reqType )
                {
                case NEService::SERVICE_REQUEST_REGISTER_CLIENT:
                    {
                        CEProxyAddress proxy(msgReceived);
                        proxy.SetSource( mChannel.GetSource() );
                        if ( result == NEMemory::ResultSucceed )
                            mServiceConsumer.RegisterRemoteProxy(proxy);
                        else
                            mServiceConsumer.UnregisterRemoteProxy(proxy);
                    }
                    break;

                case NEService::SERVICE_REQUEST_REGISTER_STUB:
                    {
                        CEStubAddress stub(msgReceived);
                        stub.SetSource( mChannel.GetSource() );
                        if ( result == NEMemory::ResultSucceed )
                            mServiceConsumer.RegisterRemoteStub(stub);
                        else
                            mServiceConsumer.UnregisterRemoteStub(stub);
                    }
                    break;

                case NEService::SERVICE_REQUEST_UNREGISTER_CLIENT:
                    {
                        CEProxyAddress proxy(msgReceived);
                        proxy.SetSource( mChannel.GetSource() );
                        mServiceConsumer.UnregisterRemoteProxy(proxy);
                    }
                    break;

                case NEService::SERVICE_REQUEST_UNREGISTER_STUB:
                    {
                        CEStubAddress stub(msgReceived);
                        stub.SetSource( mChannel.GetSource() );
                        mServiceConsumer.UnregisterRemoteStub(stub);
                    }
                    break;

                default:
                    ASSERT(false);
                    break;
                }
            }
            break;

        default:
            {
                if ( NEService::IsExecutableId(msgId) )
                {
                    TRACE_DBG("Processing executable remote message with ID [ %p ]", msgId);
                    CEStreamableEvent * eventRemote = CERemoteEventFactory::CreateEventFromStream(msgReceived, mChannel);
                    if ( eventRemote != NULL )
                        eventRemote->SendEvent();
                    else
                        FailedProcessMessage(msgReceived);
                }
                else
                {
                    TRACE_WARN("The message [ %s ] ( ID = %p )was not processed on client service side", NEService::GetString(msgId), msgId);
                }
            }
            break;
        }
    }
    else
    {
        TRACE_WARN("Invalid message from host [ %s : %u ], ignore processing", addrHost.GetHostAddress().GetBuffer(), addrHost.GetHostPort());
    }
}

void CEClientService::ProcessRemoteEvent( CERemoteRequestEvent & requestEvent)
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_ProcessRemoteRequestEvent);
    TRACE_DBG("Processing request event [ %s ] with message ID [ %p ] of runtime object [ %s ], target stub [ %s ], source proxy [ %s ], request type [ %s ]"
                , CEEvent::GetString( requestEvent.GetEventType() )
                , requestEvent.GetRequestId()
                , requestEvent.GetRuntimeClassName()
                , CEStubAddress::ConvertAddressToPath(requestEvent.GetTargetStub()).GetBuffer()
                , CEProxyAddress::ConvertAddressToPath(requestEvent.GetEventSource()).GetBuffer()
                , NEService::GetString(requestEvent.GetRequestType()) );

    if ( requestEvent.IsEventRemote() )
    {
        CERemoteMessage data;
        if ( CERemoteEventFactory::CreateStreamFromEvent( data, requestEvent, mChannel) )
        {
            TRACE_DBG("Send request, remote message of ID [ %p ] ( %s ) from source [ %u ] to target [ %u ]"
                            , data.GetMessageId()
                            , NEService::GetString( static_cast<NEService::eFuncIdRange>(data.GetMessageId()))
                            , data.GetSource()
                            , data.GetTarget());

            CESendMessageEvent::SendEvent( CESendMessageEventData(data), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend) );
        }
        else
        {
            TRACE_ERR("Failed to create remote request message data with ID [ %p ]", requestEvent.GetRequestId() );
        }
    }
    else
    {
        TRACE_WARN("Request event with ID [ %p ] is not remote, ignoring sending event", requestEvent.GetRequestId());
    }
}

void CEClientService::ProcessRemoteEvent( CERemoteNotifyRequestEvent & requestNotifyEvent )
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_ProcessRemoteNotifyRequestEvent);
    TRACE_DBG("Processing notify request event [ %s ] with message ID [ %p ] of runtime object [ %s ], target stub [ %s ], source proxy [ %s ], request type [ %s ]"
                , CEEvent::GetString( requestNotifyEvent.GetEventType() )
                , requestNotifyEvent.GetRequestId()
                , requestNotifyEvent.GetRuntimeClassName()
                , CEStubAddress::ConvertAddressToPath(requestNotifyEvent.GetTargetStub()).GetBuffer()
                , CEProxyAddress::ConvertAddressToPath(requestNotifyEvent.GetEventSource()).GetBuffer()
                , NEService::GetString(requestNotifyEvent.GetRequestType()) );


    if ( requestNotifyEvent.IsEventRemote() )
    {
        CERemoteMessage data;
        if ( CERemoteEventFactory::CreateStreamFromEvent( data, requestNotifyEvent, mChannel) )
        {
            TRACE_DBG("Send notify request remote message of ID [ %p ] ( %s ) from source [ %p ] to target [ %p ]"
                            , data.GetMessageId()
                            , NEService::GetString( static_cast<NEService::eFuncIdRange>(data.GetMessageId()))
                            , data.GetSource()
                            , data.GetTarget());

            CESendMessageEvent::SendEvent( CESendMessageEventData(data), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend) );
        }
        else
        {
            TRACE_ERR("Failed to create remote notify request message data with ID [ %p ]", requestNotifyEvent.GetRequestId() );
        }
    }
    else
    {
        TRACE_WARN("Notify request event with ID [ %p ] is not remote, ignoring sending event", requestNotifyEvent.GetRequestId());
    }
}


void CEClientService::ProcessRemoteEvent(CERemoteResponseEvent & responseEvent)
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_ProcessRemoteResponseEvent);
    TRACE_DBG("Processing response event [ %s ] with message ID [ %p ] of runtime object [ %s ], target proxy [ %s ], data type [ %s ]"
                , CEEvent::GetString( responseEvent.GetEventType() )
                , responseEvent.GetResponseId()
                , responseEvent.GetRuntimeClassName()
                , CEProxyAddress::ConvertAddressToPath(responseEvent.GetTargetProxy()).GetBuffer()
                , NEService::GetString(responseEvent.GetDataType()) );

    if ( responseEvent.IsEventRemote() )
    {
        CERemoteMessage data;
        if ( CERemoteEventFactory::CreateStreamFromEvent( data, responseEvent, mChannel) )
        {
            TRACE_DBG("Send response remote message of ID [ %p ] ( %s ) from source [ %p ] to target [ %p ]"
                            , data.GetMessageId()
                            , NEService::GetString( static_cast<NEService::eFuncIdRange>(data.GetMessageId()))
                            , data.GetSource()
                            , data.GetTarget());

            CESendMessageEvent::SendEvent( CESendMessageEventData(data), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend) );
        }
        else
        {
            TRACE_ERR("Failed to create remote response message data with ID [ %p ]", responseEvent.GetResponseId() );
        }
    }
    else
    {
        TRACE_WARN("Response event with ID [ %p ] is not remote, ignoring sending event", responseEvent.GetResponseId());
    }
}

bool CEClientService::RunDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_CEClientService_RunDispatcher);
    CEClientServiceEvent::AddListener( static_cast<IEClientServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );
    CEClientServiceEvent::SendEvent( CEClientServiceEventData(CEClientServiceEventData::CMD_StartService), static_cast<IEClientServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );

    setConnectionState(CEClientService::ConnectionStarting);
    bool result = CEDispatcherThread::RunDispatcher();
    setConnectionState(CEClientService::ConnectionStopped);

    CEClientServiceEvent::RemoveListener( static_cast<IEClientServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );

    return result;
}

bool CEClientService::PostEvent(CEEvent & eventElem)
{
    if ( eventElem.IsEventRemote() )
        eventElem.SetEventConsumer( static_cast<IERemoteEventConsumer *>(this) );
    return CEEventDispatcher::PostEvent(eventElem);
}
