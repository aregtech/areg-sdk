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
        return ((client != areg::COOKIE_UNKNOWN) && client != (areg::COOKIE_LOCAL));
    }

    inline static void _createRegisterRequest( areg::RemoteMessage & out_msgRegister
                                             , areg::RegistrationAction reqType
                                             , areg::DisconnectReason reason
                                             , const areg::StubAddress & addrService)
    {
        if (out_msgRegister.initMessage(areg::getMessageRegisterService().rbHeader) != nullptr)
        {
            out_msgRegister.setSequenceNr(areg::SEQUENCE_NUMBER_NOTIFY);
            out_msgRegister << reqType;
            out_msgRegister << addrService;
            out_msgRegister << reason;
        }
    }

    inline static void _createRegisterRequest( areg::RemoteMessage & out_msgRegister
                                             , areg::RegistrationAction reqType
                                             , areg::DisconnectReason reason
                                             , const ProxyAddress & addrService)
    {
        if (out_msgRegister.initMessage(areg::getMessageRegisterService().rbHeader) != nullptr)
        {
            out_msgRegister.setSequenceNr(areg::SEQUENCE_NUMBER_NOTIFY);
            out_msgRegister << reqType;
            out_msgRegister << addrService;
            out_msgRegister << reason;
        }
    }

    inline static void _createRegisterNotify( areg::RemoteMessage & out_msgNotify
                                            , areg::RegistrationAction reqType
                                            , areg::DisconnectReason reason
                                            , const areg::StubAddress & addrService)
    {
        if (out_msgNotify.initMessage(areg::getMessageRegisterNotify().rbHeader) != nullptr)
        {
            out_msgNotify.setSequenceNr(areg::SEQUENCE_NUMBER_NOTIFY);
            out_msgNotify << reqType;
            out_msgNotify << addrService;
            out_msgNotify << reason;
        }
    }

    inline static void _createRegisterNotify( areg::RemoteMessage & out_msgNotify
                                            , areg::RegistrationAction reqType
                                            , areg::DisconnectReason reason
                                            , const ProxyAddress & addrService)
    {
        if (out_msgNotify.initMessage(areg::getMessageRegisterNotify().rbHeader) != nullptr)
        {
            out_msgNotify.setSequenceNr(areg::SEQUENCE_NUMBER_NOTIFY);
            out_msgNotify << reqType;
            out_msgNotify << addrService;
            out_msgNotify << reason;
        }
    }

}

AREG_API_IMPL const areg::RawMessage & areg::getMessageHelloServer()
{
    static constexpr areg::RawMessage _messageHelloServer
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(areg::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(areg::MessageHeader)    // biOffset
                , areg::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , areg::COOKIE_ROUTER                      // rbhTarget
            , areg::INVALID_VALUE                       // rbhChecksum
            , areg::INVALID_VALUE                       // rbhSource
            , static_cast<uint32_t>(areg::FuncIdRange::SystemServiceConnect) // rbhMessageId
            , areg::MESSAGE_SUCCESS                     // rbhResult
            , areg::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}                            // rbData
    };

    return _messageHelloServer;
}

AREG_API_IMPL const areg::RawMessage & areg::getMessageByeServer()
{
    static constexpr areg::RawMessage _messageByeServer
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(areg::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(areg::MessageHeader)    // biOffset
                , areg::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , areg::COOKIE_ROUTER                      // rbhTarget
            , areg::INVALID_VALUE                       // rbhChecksum
            , areg::INVALID_VALUE                       // rbhSource
            , static_cast<uint32_t>(areg::FuncIdRange::SystemServiceDisconnect) // rbhMessageId
            , areg::MESSAGE_SUCCESS                     // rbhResult
            , areg::SEQUENCE_NUMBER_ANY                // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageByeServer;
}

AREG_API_IMPL const areg::RawMessage & areg::getMessageNotifyClientConnection()
{
    static constexpr areg::RawMessage _messageAcceptClient
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(areg::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(areg::MessageHeader)    // biOffset
                , areg::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , areg::INVALID_VALUE                       // rbhTarget
            , areg::INVALID_VALUE                       // rbhChecksum
            , areg::COOKIE_ROUTER                      // rbhSource
            , static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyConnection)   // rbhMessageId
            , areg::MESSAGE_SUCCESS                     // rbhResult
            , areg::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageAcceptClient;
}

AREG_API_IMPL const areg::RawMessage & areg::getMessageRegisterService()
{
    static constexpr areg::RawMessage _messageRegisterService
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(areg::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(areg::MessageHeader)    // biOffset
                , areg::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , areg::COOKIE_ROUTER                      // rbhTarget
            , areg::INVALID_VALUE                       // rbhChecksum
            , areg::INVALID_VALUE                       // rbhSource
            , static_cast<uint32_t>(areg::FuncIdRange::SystemServiceRequestRegister) // rbhMessageId
            , areg::MESSAGE_SUCCESS                     // rbhResult
            , areg::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageRegisterService;
}

AREG_API_IMPL const areg::RawMessage & areg::getMessageQueryInstances()
{
    static constexpr areg::RawMessage _messageQueryService
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(areg::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(areg::MessageHeader)    // biOffset
                , areg::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , areg::INVALID_VALUE                       // rbhTarget
            , areg::INVALID_VALUE                       // rbhChecksum
            , areg::INVALID_VALUE                       // rbhSource
            , static_cast<uint32_t>(areg::FuncIdRange::SystemServiceQueryInstances)    // rbhMessageId
            , areg::MESSAGE_SUCCESS                     // rbhResult
            , areg::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageQueryService;
}


AREG_API_IMPL const areg::RawMessage & areg::getMessageNotifyInstances()
{
    static constexpr areg::RawMessage _messageQueryService
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(areg::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(areg::MessageHeader)    // biOffset
                , areg::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , areg::COOKIE_ROUTER                      // rbhTarget
            , areg::INVALID_VALUE                       // rbhChecksum
            , areg::INVALID_VALUE                       // rbhSource
            , static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyInstances)    // rbhMessageId
            , areg::MESSAGE_SUCCESS                     // rbhResult
            , areg::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageQueryService;
}

AREG_API_IMPL const areg::RawMessage & areg::getMessageRegisterNotify()
{
    static constexpr areg::RawMessage _messageRegisterNotify
    {
        {
            {   /*rbhBufHeader*/
                  sizeof(areg::RawMessage)          // biBufSize
                , sizeof(uint8_t)                     // biLength
                , sizeof(areg::MessageHeader)    // biOffset
                , areg::BufferType::Remote       // biBufType
                , 0                                         // biUsed
            }
            , areg::INVALID_VALUE                       // rbhTarget
            , areg::INVALID_VALUE                       // rbhChecksum
            , areg::COOKIE_ROUTER                      // rbhSource
            , static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyRegister)   // rbhMessageId
            , areg::MESSAGE_SUCCESS                     // rbhResult
            , areg::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
        }
        , {static_cast<char>(0)}
    };

    return _messageRegisterNotify;
}

AREG_API_IMPL areg::RemoteMessage areg::createRouterRegisterService( const areg::StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( stub.isServicePublic() && _isValidSource(source) )
    {
        areg::StubAddress temp( stub );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, areg::RegistrationAction::RegisterStub, areg::DisconnectReason::UndefinedReason, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::createRouterRegisterClient( const ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target )
{
    areg::RemoteMessage msgResult;
    if ( proxy.isServicePublic() && _isValidSource(source) )
    {
        ProxyAddress temp( proxy );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, areg::RegistrationAction::RegisterClient, areg::DisconnectReason::UndefinedReason, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::createRouterUnregisterService( const areg::StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( stub.isServicePublic() && _isValidSource(source) )
    {
        areg::StubAddress temp( stub );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, areg::RegistrationAction::UnregisterStub, reason, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::createRouterUnregisterClient( const ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( proxy.isServicePublic() && _isValidSource(source) )
    {
        ProxyAddress temp( proxy );
        temp.setCookie(source);
        _createRegisterRequest(msgResult, areg::RegistrationAction::UnregisterClient, reason, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL bool areg::isMessageHelloServer(const areg::RemoteMessage & msgHelloServer)
{
    bool result = false;
    if ( msgHelloServer.isChecksumValid() )
    {
        result = (msgHelloServer.getMessageId() == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceConnect)) &&
                 (msgHelloServer.getSource()    == areg::COOKIE_UNKNOWN);
    }

    return result;
}

AREG_API_IMPL bool areg::isMessageByeServer(const areg::RemoteMessage & msgByeServer)
{
    bool result = false;
    if ( msgByeServer.isChecksumValid() )
    {
        result = (msgByeServer.getMessageId()   == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceDisconnect)) &&
                 (msgByeServer.getSource()      != areg::COOKIE_UNKNOWN);
    }

    return result;
}

AREG_API_IMPL bool areg::isMessagNotifyClient(const areg::RemoteMessage & msgNotifyClient)
{
    bool result = false;
    if ( msgNotifyClient.isChecksumValid() )
    {
        result = (msgNotifyClient.getMessageId()== static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyConnection)) &&
                 (msgNotifyClient.getSource()   == areg::COOKIE_ROUTER);
    }

    return result;
}

AREG_API_IMPL bool areg::isMessageRegisterService(const areg::RemoteMessage & msgRegisterService)
{
    bool result = false;
    if ( msgRegisterService.isChecksumValid() )
    {
        result =(msgRegisterService.getMessageId() == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceRequestRegister) ) &&
                (msgRegisterService.getSource()    != areg::COOKIE_UNKNOWN     ) &&
                (msgRegisterService.getTarget()    == areg::COOKIE_ROUTER      );
    }

    return result;
}

AREG_API_IMPL areg::RemoteMessage areg::createServiceRegisteredNotification(const areg::StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( stub.isServicePublic() && _isValidSource(target) )
    {
        areg::StubAddress temp( stub );
        _createRegisterNotify(msgResult, areg::RegistrationAction::RegisterStub, areg::DisconnectReason::UndefinedReason, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::createServiceClientRegisteredNotification(const ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( proxy.isServicePublic() && _isValidSource(target) )
    {
        ProxyAddress temp( proxy );
        _createRegisterNotify(msgResult, areg::RegistrationAction::RegisterClient, areg::DisconnectReason::UndefinedReason, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::createServiceUnregisteredNotification(const areg::StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( stub.isServicePublic() && _isValidSource(target) )
    {
        areg::StubAddress temp( stub );
        _createRegisterNotify(msgResult, areg::RegistrationAction::UnregisterStub, reason, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::createServiceClientUnregisteredNotification(const ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgResult;
    if ( proxy.isServicePublic() && _isValidSource(target) )
    {
        ProxyAddress temp( proxy );
        _createRegisterNotify(msgResult, areg::RegistrationAction::UnregisterClient, reason, temp);

        msgResult.setSource(source);
        msgResult.setTarget(target);
    }

    return msgResult;
}

AREG_API_IMPL areg::RemoteMessage areg::createConnectRequest(const ITEM_ID & source, const ITEM_ID & target, areg::MessageSource msgSource)
{
    areg::RemoteMessage msgHelloServer;
    if ( msgHelloServer.initMessage( areg::getMessageHelloServer().rbHeader ) != nullptr )
    {
        msgHelloServer.setTarget(target);
        msgHelloServer.setSource( areg::SOURCE_UNKNOWN );
        msgHelloServer.setSequenceNr( areg::SEQUENCE_NUMBER_NOTIFY );
        areg::ConnectedInstance instance{ };
        instance.ciSource   = msgSource;
        instance.ciBitness  = static_cast<areg::InstanceBitness>(areg::Process::getInstance().getBitness());
        instance.ciCookie   = source;
        instance.ciTimestamp= static_cast<TIME64>(areg::DateTime::getNow());
        instance.ciInstance = areg::Process::getInstance().getName();
        // instance.ciInstance = Process::getInstance().getAppName();
        instance.ciLocation = areg::Process::getInstance().getPath();

        msgHelloServer << instance;
    }

    return msgHelloServer;
}

AREG_API_IMPL areg::RemoteMessage areg::createDisconnectRequest(const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgByeServer;
    if ( msgByeServer.initMessage( areg::getMessageByeServer().rbHeader ) != nullptr )
    {
        msgByeServer.setTarget(target);
        msgByeServer.setSource(source);
        msgByeServer << source;
    }

    return msgByeServer;
}

AREG_API_IMPL areg::RemoteMessage areg::createConnectNotify( const ITEM_ID & source, const ITEM_ID & target )
{
    areg::RemoteMessage msgNotifyConnect;
    if ( msgNotifyConnect.initMessage( areg::getMessageNotifyClientConnection().rbHeader ) != nullptr )
    {
        msgNotifyConnect.setSource(source);
        msgNotifyConnect.setTarget( target );
        msgNotifyConnect.setSequenceNr( areg::SEQUENCE_NUMBER_ANY );

        msgNotifyConnect << target;
        msgNotifyConnect << areg::ServiceConnectionState::Connected;
    }

    return msgNotifyConnect;
}

AREG_API_IMPL areg::RemoteMessage areg::createDisconnectNotify(const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgNotifyDisconnect;
    if ( msgNotifyDisconnect.initMessage( areg::getMessageNotifyClientConnection().rbHeader ) != nullptr )
    {
        msgNotifyDisconnect.setSource(source);
        msgNotifyDisconnect.setTarget( target );
        msgNotifyDisconnect.setSequenceNr( areg::SEQUENCE_NUMBER_ANY );

        msgNotifyDisconnect << target;
        msgNotifyDisconnect << areg::ServiceConnectionState::Disconnected;
    }

    return msgNotifyDisconnect;
}

AREG_API_IMPL areg::RemoteMessage areg::createRejectNotify(const ITEM_ID & source, const ITEM_ID & target)
{
    areg::RemoteMessage msgNotifyReject;
    if ( msgNotifyReject.initMessage( areg::getMessageNotifyClientConnection().rbHeader ) != nullptr )
    {
        msgNotifyReject.setSource(source);
        msgNotifyReject.setTarget(target);
        msgNotifyReject.setSequenceNr( areg::SEQUENCE_NUMBER_ANY );

        msgNotifyReject << target;
        msgNotifyReject << areg::ServiceConnectionState::Rejected;
    }

    return msgNotifyReject;
}
