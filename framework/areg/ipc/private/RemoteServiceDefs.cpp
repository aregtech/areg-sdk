/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/RemoteServiceDefs.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Remote service namespace
 ************************************************************************/
#include "areg/ipc/RemoteServiceDefs.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"

namespace
{
    inline static bool _isValidSource(const ITEM_ID & client)
    {
        return ((client != NEService::COOKIE_UNKNOWN) && client != (NEService::COOKIE_LOCAL));
    }

    inline static void _createRegisterRequest( RemoteMessage & out_msgRegister
                                             , NEService::RegistrationAction reqType
                                             , NEService::DisconnectReason reason
                                             , const StubAddress & addrService)
    {
        if (out_msgRegister.init_message(NERemoteService::message_register_service().rbHeader) != nullptr)
        {
            out_msgRegister.set_sequence_nr(NEService::SEQUENCE_NUMBER_NOTIFY);
            out_msgRegister << reqType;
            out_msgRegister << addrService;
            out_msgRegister << reason;
        }
    }

    inline static void _createRegisterRequest( RemoteMessage & out_msgRegister
                                             , NEService::RegistrationAction reqType
                                             , NEService::DisconnectReason reason
                                             , const ProxyAddress & addrService)
    {
        if (out_msgRegister.init_message(NERemoteService::message_register_service().rbHeader) != nullptr)
        {
            out_msgRegister.set_sequence_nr(NEService::SEQUENCE_NUMBER_NOTIFY);
            out_msgRegister << reqType;
            out_msgRegister << addrService;
            out_msgRegister << reason;
        }
    }

    inline static void _createRegisterNotify( RemoteMessage & out_msgNotify
                                            , NEService::RegistrationAction reqType
                                            , NEService::DisconnectReason reason
                                            , const StubAddress & addrService)
    {
        if (out_msgNotify.init_message(NERemoteService::message_register_notify().rbHeader) != nullptr)
        {
            out_msgNotify.set_sequence_nr(NEService::SEQUENCE_NUMBER_NOTIFY);
            out_msgNotify << reqType;
            out_msgNotify << addrService;
            out_msgNotify << reason;
        }
    }

    inline static void _createRegisterNotify( RemoteMessage & out_msgNotify
                                            , NEService::RegistrationAction reqType
                                            , NEService::DisconnectReason reason
                                            , const ProxyAddress & addrService)
    {
        if (out_msgNotify.init_message(NERemoteService::message_register_notify().rbHeader) != nullptr)
        {
            out_msgNotify.set_sequence_nr(NEService::SEQUENCE_NUMBER_NOTIFY);
            out_msgNotify << reqType;
            out_msgNotify << addrService;
            out_msgNotify << reason;
        }
    }

}

AREG_API_IMPL const NEMemory::RawMessage & NERemoteService::message_hello_server()
{
    static constexpr NEMemory::RawMessage _messageHelloServer
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(NEMemory::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(NEMemory::MessageHeader)    // biOffset
                , NEMemory::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , NEService::COOKIE_ROUTER                      // rbhTarget
            , NEMemory::INVALID_VALUE                       // rbhChecksum
            , NEMemory::INVALID_VALUE                       // rbhSource
            , static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceConnect) // rbhMessageId
            , NEMemory::MESSAGE_SUCCESS                     // rbhResult
            , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}                            // rbData
    };

    return _messageHelloServer;
}

AREG_API_IMPL const NEMemory::RawMessage & NERemoteService::message_bye_server()
{
    static constexpr NEMemory::RawMessage _messageByeServer
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(NEMemory::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(NEMemory::MessageHeader)    // biOffset
                , NEMemory::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , NEService::COOKIE_ROUTER                      // rbhTarget
            , NEMemory::INVALID_VALUE                       // rbhChecksum
            , NEMemory::INVALID_VALUE                       // rbhSource
            , static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceDisconnect) // rbhMessageId
            , NEMemory::MESSAGE_SUCCESS                     // rbhResult
            , NEService::SEQUENCE_NUMBER_ANY                // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageByeServer;
}

AREG_API_IMPL const NEMemory::RawMessage & NERemoteService::notify_client_connection()
{
    static constexpr NEMemory::RawMessage _messageAcceptClient
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(NEMemory::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(NEMemory::MessageHeader)    // biOffset
                , NEMemory::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , NEMemory::INVALID_VALUE                       // rbhTarget
            , NEMemory::INVALID_VALUE                       // rbhChecksum
            , NEService::COOKIE_ROUTER                      // rbhSource
            , static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceNotifyConnection)   // rbhMessageId
            , NEMemory::MESSAGE_SUCCESS                     // rbhResult
            , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageAcceptClient;
}

AREG_API_IMPL const NEMemory::RawMessage & NERemoteService::message_register_service()
{
    static constexpr NEMemory::RawMessage _messageRegisterService
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(NEMemory::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(NEMemory::MessageHeader)    // biOffset
                , NEMemory::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , NEService::COOKIE_ROUTER                      // rbhTarget
            , NEMemory::INVALID_VALUE                       // rbhChecksum
            , NEMemory::INVALID_VALUE                       // rbhSource
            , static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceRequestRegister) // rbhMessageId
            , NEMemory::MESSAGE_SUCCESS                     // rbhResult
            , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageRegisterService;
}

AREG_API_IMPL const NEMemory::RawMessage & NERemoteService::message_query_instances()
{
    static constexpr NEMemory::RawMessage _messageQueryService
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(NEMemory::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(NEMemory::MessageHeader)    // biOffset
                , NEMemory::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , NEMemory::INVALID_VALUE                       // rbhTarget
            , NEMemory::INVALID_VALUE                       // rbhChecksum
            , NEMemory::INVALID_VALUE                       // rbhSource
            , static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceQueryInstances)    // rbhMessageId
            , NEMemory::MESSAGE_SUCCESS                     // rbhResult
            , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageQueryService;
}


AREG_API_IMPL const NEMemory::RawMessage & NERemoteService::message_notify_instances()
{
    static constexpr NEMemory::RawMessage _messageQueryService
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(NEMemory::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(NEMemory::MessageHeader)    // biOffset
                , NEMemory::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , NEService::COOKIE_ROUTER                      // rbhTarget
            , NEMemory::INVALID_VALUE                       // rbhChecksum
            , NEMemory::INVALID_VALUE                       // rbhSource
            , static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceNotifyInstances)    // rbhMessageId
            , NEMemory::MESSAGE_SUCCESS                     // rbhResult
            , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageQueryService;
}

AREG_API_IMPL const NEMemory::RawMessage & NERemoteService::message_register_notify()
{
    static constexpr NEMemory::RawMessage _messageRegisterNotify
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(NEMemory::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(NEMemory::MessageHeader)    // biOffset
                , NEMemory::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , NEMemory::INVALID_VALUE                       // rbhTarget
            , NEMemory::INVALID_VALUE                       // rbhChecksum
            , NEService::COOKIE_ROUTER                      // rbhSource
            , static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceNotifyRegister)   // rbhMessageId
            , NEMemory::MESSAGE_SUCCESS                     // rbhResult
            , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageRegisterNotify;
}

AREG_API_IMPL RemoteMessage NERemoteService::router_register_service( const StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( stub.is_service_public() && _isValidSource(source) )
    {
        StubAddress temp( stub );
        temp.set_cookie(source);
        _createRegisterRequest(msgResult, NEService::RegistrationAction::RegisterStub, NEService::DisconnectReason::UndefinedReason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NERemoteService::router_register_client( const ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target )
{
    RemoteMessage msgResult;
    if ( proxy.is_service_public() && _isValidSource(source) )
    {
        ProxyAddress temp( proxy );
        temp.set_cookie(source);
        _createRegisterRequest(msgResult, NEService::RegistrationAction::RegisterClient, NEService::DisconnectReason::UndefinedReason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NERemoteService::router_unregister_service( const StubAddress & stub, NEService::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( stub.is_service_public() && _isValidSource(source) )
    {
        StubAddress temp( stub );
        temp.set_cookie(source);
        _createRegisterRequest(msgResult, NEService::RegistrationAction::UnregisterStub, reason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NERemoteService::router_unregister_client( const ProxyAddress & proxy, NEService::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( proxy.is_service_public() && _isValidSource(source) )
    {
        ProxyAddress temp( proxy );
        temp.set_cookie(source);
        _createRegisterRequest(msgResult, NEService::RegistrationAction::UnregisterClient, reason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL bool NERemoteService::is_server_hello(const RemoteMessage & msgHelloServer)
{
    bool result = false;
    if ( msgHelloServer.is_checksum_valid() )
    {
        result = (msgHelloServer.message_id() == static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceConnect)) &&
                 (msgHelloServer.source()    == NEService::COOKIE_UNKNOWN);
    }

    return result;
}

AREG_API_IMPL bool NERemoteService::is_server_bye(const RemoteMessage & msgByeServer)
{
    bool result = false;
    if ( msgByeServer.is_checksum_valid() )
    {
        result = (msgByeServer.message_id()   == static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceDisconnect)) &&
                 (msgByeServer.source()      != NEService::COOKIE_UNKNOWN);
    }

    return result;
}

AREG_API_IMPL bool NERemoteService::is_notify_client(const RemoteMessage & msgNotifyClient)
{
    bool result = false;
    if ( msgNotifyClient.is_checksum_valid() )
    {
        result = (msgNotifyClient.message_id()== static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceNotifyConnection)) &&
                 (msgNotifyClient.source()   == NEService::COOKIE_ROUTER);
    }

    return result;
}

AREG_API_IMPL bool NERemoteService::is_register_message(const RemoteMessage & msgRegisterService)
{
    bool result = false;
    if ( msgRegisterService.is_checksum_valid() )
    {
        result =(msgRegisterService.message_id() == static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceRequestRegister) ) &&
                (msgRegisterService.source()    != NEService::COOKIE_UNKNOWN     ) &&
                (msgRegisterService.target()    == NEService::COOKIE_ROUTER      );
    }

    return result;
}

AREG_API_IMPL RemoteMessage NERemoteService::service_registered_event(const StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( stub.is_service_public() && _isValidSource(target) )
    {
        StubAddress temp( stub );
        _createRegisterNotify(msgResult, NEService::RegistrationAction::RegisterStub, NEService::DisconnectReason::UndefinedReason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NERemoteService::client_registered_event(const ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( proxy.is_service_public() && _isValidSource(target) )
    {
        ProxyAddress temp( proxy );
        _createRegisterNotify(msgResult, NEService::RegistrationAction::RegisterClient, NEService::DisconnectReason::UndefinedReason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NERemoteService::service_unregistered_event(const StubAddress & stub, NEService::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( stub.is_service_public() && _isValidSource(target) )
    {
        StubAddress temp( stub );
        _createRegisterNotify(msgResult, NEService::RegistrationAction::UnregisterStub, reason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NERemoteService::client_unregistered_event(const ProxyAddress & proxy, NEService::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgResult;
    if ( proxy.is_service_public() && _isValidSource(target) )
    {
        ProxyAddress temp( proxy );
        _createRegisterNotify(msgResult, NEService::RegistrationAction::UnregisterClient, reason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL RemoteMessage NERemoteService::create_connect_request(const ITEM_ID & source, const ITEM_ID & target, NEService::MessageSource msgSource)
{
    RemoteMessage msgHelloServer;
    if ( msgHelloServer.init_message( NERemoteService::message_hello_server().rbHeader ) != nullptr )
    {
        msgHelloServer.set_target(target);
        msgHelloServer.set_source( NEService::SOURCE_UNKNOWN );
        msgHelloServer.set_sequence_nr( NEService::SEQUENCE_NUMBER_NOTIFY );
        NEService::ConnectedInstance instance{ };
        instance.ciSource   = msgSource;
        instance.ciBitness  = static_cast<NEService::InstanceBitness>(Process::instance().bitness());
        instance.ciCookie   = source;
        instance.ciTimestamp= static_cast<TIME64>(DateTime::now());
        instance.ciInstance = Process::instance().name();
        // instance.ciInstance = Process::instance().app_name();
        instance.ciLocation = Process::instance().path();

        msgHelloServer << instance;
    }

    return msgHelloServer;
}

AREG_API_IMPL RemoteMessage NERemoteService::create_disconnect_request(const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgByeServer;
    if ( msgByeServer.init_message( NERemoteService::message_bye_server().rbHeader ) != nullptr )
    {
        msgByeServer.set_target(target);
        msgByeServer.set_source(source);
        msgByeServer << source;
    }

    return msgByeServer;
}

AREG_API_IMPL RemoteMessage NERemoteService::create_connect_notify( const ITEM_ID & source, const ITEM_ID & target )
{
    RemoteMessage msgNotifyConnect;
    if ( msgNotifyConnect.init_message( NERemoteService::notify_client_connection().rbHeader ) != nullptr )
    {
        msgNotifyConnect.set_source(source);
        msgNotifyConnect.set_target( target );
        msgNotifyConnect.set_sequence_nr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyConnect << target;
        msgNotifyConnect << NEService::ServiceConnectionState::Connected;
    }

    return msgNotifyConnect;
}

AREG_API_IMPL RemoteMessage NERemoteService::create_disconnect_notify(const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgNotifyDisconnect;
    if ( msgNotifyDisconnect.init_message( NERemoteService::notify_client_connection().rbHeader ) != nullptr )
    {
        msgNotifyDisconnect.set_source(source);
        msgNotifyDisconnect.set_target( target );
        msgNotifyDisconnect.set_sequence_nr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyDisconnect << target;
        msgNotifyDisconnect << NEService::ServiceConnectionState::Disconnected;
    }

    return msgNotifyDisconnect;
}

AREG_API_IMPL RemoteMessage NERemoteService::create_reject_notify(const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgNotifyReject;
    if ( msgNotifyReject.init_message( NERemoteService::notify_client_connection().rbHeader ) != nullptr )
    {
        msgNotifyReject.set_source(source);
        msgNotifyReject.set_target(target);
        msgNotifyReject.set_sequence_nr( NEService::SEQUENCE_NUMBER_ANY );

        msgNotifyReject << target;
        msgNotifyReject << NEService::ServiceConnectionState::Rejected;
    }

    return msgNotifyReject;
}
