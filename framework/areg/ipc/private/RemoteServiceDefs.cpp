/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/ipc/private/RemoteServiceDefs.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Remote service namespace
 ************************************************************************/
#include "areg/ipc/RemoteServiceDefs.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/Process.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"

namespace {

    inline static bool _isValidSource(const ITEM_ID & client)
    {
        return ((client != areg::COOKIE_UNKNOWN) && client != (areg::COOKIE_LOCAL));
    }

    inline static void _create_register_request( areg::MessageEnvelope & out
                                               , areg::RegistrationAction reqType
                                               , areg::DisconnectReason reason
                                               , const areg::StubAddress & addr)
    {
        static constexpr areg::EventHeader HDR{ areg::message_register_service() };
        areg::EventHeader hdr{ HDR };
        addr.to_endpoint(hdr.rawService, hdr.provider);
        hdr.callType   = static_cast<uint8_t>(reqType);
        hdr.result     = static_cast<uint32_t>(reason);
        out.init_envelope(hdr);
    }

    inline static void _create_register_request( areg::MessageEnvelope & out
                                               , areg::RegistrationAction reqType
                                               , areg::DisconnectReason reason
                                               , const areg::ProxyAddress & addr)
    {
        static constexpr areg::EventHeader HDR{ areg::message_register_service() };
        areg::EventHeader hdr{ HDR };
        addr.to_endpoint(hdr.rawService, hdr.consumer);
        hdr.callType   = static_cast<uint8_t>(reqType);
        hdr.result     = static_cast<uint32_t>(reason);
        out.init_envelope(hdr);
    }

    inline static void _create_register_notify( areg::MessageEnvelope & out
                                              , areg::RegistrationAction reqType
                                              , areg::DisconnectReason reason
                                              , const areg::StubAddress & addr)
    {
        static constexpr areg::EventHeader HDR{ areg::message_register_notify() };
        areg::EventHeader hdr{ HDR };
        addr.to_endpoint(hdr.rawService, hdr.provider);
        hdr.callType   = static_cast<uint8_t>(reqType);
        hdr.result     = static_cast<uint32_t>(reason);
        out.init_envelope(hdr);
    }

    inline static void _create_register_notify( areg::MessageEnvelope & out
                                              , areg::RegistrationAction reqType
                                              , areg::DisconnectReason reason
                                              , const areg::ProxyAddress & addr)
    {
        static constexpr areg::EventHeader HDR{ areg::message_register_notify() };
        areg::EventHeader hdr{ HDR };
        addr.to_endpoint(hdr.rawService, hdr.consumer);
        hdr.callType   = static_cast<uint8_t>(reqType);
        hdr.result     = static_cast<uint32_t>(reason);
        out.init_envelope(hdr);
    }

} // namespace

AREG_API_IMPL areg::MessageEnvelope areg::router_register_service( const areg::StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::MessageEnvelope msgResult;
    if ( stub.is_service_public() && _isValidSource(source) )
    {
        _create_register_request(msgResult, areg::RegistrationAction::RegisterProvider, areg::DisconnectReason::UndefinedReason, stub);
        msgResult.set_source(static_cast<uint32_t>(source));
        msgResult.set_target(static_cast<uint32_t>(target));
    }

    return msgResult;
}

AREG_API_IMPL areg::MessageEnvelope areg::router_register_consumer( const areg::ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target )
{
    areg::MessageEnvelope msgResult;
    if ( proxy.is_service_public() && _isValidSource(source) )
    {
        _create_register_request(msgResult, areg::RegistrationAction::RegisterConsumer, areg::DisconnectReason::UndefinedReason, proxy);
        msgResult.set_source(static_cast<uint32_t>(source));
        msgResult.set_target(static_cast<uint32_t>(target));
    }

    return msgResult;
}

AREG_API_IMPL areg::MessageEnvelope areg::router_unregister_service( const areg::StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::MessageEnvelope msgResult;
    if ( stub.is_service_public() && _isValidSource(source) )
    {
        _create_register_request(msgResult, areg::RegistrationAction::UnregisterProvider, reason, stub);
        msgResult.set_source(static_cast<uint32_t>(source));
        msgResult.set_target(static_cast<uint32_t>(target));
    }

    return msgResult;
}

AREG_API_IMPL areg::MessageEnvelope areg::router_unregister_consumer( const areg::ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::MessageEnvelope msgResult;
    if ( proxy.is_service_public() && _isValidSource(source) )
    {
        _create_register_request(msgResult, areg::RegistrationAction::UnregisterConsumer, reason, proxy);
        msgResult.set_source(static_cast<uint32_t>(source));
        msgResult.set_target(static_cast<uint32_t>(target));
    }

    return msgResult;
}

AREG_API_IMPL bool areg::is_server_hello(const areg::MessageEnvelope & msg)
{
    bool result = false;
    if ( msg.is_checksum_valid() )
    {
        result = (msg.message_id() == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceConnect)) &&
                 (msg.source()    == static_cast<uint32_t>(areg::COOKIE_UNKNOWN));
    }

    return result;
}

AREG_API_IMPL bool areg::is_server_bye(const areg::MessageEnvelope & msg)
{
    bool result = false;
    if ( msg.is_checksum_valid() )
    {
        result = (msg.message_id() == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceDisconnect)) &&
                 (msg.source()     != static_cast<uint32_t>(areg::COOKIE_UNKNOWN));
    }

    return result;
}

AREG_API_IMPL bool areg::is_notify_client(const areg::MessageEnvelope & msg)
{
    bool result = false;
    if ( msg.is_checksum_valid() )
    {
        result = (msg.message_id() == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyConnection)) &&
                 (msg.source()     == static_cast<uint32_t>(areg::COOKIE_ROUTER));
    }

    return result;
}

AREG_API_IMPL bool areg::is_register_message(const areg::MessageEnvelope & msg)
{
    bool result = false;
    if ( msg.is_checksum_valid() )
    {
        result = (msg.message_id() == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceRequestRegister)) &&
                 (msg.source()     != static_cast<uint32_t>(areg::COOKIE_UNKNOWN)) &&
                 (msg.target()     == static_cast<uint32_t>(areg::COOKIE_ROUTER));
    }

    return result;
}

AREG_API_IMPL areg::MessageEnvelope areg::service_registered_event(const StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::MessageEnvelope msgResult;
    if ( stub.is_service_public() && _isValidSource(target) )
    {
        _create_register_notify(msgResult, areg::RegistrationAction::RegisterProvider, areg::DisconnectReason::UndefinedReason, stub);
        msgResult.set_source(static_cast<uint32_t>(source));
        msgResult.set_target(static_cast<uint32_t>(target));
    }

    return msgResult;
}

AREG_API_IMPL areg::MessageEnvelope areg::client_registered_event(const ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::MessageEnvelope msgResult;
    if ( proxy.is_service_public() && _isValidSource(target) )
    {
        _create_register_notify(msgResult, areg::RegistrationAction::RegisterConsumer, areg::DisconnectReason::UndefinedReason, proxy);
        msgResult.set_source(static_cast<uint32_t>(source));
        msgResult.set_target(static_cast<uint32_t>(target));
    }

    return msgResult;
}

AREG_API_IMPL areg::MessageEnvelope areg::service_unregistered_event(const StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::MessageEnvelope msgResult;
    if ( stub.is_service_public() && _isValidSource(target) )
    {
        _create_register_notify(msgResult, areg::RegistrationAction::UnregisterProvider, reason, stub);
        msgResult.set_source(static_cast<uint32_t>(source));
        msgResult.set_target(static_cast<uint32_t>(target));
    }

    return msgResult;
}

AREG_API_IMPL areg::MessageEnvelope areg::client_unregistered_event(const ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::MessageEnvelope msgResult;
    if ( proxy.is_service_public() && _isValidSource(target) )
    {
        _create_register_notify(msgResult, areg::RegistrationAction::UnregisterConsumer, reason, proxy);
        msgResult.set_source(static_cast<uint32_t>(source));
        msgResult.set_target(static_cast<uint32_t>(target));
    }

    return msgResult;
}

AREG_API_IMPL areg::MessageEnvelope areg::create_connect_request(const ITEM_ID & source, const ITEM_ID & target, areg::MessageSource msgSource)
{
    static constexpr areg::EventHeader HDR{ areg::message_hello_server() };
    areg::MessageEnvelope msgHelloServer;
    areg::EventHeader hdr{ HDR };
    hdr.target    = static_cast<uint32_t>(target);
    hdr.source    = static_cast<uint32_t>(areg::SOURCE_UNKNOWN);
    if (msgHelloServer.init_envelope(hdr, sizeof(areg::ConnectedInstance)) != nullptr)
    {
        areg::ConnectedInstance instance{ };
        instance.ciSource   = msgSource;
        instance.ciBitness  = static_cast<areg::InstanceBitness>(Process::instance().bitness());
        instance.ciCookie   = source;
        instance.ciTimestamp= static_cast<TIME64>(DateTime::now());
        instance.ciInstance = Process::instance().name();
        instance.ciLocation = Process::instance().path();

        msgHelloServer << instance;
    }

    return msgHelloServer;
}

AREG_API_IMPL areg::MessageEnvelope areg::create_disconnect_request(const ITEM_ID & source, const ITEM_ID & target)
{
    static constexpr areg::EventHeader HDR{ areg::message_bye_server() };
    areg::MessageEnvelope msgByeServer;
    areg::EventHeader hdr{ HDR };
    hdr.target    = static_cast<uint32_t>(target);
    hdr.source    = static_cast<uint32_t>(source);
    if (msgByeServer.init_envelope(hdr, sizeof(ITEM_ID)) != nullptr)
    {
        msgByeServer << source;
    }

    return msgByeServer;
}

AREG_API_IMPL areg::MessageEnvelope areg::create_connect_notify( const ITEM_ID & source, const ITEM_ID & target )
{
    static constexpr areg::EventHeader HDR{ areg::notify_client_connection() };
    areg::MessageEnvelope msgNotifyConnect;
    areg::EventHeader hdr{ HDR };
    hdr.source    = static_cast<uint32_t>(source);
    hdr.target    = static_cast<uint32_t>(target);
    if (msgNotifyConnect.init_envelope(hdr, sizeof(ITEM_ID) + sizeof(areg::ServiceConnectionState)) != nullptr)
    {
        msgNotifyConnect << target;
        msgNotifyConnect << areg::ServiceConnectionState::Connected;
    }

    return msgNotifyConnect;
}

AREG_API_IMPL areg::MessageEnvelope areg::create_disconnect_notify(const ITEM_ID & source, const ITEM_ID & target)
{
    static constexpr areg::EventHeader HDR{ areg::notify_client_connection() };
    areg::MessageEnvelope msgNotifyDisconnect;
    areg::EventHeader hdr{ HDR };
    hdr.source    = static_cast<uint32_t>(source);
    hdr.target    = static_cast<uint32_t>(target);
    if (msgNotifyDisconnect.init_envelope(hdr, sizeof(ITEM_ID) + sizeof(areg::ServiceConnectionState)) != nullptr)
    {
        msgNotifyDisconnect << target;
        msgNotifyDisconnect << areg::ServiceConnectionState::Disconnected;
    }

    return msgNotifyDisconnect;
}

AREG_API_IMPL areg::MessageEnvelope areg::create_reject_notify(const ITEM_ID & source, const ITEM_ID & target)
{
    static constexpr areg::EventHeader HDR{ areg::notify_client_connection() };
    areg::MessageEnvelope msgNotifyReject;
    areg::EventHeader hdr{ HDR };
    hdr.source    = static_cast<uint32_t>(source);
    hdr.target    = static_cast<uint32_t>(target);
    if (msgNotifyReject.init_envelope(hdr, sizeof(ITEM_ID) + sizeof(areg::ServiceConnectionState)) != nullptr)
    {
        msgNotifyReject << target;
        msgNotifyReject << areg::ServiceConnectionState::Rejected;
    }

    return msgNotifyReject;
}
