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

namespace {
    inline static bool _isValidSource(const ITEM_ID & client)
    {
        return ((client != areg::COOKIE_UNKNOWN) && client != (areg::COOKIE_LOCAL));
    }

    inline static void _createRegisterRequest( areg::RemoteMessage & out_msgRegister
                                             , areg::RegistrationAction reqType
                                             , areg::DisconnectReason reason
                                             , const areg::StubAddress & addrService)
    {
        if (out_msgRegister.init_message(areg::message_register_service().rbHeader) != nullptr)
        {
            out_msgRegister.set_sequence(areg::SEQUENCE_NUMBER_NOTIFY);
            out_msgRegister << reqType;
            out_msgRegister << addrService;
            out_msgRegister << reason;
        }
    }

    inline static void _createRegisterRequest(areg::RemoteMessage & out_msgRegister
                                             , areg::RegistrationAction reqType
                                             , areg::DisconnectReason reason
                                             , const areg::ProxyAddress & addrService)
    {
        if (out_msgRegister.init_message(areg::message_register_service().rbHeader) != nullptr)
        {
            out_msgRegister.set_sequence(areg::SEQUENCE_NUMBER_NOTIFY);
            out_msgRegister << reqType;
            out_msgRegister << addrService;
            out_msgRegister << reason;
        }
    }

    inline static void _createRegisterNotify(areg::RemoteMessage & out_msgNotify
                                            , areg::RegistrationAction reqType
                                            , areg::DisconnectReason reason
                                            , const areg::StubAddress & addrService)
    {
        if (out_msgNotify.init_message(areg::message_register_notify().rbHeader) != nullptr)
        {
            out_msgNotify.set_sequence(areg::SEQUENCE_NUMBER_NOTIFY);
            out_msgNotify << reqType;
            out_msgNotify << addrService;
            out_msgNotify << reason;
        }
    }

    inline static void _createRegisterNotify(areg::RemoteMessage & out_msgNotify
                                            , areg::RegistrationAction reqType
                                            , areg::DisconnectReason reason
                                            , const areg::ProxyAddress & addrService)
    {
        if (out_msgNotify.init_message(areg::message_register_notify().rbHeader) != nullptr)
        {
            out_msgNotify.set_sequence(areg::SEQUENCE_NUMBER_NOTIFY);
            out_msgNotify << reqType;
            out_msgNotify << addrService;
            out_msgNotify << reason;
        }
    }

} // namespace

AREG_API_IMPL areg::RemoteMessage areg::router_register_service( const areg::StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( stub.is_service_public() && _isValidSource(source) )
    {
        StubAddress temp( stub );
        temp.set_cookie(source);
        _createRegisterRequest(msgResult, areg::RegistrationAction::RegisterProvider, areg::DisconnectReason::UndefinedReason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::router_register_client( const areg::ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target )
{
    areg::RemoteMessage msgResult;
    if ( proxy.is_service_public() && _isValidSource(source) )
    {
        ProxyAddress temp( proxy );
        temp.set_cookie(source);
        _createRegisterRequest(msgResult, areg::RegistrationAction::RegisterClient, areg::DisconnectReason::UndefinedReason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::router_unregister_service( const areg::StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( stub.is_service_public() && _isValidSource(source) )
    {
        StubAddress temp( stub );
        temp.set_cookie(source);
        _createRegisterRequest(msgResult, areg::RegistrationAction::UnregisterProvider, reason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::router_unregister_client( const areg::ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( proxy.is_service_public() && _isValidSource(source) )
    {
        ProxyAddress temp( proxy );
        temp.set_cookie(source);
        _createRegisterRequest(msgResult, areg::RegistrationAction::UnregisterClient, reason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL bool areg::is_server_hello(const areg::RemoteMessage & msgHelloServer)
{
    bool result = false;
    if ( msgHelloServer.is_checksum_valid() )
    {
        result = (msgHelloServer.message_id() == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceConnect)) &&
                 (msgHelloServer.source()    == areg::COOKIE_UNKNOWN);
    }

    return result;
}

AREG_API_IMPL bool areg::is_server_bye(const areg::RemoteMessage & msgByeServer)
{
    bool result = false;
    if ( msgByeServer.is_checksum_valid() )
    {
        result = (msgByeServer.message_id()   == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceDisconnect)) &&
                 (msgByeServer.source()      != areg::COOKIE_UNKNOWN);
    }

    return result;
}

AREG_API_IMPL bool areg::is_notify_client(const areg::RemoteMessage & msgNotifyClient)
{
    bool result = false;
    if ( msgNotifyClient.is_checksum_valid() )
    {
        result = (msgNotifyClient.message_id()== static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyConnection)) &&
                 (msgNotifyClient.source()   == areg::COOKIE_ROUTER);
    }

    return result;
}

AREG_API_IMPL bool areg::is_register_message(const areg::RemoteMessage & msgRegisterService)
{
    bool result = false;
    if ( msgRegisterService.is_checksum_valid() )
    {
        result =(msgRegisterService.message_id() == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceRequestRegister) ) &&
                (msgRegisterService.source()    != areg::COOKIE_UNKNOWN     ) &&
                (msgRegisterService.target()    == areg::COOKIE_ROUTER      );
    }

    return result;
}

AREG_API_IMPL areg::RemoteMessage areg::service_registered_event(const StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( stub.is_service_public() && _isValidSource(target) )
    {
        StubAddress temp( stub );
        _createRegisterNotify(msgResult, areg::RegistrationAction::RegisterProvider, areg::DisconnectReason::UndefinedReason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::client_registered_event(const ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( proxy.is_service_public() && _isValidSource(target) )
    {
        ProxyAddress temp( proxy );
        _createRegisterNotify(msgResult, areg::RegistrationAction::RegisterClient, areg::DisconnectReason::UndefinedReason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::service_unregistered_event(const StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( stub.is_service_public() && _isValidSource(target) )
    {
        StubAddress temp( stub );
        _createRegisterNotify(msgResult, areg::RegistrationAction::UnregisterProvider, reason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::client_unregistered_event(const ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( proxy.is_service_public() && _isValidSource(target) )
    {
        ProxyAddress temp( proxy );
        _createRegisterNotify(msgResult, areg::RegistrationAction::UnregisterClient, reason, temp);

        msgResult.set_source(source);
        msgResult.set_target(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::create_connect_request(const ITEM_ID & source, const ITEM_ID & target, areg::MessageSource msgSource)
{
    areg::RemoteMessage msgHelloServer;
    if ( msgHelloServer.init_message( areg::message_hello_server().rbHeader ) != nullptr )
    {
        msgHelloServer.set_target(target);
        msgHelloServer.set_source( areg::SOURCE_UNKNOWN );
        msgHelloServer.set_sequence( areg::SEQUENCE_NUMBER_NOTIFY );
        areg::ConnectedInstance instance{ };
        instance.ciSource   = msgSource;
        instance.ciBitness  = static_cast<areg::InstanceBitness>(Process::instance().bitness());
        instance.ciCookie   = source;
        instance.ciTimestamp= static_cast<TIME64>(DateTime::now());
        instance.ciInstance = Process::instance().name();
        // instance.ciInstance = Process::instance().app_name();
        instance.ciLocation = Process::instance().path();

        msgHelloServer << instance;
    }

    return msgHelloServer;
}

AREG_API_IMPL areg::RemoteMessage areg::create_disconnect_request(const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgByeServer;
    if ( msgByeServer.init_message( areg::message_bye_server().rbHeader ) != nullptr )
    {
        msgByeServer.set_target(target);
        msgByeServer.set_source(source);
        msgByeServer << source;
    }

    return msgByeServer;
}

AREG_API_IMPL areg::RemoteMessage areg::create_connect_notify( const ITEM_ID & source, const ITEM_ID & target )
{
    areg::RemoteMessage msgNotifyConnect;
    if ( msgNotifyConnect.init_message( areg::notify_client_connection().rbHeader ) != nullptr )
    {
        msgNotifyConnect.set_source(source);
        msgNotifyConnect.set_target( target );
        msgNotifyConnect.set_sequence( areg::SEQUENCE_NUMBER_ANY );

        msgNotifyConnect << target;
        msgNotifyConnect << areg::ServiceConnectionState::Connected;
    }

    return msgNotifyConnect;
}

AREG_API_IMPL areg::RemoteMessage areg::create_disconnect_notify(const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgNotifyDisconnect;
    if ( msgNotifyDisconnect.init_message( areg::notify_client_connection().rbHeader ) != nullptr )
    {
        msgNotifyDisconnect.set_source(source);
        msgNotifyDisconnect.set_target( target );
        msgNotifyDisconnect.set_sequence( areg::SEQUENCE_NUMBER_ANY );

        msgNotifyDisconnect << target;
        msgNotifyDisconnect << areg::ServiceConnectionState::Disconnected;
    }

    return msgNotifyDisconnect;
}

AREG_API_IMPL areg::RemoteMessage areg::create_reject_notify(const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgNotifyReject;
    if ( msgNotifyReject.init_message( areg::notify_client_connection().rbHeader ) != nullptr )
    {
        msgNotifyReject.set_source(source);
        msgNotifyReject.set_target(target);
        msgNotifyReject.set_sequence( areg::SEQUENCE_NUMBER_ANY );

        msgNotifyReject << target;
        msgNotifyReject << areg::ServiceConnectionState::Rejected;
    }

    return msgNotifyReject;
}
