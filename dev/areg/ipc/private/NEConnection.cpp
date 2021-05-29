/************************************************************************
 * \file        areg/ipc/private/NEConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Connection constants.
 ************************************************************************/

#include "areg/ipc/private/NEConnection.hpp"
#include "areg/component/NEService.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/base/RemoteMessage.hpp"

AREG_API const NEMemory::sRemoteMessage     NEConnection::MessageHelloServer       =
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
    , {static_cast<char>(0)}                            // rbData
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

inline static bool _isValidRouterCookie( ITEM_ID router )
{
    return (router == NEService::COOKIE_ROUTER);
}

inline static void _createRegisterRequest( RemoteMessage & out_msgRegister, ITEM_ID source, NEService::eServiceRequestType reqType, const StubAddress & addrService )
{
    if ( out_msgRegister.initMessage( NEConnection::MessageRegisterService.rbHeader ) != NULL )
    {
        out_msgRegister.setSource(source);
        out_msgRegister.setSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgRegister << reqType;
        out_msgRegister << addrService;
        out_msgRegister.bufferCompletionFix();
    }

}

inline static void _createRegisterRequest( RemoteMessage & out_msgRegister, ITEM_ID source, NEService::eServiceRequestType reqType, const ProxyAddress & addrService )
{
    if ( out_msgRegister.initMessage( NEConnection::MessageRegisterService.rbHeader ) != NULL )
    {
        out_msgRegister.setSource(source);
        out_msgRegister.setSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgRegister << reqType;
        out_msgRegister << addrService;
        out_msgRegister.bufferCompletionFix();
    }
}

inline static void _createRegistereNotify( RemoteMessage & out_msgNotify, ITEM_ID target, NEService::eServiceRequestType reqType, const StubAddress & addrService )
{
    if ( out_msgNotify.initMessage( NEConnection::MessageRegisterNotify.rbHeader ) != NULL )
    {
        out_msgNotify.setTarget(target);
        out_msgNotify.setSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgNotify << reqType;
        out_msgNotify << addrService;
        out_msgNotify.bufferCompletionFix();
    }
}

inline static void _createRegistereNotify( RemoteMessage & out_msgNotify, ITEM_ID target, NEService::eServiceRequestType reqType, const ProxyAddress & addrService )
{
    if ( out_msgNotify.initMessage( NEConnection::MessageRegisterNotify.rbHeader ) != NULL )
    {
        out_msgNotify.setTarget(target);
        out_msgNotify.setSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgNotify << reqType;
        out_msgNotify << addrService;
        out_msgNotify.bufferCompletionFix();
    }
}

RemoteMessage NEConnection::createRouterRegisterService( const StubAddress & stub, ITEM_ID source )
{
    RemoteMessage msgResult;
    if ( stub.isServiceRemote() && _isValidSource(source) )
    {
        StubAddress temp( stub );
        temp.SetCookie(source);
        _createRegisterRequest(msgResult, source, NEService::SERVICE_REQUEST_REGISTER_STUB, temp);
    }
    return msgResult;
}

RemoteMessage NEConnection::createRouterRegisterClient( const ProxyAddress & proxy, ITEM_ID source )
{
    RemoteMessage msgResult;
    if ( proxy.isServiceRemote() && _isValidSource(source) )
    {
        ProxyAddress temp( proxy );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, source, NEService::SERVICE_REQUEST_REGISTER_CLIENT, temp);
    }
    return msgResult;
}

RemoteMessage NEConnection::createRouterUnregisterService( const StubAddress & stub, ITEM_ID source )
{
    RemoteMessage msgResult;
    if ( stub.isServiceRemote() && _isValidSource(source) )
    {
        StubAddress temp( stub );
        temp.SetCookie(source);
        _createRegisterRequest(msgResult, source, NEService::SERVICE_REQUEST_UNREGISTER_STUB, temp);
    }
    return msgResult;
}

RemoteMessage NEConnection::createRouterUnregisterClient( const ProxyAddress & proxy, ITEM_ID source )
{
    RemoteMessage msgResult;
    if ( proxy.isServiceRemote() && _isValidSource(source) )
    {
        ProxyAddress temp( proxy );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, source, NEService::SERVICE_REQUEST_UNREGISTER_CLIENT, temp);
    }
    return msgResult;
}

bool NEConnection::isMessageHelloServer(const RemoteMessage & msgHelloServer)
{
    bool result = false;
    if ( msgHelloServer.isChecksumValid() )
    {
        result = (msgHelloServer.getMessageId() == NEService::SI_ROUTER_CONNECT) && (msgHelloServer.getSource()  == NEService::COOKIE_UNKNOWN);
    }

    return result;
}

bool NEConnection::isMessageByeServer(const RemoteMessage & msgByeServer)
{
    bool result = false;
    if ( msgByeServer.isChecksumValid() )
    {
        result = (msgByeServer.getMessageId() == NEService::SI_ROUTER_DISCONNECT) && (msgByeServer.getSource()  != NEService::COOKIE_UNKNOWN);
    }
    
    return result;
}

bool NEConnection::isMessagNotifyClient(const RemoteMessage & msgNotifyClient)
{
    bool result = false;
    if ( msgNotifyClient.isChecksumValid() )
    {
        result = (msgNotifyClient.getMessageId() == NEService::SI_ROUTER_NOTIFY) && (msgNotifyClient.getSource() == NEService::COOKIE_ROUTER);
    }

    return result;
}

bool NEConnection::isMessageRegisterService(const RemoteMessage & msgRegisterService)
{
    bool result = false;
    if ( msgRegisterService.isChecksumValid() )
    {
        result =(msgRegisterService.getMessageId() == NEService::SI_ROUTER_REGISTER) && 
                (msgRegisterService.getSource() != NEService::COOKIE_UNKNOWN)        && 
                (msgRegisterService.getTarget() == NEService::COOKIE_ROUTER);
    }
    return result;
}

RemoteMessage NEConnection::createServiceRegisteredNotification(const StubAddress & stub, ITEM_ID target)
{
    RemoteMessage msgResult;
    if ( stub.isServiceRemote() && _isValidSource(target) )
    {
        StubAddress temp( stub );
        _createRegistereNotify(msgResult, target, NEService::SERVICE_REQUEST_REGISTER_STUB, temp);
    }
    return msgResult;
}

RemoteMessage NEConnection::createServiceClientRegisteredNotification(const ProxyAddress & proxy, ITEM_ID target)
{
    RemoteMessage msgResult;
    if ( proxy.isServiceRemote() && _isValidSource(target) )
    {
        ProxyAddress temp( proxy );
        _createRegistereNotify(msgResult, target, NEService::SERVICE_REQUEST_REGISTER_CLIENT, temp);
    }
    return msgResult;
}

RemoteMessage NEConnection::createServiceUnregisteredNotification(const StubAddress & stub, ITEM_ID target)
{
    RemoteMessage msgResult;
    if ( stub.isServiceRemote() && _isValidSource(target) )
    {
        StubAddress temp( stub );
        _createRegistereNotify(msgResult, target, NEService::SERVICE_REQUEST_UNREGISTER_STUB, temp);
    }
    return msgResult;
}

RemoteMessage NEConnection::createServiceClientUnregisteredNotification(const ProxyAddress & proxy, ITEM_ID target)
{
    RemoteMessage msgResult;
    if ( proxy.isServiceRemote() && _isValidSource(target) )
    {
        ProxyAddress temp( proxy );
        _createRegistereNotify(msgResult, target, NEService::SERVICE_REQUEST_UNREGISTER_CLIENT, temp);
    }
    return msgResult;
}

RemoteMessage NEConnection::createConnectRequest(void)
{
    RemoteMessage msgHelloServer;
    if ( msgHelloServer.initMessage( NEConnection::MessageHelloServer.rbHeader ) != NULL )
    {
        msgHelloServer.setSource( NEService::SOUR_UNKNOWN );
        msgHelloServer.setSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );

        msgHelloServer.bufferCompletionFix();
    }
    return msgHelloServer;
}

RemoteMessage NEConnection::createDisconnectRequest(ITEM_ID cookie)
{
    RemoteMessage msgBeyServer;
    if ( msgBeyServer.initMessage( NEConnection::MessageByeServer.rbHeader ) != NULL )
    {
        msgBeyServer.setSource( cookie );
        msgBeyServer.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );
        msgBeyServer << cookie;

        msgBeyServer.bufferCompletionFix();
    }
    return msgBeyServer;
}

RemoteMessage NEConnection::createConnectNotify( ITEM_ID cookie  )
{
    RemoteMessage msgNotifyConnect;
    if ( msgNotifyConnect.initMessage( NEConnection::MessageAcceptClient.rbHeader ) != NULL )
    {
        msgNotifyConnect.setTarget( cookie );
        msgNotifyConnect.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyConnect << NEService::ServiceConnected;
        msgNotifyConnect << cookie;

        msgNotifyConnect.bufferCompletionFix();
    }
    return msgNotifyConnect;
}

RemoteMessage NEConnection::createDisconnectNotify(ITEM_ID cookie)
{
    RemoteMessage msgNotifyDisconnect;
    if ( msgNotifyDisconnect.initMessage( NEConnection::MessageByeClient.rbHeader ) != NULL )
    {
        msgNotifyDisconnect.setTarget( cookie );
        msgNotifyDisconnect.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyDisconnect << NEService::ServiceDisconnected;
        msgNotifyDisconnect << cookie;

        msgNotifyDisconnect.bufferCompletionFix();
    }
    return msgNotifyDisconnect;
}

RemoteMessage NEConnection::createRejectNotify(ITEM_ID cookie)
{
    RemoteMessage msgNotifyReject;
    if ( msgNotifyReject.initMessage( NEConnection::MessageRejectClient.rbHeader ) != NULL )
    {
        msgNotifyReject.setTarget( cookie );
        msgNotifyReject.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyReject << NEService::ServiceRejected;
        msgNotifyReject << cookie;

        msgNotifyReject.bufferCompletionFix();
    }
    return msgNotifyReject;
}
