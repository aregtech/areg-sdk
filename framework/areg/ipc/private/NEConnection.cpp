/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/NEConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Connection constants.
 ************************************************************************/
#include "areg/ipc/NEConnection.hpp"

#include "areg/base/Process.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/component/NEService.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"

AREG_API const NEMemory::sRemoteMessage & NEConnection::getMessageHelloServer( void )
{
    static constexpr NEMemory::sRemoteMessage _messageHelloServer
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
            , static_cast<uint32_t>(NEService::eFuncIdRange::SystemServiceConnect) // rbhMessageId
            , NEMemory::MESSAGE_SUCCESS                     // rbhResult
            , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}                            // rbData
    };

    return _messageHelloServer;
}

AREG_API const NEMemory::sRemoteMessage & NEConnection::getMessageByeServer( void )
{
    static constexpr NEMemory::sRemoteMessage _messageByeServer
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
            , static_cast<uint32_t>(NEService::eFuncIdRange::SystemServiceDisconnect) // rbhMessageId
            , NEMemory::MESSAGE_SUCCESS                     // rbhResult
            , NEService::SEQUENCE_NUMBER_ANY                // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageByeServer;
}

AREG_API const NEMemory::sRemoteMessage & NEConnection::getMessageAcceptClient()
{
    static constexpr NEMemory::sRemoteMessage _messageAcceptClient
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

    return _messageAcceptClient;
}

AREG_API const NEMemory::sRemoteMessage & NEConnection::getMessageRejectClient( void )
{
    static constexpr NEMemory::sRemoteMessage _messageRejectClient
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

    return _messageRejectClient;
}


AREG_API const NEMemory::sRemoteMessage & NEConnection::getMessageByeClient( void )
{
    static constexpr NEMemory::sRemoteMessage _messageByeClient
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

    return _messageByeClient;
}

AREG_API const NEMemory::sRemoteMessage & NEConnection::getMessageRegisterService( void )
{
    static constexpr NEMemory::sRemoteMessage _messageRegisterService
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

    return _messageRegisterService;
}

AREG_API const NEMemory::sRemoteMessage & NEConnection::getMessageQueryService( void )
{
    static constexpr NEMemory::sRemoteMessage _messageQueryService
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

    return _messageQueryService;
}

AREG_API const NEMemory::sRemoteMessage & NEConnection::getMessageRegisterNotify( void )
{
    static constexpr NEMemory::sRemoteMessage _messageRegisterNotify
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

    return _messageRegisterNotify;
}

inline static bool _isValidSource(const ITEM_ID & client )
{
    return ((client != NEService::COOKIE_UNKNOWN) && client != (NEService::COOKIE_LOCAL));
}

inline static void _createRegisterRequest( RemoteMessage & out_msgRegister, NEService::eServiceRequestType reqType, NEService::eDisconnectReason reason, const StubAddress & addrService )
{
    if ( out_msgRegister.initMessage( NEConnection::getMessageRegisterService().rbHeader ) != nullptr )
    {
        out_msgRegister.setSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgRegister << reqType;
        out_msgRegister << addrService;
        out_msgRegister << reason;
    }
}

inline static void _createRegisterRequest( RemoteMessage & out_msgRegister, NEService::eServiceRequestType reqType, NEService::eDisconnectReason reason, const ProxyAddress & addrService )
{
    if ( out_msgRegister.initMessage( NEConnection::getMessageRegisterService().rbHeader ) != nullptr )
    {
        out_msgRegister.setSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgRegister << reqType;
        out_msgRegister << addrService;
        out_msgRegister << reason;
    }
}

inline static void _createRegisterNotify( RemoteMessage & out_msgNotify, NEService::eServiceRequestType reqType, NEService::eDisconnectReason reason, const StubAddress & addrService )
{
    if ( out_msgNotify.initMessage( NEConnection::getMessageRegisterNotify().rbHeader ) != nullptr )
    {
        out_msgNotify.setSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgNotify << reqType;
        out_msgNotify << addrService;
        out_msgNotify << reason;
    }
}

inline static void _createRegisterNotify( RemoteMessage & out_msgNotify, NEService::eServiceRequestType reqType, NEService::eDisconnectReason reason, const ProxyAddress & addrService )
{
    if ( out_msgNotify.initMessage( NEConnection::getMessageRegisterNotify().rbHeader ) != nullptr )
    {
        out_msgNotify.setSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        out_msgNotify << reqType;
        out_msgNotify << addrService;
        out_msgNotify << reason;
    }
}

AREG_API_IMPL RemoteMessage NEConnection::createRouterRegisterService( const StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( stub.isServicePublic() && _isValidSource(source) )
    {
        StubAddress temp( stub );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, NEService::eServiceRequestType::RegisterStub, NEService::eDisconnectReason::ReasonUndefined, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NEConnection::createRouterRegisterClient( const ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target )
{
    RemoteMessage msgResult;
    if ( proxy.isServicePublic() && _isValidSource(source) )
    {
        ProxyAddress temp( proxy );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, NEService::eServiceRequestType::RegisterClient, NEService::eDisconnectReason::ReasonUndefined, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NEConnection::createRouterUnregisterService( const StubAddress & stub, NEService::eDisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( stub.isServicePublic() && _isValidSource(source) )
    {
        StubAddress temp( stub );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, NEService::eServiceRequestType::UnregisterStub, reason, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NEConnection::createRouterUnregisterClient( const ProxyAddress & proxy, NEService::eDisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( proxy.isServicePublic() && _isValidSource(source) )
    {
        ProxyAddress temp( proxy );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, NEService::eServiceRequestType::UnregisterClient, reason, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL bool NEConnection::isMessageHelloServer(const RemoteMessage & msgHelloServer)
{
    bool result = false;
    if ( msgHelloServer.isChecksumValid() )
    {
        result = (msgHelloServer.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::SystemServiceConnect)) &&
                 (msgHelloServer.getSource()    == NEService::COOKIE_UNKNOWN);
    }

    return result;
}

AREG_API_IMPL bool NEConnection::isMessageByeServer(const RemoteMessage & msgByeServer)
{
    bool result = false;
    if ( msgByeServer.isChecksumValid() )
    {
        result = (msgByeServer.getMessageId()   == static_cast<uint32_t>(NEService::eFuncIdRange::SystemServiceDisconnect)) &&
                 (msgByeServer.getSource()      != NEService::COOKIE_UNKNOWN);
    }

    return result;
}

AREG_API_IMPL bool NEConnection::isMessagNotifyClient(const RemoteMessage & msgNotifyClient)
{
    bool result = false;
    if ( msgNotifyClient.isChecksumValid() )
    {
        result = (msgNotifyClient.getMessageId()== static_cast<uint32_t>(NEService::eFuncIdRange::ServiceRouterNotify)) &&
                 (msgNotifyClient.getSource()   == NEService::COOKIE_ROUTER);
    }

    return result;
}

AREG_API_IMPL bool NEConnection::isMessageRegisterService(const RemoteMessage & msgRegisterService)
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

AREG_API_IMPL RemoteMessage NEConnection::createServiceRegisteredNotification(const StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( stub.isServicePublic() && _isValidSource(target) )
    {
        StubAddress temp( stub );
        _createRegisterNotify(msgResult, NEService::eServiceRequestType::RegisterStub, NEService::eDisconnectReason::ReasonUndefined, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NEConnection::createServiceClientRegisteredNotification(const ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( proxy.isServicePublic() && _isValidSource(target) )
    {
        ProxyAddress temp( proxy );
        _createRegisterNotify(msgResult, NEService::eServiceRequestType::RegisterClient, NEService::eDisconnectReason::ReasonUndefined, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NEConnection::createServiceUnregisteredNotification(const StubAddress & stub, NEService::eDisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( stub.isServicePublic() && _isValidSource(target) )
    {
        StubAddress temp( stub );
        _createRegisterNotify(msgResult, NEService::eServiceRequestType::UnregisterStub, reason, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NEConnection::createServiceClientUnregisteredNotification(const ProxyAddress & proxy, NEService::eDisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( proxy.isServicePublic() && _isValidSource(target) )
    {
        ProxyAddress temp( proxy );
        _createRegisterNotify(msgResult, NEService::eServiceRequestType::UnregisterClient, reason, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NEConnection::createConnectRequest(const ITEM_ID & target)
{
    RemoteMessage msgHelloServer;
    if ( msgHelloServer.initMessage( NEConnection::getMessageHelloServer().rbHeader ) != nullptr )
    {
        String instance (Process::getInstance( ).getAppName( ));
        msgHelloServer.setTarget(target);
        msgHelloServer.setSource( NEService::SOURCE_UNKNOWN );
        msgHelloServer.setSequenceNr( NEService::SEQUENCE_NUMBER_NOTIFY );
        msgHelloServer << instance;
    }

    return msgHelloServer;
}

AREG_API_IMPL RemoteMessage NEConnection::createDisconnectRequest(const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgByeServer;
    if ( msgByeServer.initMessage( NEConnection::getMessageByeServer().rbHeader ) != nullptr )
    {
        msgByeServer.setTarget(target);
        msgByeServer.setSource(source);
        msgByeServer << source;
    }

    return msgByeServer;
}

AREG_API_IMPL RemoteMessage NEConnection::createConnectNotify( const ITEM_ID & source, const ITEM_ID & target )
{
    RemoteMessage msgNotifyConnect;
    if ( msgNotifyConnect.initMessage( NEConnection::getMessageAcceptClient().rbHeader ) != nullptr )
    {
        msgNotifyConnect.setSource(source);
        msgNotifyConnect.setTarget( target );
        msgNotifyConnect.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyConnect << NEService::eServiceConnection::ServiceConnected;
        msgNotifyConnect << target;
    }

    return msgNotifyConnect;
}

AREG_API_IMPL RemoteMessage NEConnection::createDisconnectNotify(const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgNotifyDisconnect;
    if ( msgNotifyDisconnect.initMessage( NEConnection::getMessageByeClient().rbHeader ) != nullptr )
    {
        msgNotifyDisconnect.setSource(source);
        msgNotifyDisconnect.setTarget( target );
        msgNotifyDisconnect.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyDisconnect << NEService::eServiceConnection::ServiceDisconnected;
        msgNotifyDisconnect << target;
    }

    return msgNotifyDisconnect;
}

AREG_API_IMPL RemoteMessage NEConnection::createRejectNotify(const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgNotifyReject;
    if ( msgNotifyReject.initMessage( NEConnection::getMessageRejectClient().rbHeader ) != nullptr )
    {
        msgNotifyReject.setSource(source);
        msgNotifyReject.setTarget( target );
        msgNotifyReject.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyReject << NEService::eServiceConnection::ServiceRejected;
        msgNotifyReject << target;
    }

    return msgNotifyReject;
}
