/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/NEConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Connection constants.
 ************************************************************************/

#include "areg/ipc/NEConnection.hpp"
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
            , NEMemory::eBufferType::BufferRemote       // biBufType
            , 0                                         // biUsed
        }
        , NEService::COOKIE_ROUTER                      // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEMemory::INVALID_VALUE                       // rbhSource
        , static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterConnect) // rbhMessageId
        , NEMemory::MESSAGE_SUCCESS                     // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}                            // rbData
};

AREG_API const NEMemory::sRemoteMessage     NEConnection::MessageByeServer         =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::eBufferType::BufferRemote       // biBufType
            , 0                                         // biUsed
        }
        , NEService::COOKIE_ROUTER                      // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEMemory::INVALID_VALUE                       // rbhSource
        , static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterDisconnect) // rbhMessageId
        , NEMemory::MESSAGE_SUCCESS                     // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

AREG_API const NEMemory::sRemoteMessage     NEConnection::MessageAcceptClient      =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::eBufferType::BufferRemote       // biBufType
            , 0                                         // biUsed
        }
        , NEMemory::INVALID_VALUE                       // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEService::COOKIE_ROUTER                      // rbhSource
        , static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterNotify)   // rbhMessageId
        , NEMemory::MESSAGE_SUCCESS                     // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

AREG_API const NEMemory::sRemoteMessage     NEConnection::MessageRejectClient      =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::eBufferType::BufferRemote       // biBufType
            , 0                                         // biUsed
        }
        , NEMemory::INVALID_VALUE                       // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEService::COOKIE_ROUTER                      // rbhSource
        , static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterNotify)   // rbhMessageId
        , NEMemory::MESSAGE_SUCCESS                     // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

AREG_API const NEMemory::sRemoteMessage     NEConnection::MessageByeClient         =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::eBufferType::BufferRemote       // biBufType
            , 0                                         // biUsed
        }
        , NEMemory::INVALID_VALUE                       // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEService::COOKIE_ROUTER                      // rbhSource
        , static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterNotify)   // rbhMessageId
        , NEMemory::MESSAGE_SUCCESS                     // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

AREG_API const NEMemory::sRemoteMessage     NEConnection::MessageRegisterService   =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::eBufferType::BufferRemote       // biBufType
            , 0                                         // biUsed
        }
        , NEService::COOKIE_ROUTER                      // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEMemory::INVALID_VALUE                       // rbhSource
        , static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterRegister) // rbhMessageId
        , NEMemory::MESSAGE_SUCCESS                     // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

AREG_API const NEMemory::sRemoteMessage     NEConnection::MessageQueryService      =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::eBufferType::BufferRemote       // biBufType
            , 0                                         // biUsed
        }
        , NEService::COOKIE_ROUTER                      // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEMemory::INVALID_VALUE                       // rbhSource
        , static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterQuery)    // rbhMessageId
        , NEMemory::MESSAGE_SUCCESS                     // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

AREG_API const NEMemory::sRemoteMessage     NEConnection::MessageRegisterNotify    =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage)          // biBufSize
            , sizeof(unsigned char)                     // biLength
            , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
            , NEMemory::eBufferType::BufferRemote       // biBufType
            , 0                                         // biUsed
        }
        , NEMemory::INVALID_VALUE                       // rbhTarget
        , NEMemory::INVALID_VALUE                       // rbhChecksum
        , NEService::COOKIE_ROUTER                      // rbhSource
        , static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterNotifyRegister)   // rbhMessageId
        , NEMemory::MESSAGE_SUCCESS                     // rbhResult
        , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

inline static bool _isValidSource( ITEM_ID client )
{
    return ((client != NEService::COOKIE_UNKNOWN) && client != (NEService::COOKIE_LOCAL));
}

inline static void _createRegisterRequest( RemoteMessage & out_msgRegister, ITEM_ID source, NEService::eServiceRequestType reqType, const StubAddress & addrService )
{
    if ( out_msgRegister.initMessage( NEConnection::MessageRegisterService.rbHeader ) != nullptr )
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
    if ( out_msgRegister.initMessage( NEConnection::MessageRegisterService.rbHeader ) != nullptr )
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
    if ( out_msgNotify.initMessage( NEConnection::MessageRegisterNotify.rbHeader ) != nullptr )
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
    if ( out_msgNotify.initMessage( NEConnection::MessageRegisterNotify.rbHeader ) != nullptr )
    {
        out_msgNotify.setTarget(target);
        out_msgNotify.setSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgNotify << reqType;
        out_msgNotify << addrService;
        out_msgNotify.bufferCompletionFix();
    }
}

AREG_API RemoteMessage NEConnection::createRouterRegisterService( const StubAddress & stub, ITEM_ID source )
{
    RemoteMessage msgResult;
    if ( stub.isServicePublic() && _isValidSource(source) )
    {
        StubAddress temp( stub );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, source, NEService::eServiceRequestType::RegisterStub, temp);
    }

    return msgResult;
}

AREG_API RemoteMessage NEConnection::createRouterRegisterClient( const ProxyAddress & proxy, ITEM_ID source )
{
    RemoteMessage msgResult;
    if ( proxy.isServicePublic() && _isValidSource(source) )
    {
        ProxyAddress temp( proxy );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, source, NEService::eServiceRequestType::RegisterClient, temp);
    }

    return msgResult;
}

AREG_API RemoteMessage NEConnection::createRouterUnregisterService( const StubAddress & stub, ITEM_ID source )
{
    RemoteMessage msgResult;
    if ( stub.isServicePublic() && _isValidSource(source) )
    {
        StubAddress temp( stub );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, source, NEService::eServiceRequestType::UnregisterStub, temp);
    }

    return msgResult;
}

AREG_API RemoteMessage NEConnection::createRouterUnregisterClient( const ProxyAddress & proxy, ITEM_ID source )
{
    RemoteMessage msgResult;
    if ( proxy.isServicePublic() && _isValidSource(source) )
    {
        ProxyAddress temp( proxy );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, source, NEService::eServiceRequestType::UnregisterClient, temp);
    }

    return msgResult;
}

AREG_API bool NEConnection::isMessageHelloServer(const RemoteMessage & msgHelloServer)
{
    bool result = false;
    if ( msgHelloServer.isChecksumValid() )
    {
        result = (msgHelloServer.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterConnect)) &&
                 (msgHelloServer.getSource()    == NEService::COOKIE_UNKNOWN);
    }

    return result;
}

AREG_API bool NEConnection::isMessageByeServer(const RemoteMessage & msgByeServer)
{
    bool result = false;
    if ( msgByeServer.isChecksumValid() )
    {
        result = (msgByeServer.getMessageId()   == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterDisconnect)) &&
                 (msgByeServer.getSource()      != NEService::COOKIE_UNKNOWN);
    }

    return result;
}

AREG_API bool NEConnection::isMessagNotifyClient(const RemoteMessage & msgNotifyClient)
{
    bool result = false;
    if ( msgNotifyClient.isChecksumValid() )
    {
        result = (msgNotifyClient.getMessageId()== static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterNotify)) &&
                 (msgNotifyClient.getSource()   == NEService::COOKIE_ROUTER);
    }

    return result;
}

AREG_API bool NEConnection::isMessageRegisterService(const RemoteMessage & msgRegisterService)
{
    bool result = false;
    if ( msgRegisterService.isChecksumValid() )
    {
        result =(msgRegisterService.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterRegister) ) &&
                (msgRegisterService.getSource()    != NEService::COOKIE_UNKNOWN     ) &&
                (msgRegisterService.getTarget()    == NEService::COOKIE_ROUTER      );
    }

    return result;
}

AREG_API RemoteMessage NEConnection::createServiceRegisteredNotification(const StubAddress & stub, ITEM_ID target)
{
    RemoteMessage msgResult;
    if ( stub.isServicePublic() && _isValidSource(target) )
    {
        StubAddress temp( stub );
        _createRegistereNotify(msgResult, target, NEService::eServiceRequestType::RegisterStub, temp);
    }

    return msgResult;
}

AREG_API RemoteMessage NEConnection::createServiceClientRegisteredNotification(const ProxyAddress & proxy, ITEM_ID target)
{
    RemoteMessage msgResult;
    if ( proxy.isServicePublic() && _isValidSource(target) )
    {
        ProxyAddress temp( proxy );
        _createRegistereNotify(msgResult, target, NEService::eServiceRequestType::RegisterClient, temp);
    }

    return msgResult;
}

AREG_API RemoteMessage NEConnection::createServiceUnregisteredNotification(const StubAddress & stub, ITEM_ID target)
{
    RemoteMessage msgResult;
    if ( stub.isServicePublic() && _isValidSource(target) )
    {
        StubAddress temp( stub );
        _createRegistereNotify(msgResult, target, NEService::eServiceRequestType::UnregisterStub, temp);
    }
    return msgResult;
}

AREG_API RemoteMessage NEConnection::createServiceClientUnregisteredNotification(const ProxyAddress & proxy, ITEM_ID target)
{
    RemoteMessage msgResult;
    if ( proxy.isServicePublic() && _isValidSource(target) )
    {
        ProxyAddress temp( proxy );
        _createRegistereNotify(msgResult, target, NEService::eServiceRequestType::UnregisterClient, temp);
    }

    return msgResult;
}

AREG_API RemoteMessage NEConnection::createConnectRequest(void)
{
    RemoteMessage msgHelloServer;
    if ( msgHelloServer.initMessage( NEConnection::MessageHelloServer.rbHeader ) != nullptr )
    {
        msgHelloServer.setSource( NEService::SOURCE_UNKNOWN );
        msgHelloServer.setSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );

        msgHelloServer.bufferCompletionFix();
    }

    return msgHelloServer;
}

AREG_API RemoteMessage NEConnection::createDisconnectRequest(ITEM_ID cookie)
{
    RemoteMessage msgBeyServer;
    if ( msgBeyServer.initMessage( NEConnection::MessageByeServer.rbHeader ) != nullptr )
    {
        msgBeyServer.setSource( cookie );
        msgBeyServer.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );
        msgBeyServer << cookie;

        msgBeyServer.bufferCompletionFix();
    }

    return msgBeyServer;
}

AREG_API RemoteMessage NEConnection::createConnectNotify( ITEM_ID cookie  )
{
    RemoteMessage msgNotifyConnect;
    if ( msgNotifyConnect.initMessage( NEConnection::MessageAcceptClient.rbHeader ) != nullptr )
    {
        msgNotifyConnect.setTarget( cookie );
        msgNotifyConnect.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyConnect << NEService::eServiceConnection::ServiceConnected;
        msgNotifyConnect << cookie;

        msgNotifyConnect.bufferCompletionFix();
    }

    return msgNotifyConnect;
}

AREG_API RemoteMessage NEConnection::createDisconnectNotify(ITEM_ID cookie)
{
    RemoteMessage msgNotifyDisconnect;
    if ( msgNotifyDisconnect.initMessage( NEConnection::MessageByeClient.rbHeader ) != nullptr )
    {
        msgNotifyDisconnect.setTarget( cookie );
        msgNotifyDisconnect.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyDisconnect << NEService::eServiceConnection::ServiceDisconnected;
        msgNotifyDisconnect << cookie;

        msgNotifyDisconnect.bufferCompletionFix();
    }

    return msgNotifyDisconnect;
}

AREG_API RemoteMessage NEConnection::createRejectNotify(ITEM_ID cookie)
{
    RemoteMessage msgNotifyReject;
    if ( msgNotifyReject.initMessage( NEConnection::MessageRejectClient.rbHeader ) != nullptr )
    {
        msgNotifyReject.setTarget( cookie );
        msgNotifyReject.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyReject << NEService::eServiceConnection::ServiceRejected;
        msgNotifyReject << cookie;

        msgNotifyReject.bufferCompletionFix();
    }

    return msgNotifyReject;
}
