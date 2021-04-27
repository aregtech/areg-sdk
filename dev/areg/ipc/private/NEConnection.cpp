/************************************************************************
 * \file        areg/ipc/private/NEConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Connection constants.
 ************************************************************************/

#include "areg/ipc/private/NEConnection.hpp"
#include "areg/component/NEService.hpp"
#include "areg/component/CEStubAddress.hpp"
#include "areg/component/CEProxyAddress.hpp"
#include "areg/base/CERemoteMessage.hpp"

const NEMemory::sRemoteMessage     NEConnection::MessageHelloServer       =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::BufferRemote                    // biBufType
            , 0                                         // biUsed
            , 0                                         // biRefCount
        }
        , NEService::COOKIE_ROUTER                      // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEMemory::INVALID_VALUE                       // rbhSource
        , NEService::SI_ROUTER_CONNECT                  // rbhMessageId
        , NEMemory::ResultSucceed                       // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

const NEMemory::sRemoteMessage     NEConnection::MessageByeServer         =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::BufferRemote                    // biBufType
            , 0                                         // biUsed
            , 0                                         // biRefCount
        }
        , NEService::COOKIE_ROUTER                      // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEMemory::INVALID_VALUE                       // rbhSource
        , NEService::SI_ROUTER_DISCONNECT               // rbhMessageId
        , NEMemory::ResultSucceed                       // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

const NEMemory::sRemoteMessage     NEConnection::MessageAcceptClient      =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::BufferRemote                    // biBufType
            , 0                                         // biUsed
            , 0                                         // biRefCount
        }
        , NEMemory::INVALID_VALUE                       // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEService::COOKIE_ROUTER                      // rbhSource
        , NEService::SI_ROUTER_NOTIFY                   // rbhMessageId
        , NEMemory::ResultSucceed                       // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

const NEMemory::sRemoteMessage     NEConnection::MessageRejectClient      =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::BufferRemote                    // biBufType
            , 0                                         // biUsed
            , 0                                         // biRefCount
        }
        , NEMemory::INVALID_VALUE                       // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEService::COOKIE_ROUTER                      // rbhSource
        , NEService::SI_ROUTER_NOTIFY                   // rbhMessageId
        , NEMemory::ResultSucceed                       // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

const NEMemory::sRemoteMessage     NEConnection::MessageByeClient         =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::BufferRemote                    // biBufType
            , 0                                         // biUsed
            , 0                                         // biRefCount
        }
        , NEMemory::INVALID_VALUE                       // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEService::COOKIE_ROUTER                      // rbhSource
        , NEService::SI_ROUTER_NOTIFY                   // rbhMessageId
        , NEMemory::ResultSucceed                       // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

const NEMemory::sRemoteMessage     NEConnection::MessageRegisterService   =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::BufferRemote                    // biBufType
            , 0                                         // biUsed
            , 0                                         // biRefCount
        }
        , NEService::COOKIE_ROUTER                      // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEMemory::INVALID_VALUE                       // rbhSource
        , NEService::SI_ROUTER_REGISTER                 // rbhMessageId
        , NEMemory::ResultSucceed                       // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

const NEMemory::sRemoteMessage     NEConnection::MessageQueryService      =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::BufferRemote                    // biBufType
            , 0                                         // biUsed
            , 0                                         // biRefCount
        }
        , NEService::COOKIE_ROUTER                      // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEMemory::INVALID_VALUE                       // rbhSource
        , NEService::SI_ROUTER_QUERY                    // rbhMessageId
        , NEMemory::ResultSucceed                       // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

const NEMemory::sRemoteMessage     NEConnection::MessageRegisterNotify    =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::BufferRemote                    // biBufType
            , 0                                         // biUsed
            , 0                                         // biRefCount
        }
        , NEMemory::INVALID_VALUE                       // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEService::COOKIE_ROUTER                      // rbhSource
        , NEService::SI_ROUTER_REGISTER_NOTIFY          // rbhMessageId
        , NEMemory::ResultSucceed                       // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

inline static bool _isValidSource( ITEM_ID client )
{
    return ((client != NEService::COOKIE_UNKNOWN) && client != (NEService::COOKIE_LOCAL));
}

inline static bool _isValidBrokerCookie( ITEM_ID broker )
{
    return broker == NEService::COOKIE_ROUTER;
}

inline static void _createRegisterRequest( CERemoteMessage & out_msgRegister, ITEM_ID source, NEService::eServiceRequestType reqType, const CEStubAddress & addrService )
{
    if ( out_msgRegister.InitializeMessage( NEConnection::MessageRegisterService.rbHeader ) != NULL )
    {
        out_msgRegister.SetSource(source);
        out_msgRegister.SetSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgRegister << reqType;
        out_msgRegister << addrService;
        out_msgRegister.BufferCompletionFix();
    }
}

inline static void _createRegisterRequest( CERemoteMessage & out_msgRegister, ITEM_ID source, NEService::eServiceRequestType reqType, const CEProxyAddress & addrService )
{
    if ( out_msgRegister.InitializeMessage( NEConnection::MessageRegisterService.rbHeader ) != NULL )
    {
        out_msgRegister.SetSource(source);
        out_msgRegister.SetSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgRegister << reqType;
        out_msgRegister << addrService;
        out_msgRegister.BufferCompletionFix();
    }
}

inline static void _createRegistereNotify( CERemoteMessage & out_msgNotify, ITEM_ID target, NEService::eServiceRequestType reqType, const CEStubAddress & addrService )
{
    if ( out_msgNotify.InitializeMessage( NEConnection::MessageRegisterNotify.rbHeader ) != NULL )
    {
        out_msgNotify.SetTarget(target);
        out_msgNotify.SetSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgNotify << reqType;
        out_msgNotify << addrService;
        out_msgNotify.BufferCompletionFix();
    }
}

inline static void _createRegistereNotify( CERemoteMessage & out_msgNotify, ITEM_ID target, NEService::eServiceRequestType reqType, const CEProxyAddress & addrService )
{
    if ( out_msgNotify.InitializeMessage( NEConnection::MessageRegisterNotify.rbHeader ) != NULL )
    {
        out_msgNotify.SetTarget(target);
        out_msgNotify.SetSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgNotify << reqType;
        out_msgNotify << addrService;
        out_msgNotify.BufferCompletionFix();
    }
}

CERemoteMessage NEConnection::CreateBrokerRegisterService( const CEStubAddress & stub, ITEM_ID source )
{
    CERemoteMessage msgResult;
    if ( stub.IsServiceRemote() && _isValidSource(source) )
    {
        CEStubAddress temp( stub );
        temp.SetCookie(source);
        _createRegisterRequest(msgResult, source, NEService::SERVICE_REQUEST_REGISTER_STUB, temp);
    }
    return msgResult;
}

CERemoteMessage NEConnection::CreateBrokerRegisterClient( const CEProxyAddress & proxy, ITEM_ID source )
{
    CERemoteMessage msgResult;
    if ( proxy.IsServiceRemote() && _isValidSource(source) )
    {
        CEProxyAddress temp( proxy );
        temp.SetCookie(source);
        _createRegisterRequest(msgResult, source, NEService::SERVICE_REQUEST_REGISTER_CLIENT, temp);
    }
    return msgResult;
}

CERemoteMessage NEConnection::CreateBrokerUnregisterService( const CEStubAddress & stub, ITEM_ID source )
{
    CERemoteMessage msgResult;
    if ( stub.IsServiceRemote() && _isValidSource(source) )
    {
        CEStubAddress temp( stub );
        temp.SetCookie(source);
        _createRegisterRequest(msgResult, source, NEService::SERVICE_REQUEST_UNREGISTER_STUB, temp);
    }
    return msgResult;
}

CERemoteMessage NEConnection::CreateBrokerUnregisterClient( const CEProxyAddress & proxy, ITEM_ID source )
{
    CERemoteMessage msgResult;
    if ( proxy.IsServiceRemote() && _isValidSource(source) )
    {
        CEProxyAddress temp( proxy );
        temp.SetCookie(source);
        _createRegisterRequest(msgResult, source, NEService::SERVICE_REQUEST_UNREGISTER_CLIENT, temp);
    }
    return msgResult;
}

bool NEConnection::IsMessageHelloServer(const CERemoteMessage & msgHelloServer)
{
    bool result = false;
    if ( msgHelloServer.IsChecksumValid() )
    {
        result = (msgHelloServer.GetMessageId() == NEService::SI_ROUTER_CONNECT) && (msgHelloServer.GetSource()  == NEService::COOKIE_UNKNOWN);
    }

    return result;
}

bool NEConnection::IsMessageByeServer(const CERemoteMessage & msgByeServer)
{
    bool result = false;
    if ( msgByeServer.IsChecksumValid() )
    {
        result = (msgByeServer.GetMessageId() == NEService::SI_ROUTER_DISCONNECT) && (msgByeServer.GetSource()  != NEService::COOKIE_UNKNOWN);
    }
    
    return result;
}

bool NEConnection::IsMessagNotifyClient(const CERemoteMessage & msgNotifyClient)
{
    bool result = false;
    if ( msgNotifyClient.IsChecksumValid() )
    {
        result = (msgNotifyClient.GetMessageId() == NEService::SI_ROUTER_NOTIFY) && (msgNotifyClient.GetSource() == NEService::COOKIE_ROUTER);
    }

    return result;
}

bool NEConnection::IsMessageRegisterService(const CERemoteMessage & msgRegisterService)
{
    bool result = false;
    if ( msgRegisterService.IsChecksumValid() )
    {
        result =(msgRegisterService.GetMessageId() == NEService::SI_ROUTER_REGISTER) && 
                (msgRegisterService.GetSource() != NEService::COOKIE_UNKNOWN)        && 
                (msgRegisterService.GetTarget() == NEService::COOKIE_ROUTER);
    }
    return result;
}

CERemoteMessage NEConnection::CreateServiceRegisteredNotification(const CEStubAddress & stub, ITEM_ID target)
{
    CERemoteMessage msgResult;
    if ( stub.IsServiceRemote() && _isValidSource(target) )
    {
        CEStubAddress temp( stub );
        _createRegistereNotify(msgResult, target, NEService::SERVICE_REQUEST_REGISTER_STUB, temp);
    }
    return msgResult;
}

CERemoteMessage NEConnection::CreateServiceClientRegisteredNotification(const CEProxyAddress & proxy, ITEM_ID target)
{
    CERemoteMessage msgResult;
    if ( proxy.IsServiceRemote() && _isValidSource(target) )
    {
        CEProxyAddress temp( proxy );
        _createRegistereNotify(msgResult, target, NEService::SERVICE_REQUEST_REGISTER_CLIENT, temp);
    }
    return msgResult;
}

CERemoteMessage NEConnection::CreateServiceUnregisteredNotification(const CEStubAddress & stub, ITEM_ID target)
{
    CERemoteMessage msgResult;
    if ( stub.IsServiceRemote() && _isValidSource(target) )
    {
        CEStubAddress temp( stub );
        _createRegistereNotify(msgResult, target, NEService::SERVICE_REQUEST_UNREGISTER_STUB, temp);
    }
    return msgResult;
}

CERemoteMessage NEConnection::CreateServiceClientUnregisteredNotification(const CEProxyAddress & proxy, ITEM_ID target)
{
    CERemoteMessage msgResult;
    if ( proxy.IsServiceRemote() && _isValidSource(target) )
    {
        CEProxyAddress temp( proxy );
        _createRegistereNotify(msgResult, target, NEService::SERVICE_REQUEST_UNREGISTER_CLIENT, temp);
    }
    return msgResult;
}

CERemoteMessage NEConnection::CreateConnectRequest(void)
{
    CERemoteMessage msgHelloServer;
    if ( msgHelloServer.InitializeMessage( NEConnection::MessageHelloServer.rbHeader ) != NULL )
    {
        msgHelloServer.SetSource( NEService::SOURCE_UNKNOWN );
        msgHelloServer.SetSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );

        msgHelloServer.BufferCompletionFix();
    }
    return msgHelloServer;
}

CERemoteMessage NEConnection::CreateDisconnectRequest(ITEM_ID cookie)
{
    CERemoteMessage msgBeyServer;
    if ( msgBeyServer.InitializeMessage( NEConnection::MessageByeServer.rbHeader ) != NULL )
    {
        msgBeyServer.SetSource( cookie );
        msgBeyServer.SetSequenceNr( NEService::SEQUENCE_NUMBER_ANY );
        msgBeyServer << cookie;

        msgBeyServer.BufferCompletionFix();
    }
    return msgBeyServer;
}

CERemoteMessage NEConnection::CreateConnectNotify( ITEM_ID cookie  )
{
    CERemoteMessage msgNotifyConnect;
    if ( msgNotifyConnect.InitializeMessage( NEConnection::MessageAcceptClient.rbHeader ) != NULL )
    {
        msgNotifyConnect.SetTarget( cookie );
        msgNotifyConnect.SetSequenceNr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyConnect << NEService::ServiceConnected;
        msgNotifyConnect << cookie;

        msgNotifyConnect.BufferCompletionFix();
    }
    return msgNotifyConnect;
}

CERemoteMessage NEConnection::CreateDisconnectNotify(ITEM_ID cookie)
{
    CERemoteMessage msgNotifyDisconnect;
    if ( msgNotifyDisconnect.InitializeMessage( NEConnection::MessageByeClient.rbHeader ) != NULL )
    {
        msgNotifyDisconnect.SetTarget( cookie );
        msgNotifyDisconnect.SetSequenceNr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyDisconnect << NEService::ServiceDisconnected;
        msgNotifyDisconnect << cookie;

        msgNotifyDisconnect.BufferCompletionFix();
    }
    return msgNotifyDisconnect;
}

CERemoteMessage NEConnection::CreateRejectNotify(ITEM_ID cookie)
{
    CERemoteMessage msgNotifyReject;
    if ( msgNotifyReject.InitializeMessage( NEConnection::MessageRejectClient.rbHeader ) != NULL )
    {
        msgNotifyReject.SetTarget( cookie );
        msgNotifyReject.SetSequenceNr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyReject << NEService::ServiceRejected;
        msgNotifyReject << cookie;

        msgNotifyReject.BufferCompletionFix();
    }
    return msgNotifyReject;
}
