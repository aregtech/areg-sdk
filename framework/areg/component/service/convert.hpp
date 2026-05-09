#ifndef AREG_COMPONENT_SERVICE_CONVERT_HPP
#define AREG_COMPONENT_SERVICE_CONVERT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/service/convert.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, areg namespace contains
 *              collection of classes, structures and types defining
 *              Service Interface.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/service/types.hpp"

//////////////////////////////////////////////////////////////////////////
// class areg enumerations string conversion
//////////////////////////////////////////////////////////////////////////

inline constexpr const char* areg::as_string(areg::ResultType value) noexcept
{
    switch (value)
    {
    case    areg::ResultType::Undefined:
        return "areg::ResultType::Undefined";

    case    areg::ResultType::Error:
        return "areg::ResultType::Error";
    case areg::ResultType::Undelivered:
        return "areg::ResultType::Undelivered";
    case    areg::ResultType::NotProcessed:
        return "areg::ResultType::NotProcessed";
    case areg::ResultType::Processed:
        return "areg::ResultType::Processed";

    case areg::ResultType::MessageUndelivered:
        return "areg::ResultType::MessageUndelivered";

    case    areg::ResultType::RequestOK:
        return "areg::ResultType::RequestOK";
    case    areg::ResultType::RequestInvalid:
        return "areg::ResultType::RequestInvalid";
    case    areg::ResultType::RequestError:
        return "areg::ResultType::RequestError";
    case    areg::ResultType::RequestBusy:
        return "areg::ResultType::RequestBusy";
    case    areg::ResultType::RequestCanceled:
        return "areg::ResultType::RequestCanceled";

    case    areg::ResultType::DataOK:
        return "areg::ResultType::DataOK";
    case    areg::ResultType::DataInvalid:
        return "areg::ResultType::DataInvalid";

    case    areg::ResultType::ServiceOK:
        return "areg::ResultType::ServiceOK";
    case    areg::ResultType::ServiceUnavailable:
        return "areg::ResultType::ServiceUnavailable";
    case    areg::ResultType::Invalid:
        return "areg::ResultType::Invalid";
    case    areg::ResultType::ServiceRejected:
        return "areg::ResultType::ServiceRejected";

    default:
        ASSERT(false);
        return "ERR: Undefined areg::ResultType value!";
    }
}

inline constexpr const char* areg::as_string(areg::DataState value) noexcept
{
    switch (value)
    {
    case    areg::DataState::DataIsUndefined:
        return "areg::DataState::DataIsUndefined";

    case    areg::DataState::DataIsOK:
        return "areg::DataState::DataIsOK";

    case    areg::DataState::DataIsInvalid:
        return "areg::DataState::DataIsInvalid";

    case    areg::DataState::DataIsUnavailable:
        return "areg::DataState::DataIsUnavailable";

    case    areg::DataState::DataUnexpectedError:
        return "areg::DataState::DataUnexpectedError";

    default:
        ASSERT(false);
        return "ERR: Undefined areg::DataState value!";
    }
}

inline constexpr const char* areg::as_string( areg::RequestType value ) noexcept
{
    switch (value)
    {
    case    areg::RequestType::Unprocessed:
        return "areg::RequestType::Unprocessed";
    case    areg::RequestType::StartNotify:
        return "areg::RequestType::StartNotify";
    case    areg::RequestType::StopNotify:
        return "areg::RequestType::StopNotify";        
    case    areg::RequestType::RemoveAllNotify:
        return "areg::RequestType::RemoveAllNotify";
    case    areg::RequestType::CallFunction:
        return "areg::RequestType::CallFunction";
    case    areg::RequestType::ServiceConnection:
        return "areg::RequestType::ServiceConnection";
    case    areg::RequestType::ClientConnection:
        return "areg::RequestType::ClientConnection";
    case    areg::RequestType::LoadComponent:
        return "areg::RequestType::LoadComponent";
    default:
        ASSERT(false);
        return "ERR: Undefined areg::RequestType value!";
    }
}

inline constexpr const char* areg::as_string( areg::MessageDataType value ) noexcept
{
    switch (value)
    {
    case areg::MessageDataType::UndefinedData:
        return "areg::MessageDataType::UndefinedData";
    case areg::MessageDataType::RequestData:
        return "areg::MessageDataType::RequestData";
    case areg::MessageDataType::ResponseData:
        return "areg::MessageDataType::ResponseData";
    case areg::MessageDataType::AttributeData:
        return "areg::MessageDataType::AttributeData";
    case areg::MessageDataType::ServiceData:
        return "areg::MessageDataType::ServiceData";

    default:
        ASSERT(false);
        return "ERR: Undefined areg::MessageDataType value!";
    }
}

inline constexpr const char* areg::as_string( areg::ServiceConnectionState value ) noexcept
{
    switch (value)
    {
    case areg::ServiceConnectionState::Unknown:
        return "areg::ServiceConnectionState::Unknown";
    case areg::ServiceConnectionState::Connected:
        return "areg::ServiceConnectionState::Connected";
    case areg::ServiceConnectionState::Pending:
        return "areg::ServiceConnectionState::Pending";
    case areg::ServiceConnectionState::Disconnected:
        return "areg::ServiceConnectionState::Disconnected";
    case areg::ServiceConnectionState::ConnectionLost:
        return "areg::ServiceConnectionState::ConnectionLost";
    case areg::ServiceConnectionState::Rejected:
        return "areg::ServiceConnectionState::Rejected";
    case areg::ServiceConnectionState::Failed:
        return "areg::ServiceConnectionState::Failed";
    case areg::ServiceConnectionState::Shutdown:
        return "areg::ServiceConnectionState::Shutdown";
    default:
        ASSERT(false);
        return "ERR: Undefined areg::ServiceConnectionState value!";
    }
}

inline constexpr const char* areg::as_string( areg::DisconnectReason reason ) noexcept
{
    switch ( reason )
    {
    case areg::DisconnectReason::UndefinedReason:
        return "areg::DisconnectReason::UndefinedReason";
    case areg::DisconnectReason::ServiceDisconnected:
        return "areg::DisconnectReason::ServiceDisconnected";
    case areg::DisconnectReason::ServiceLost:
        return "areg::DisconnectReason::ServiceLost";
    case areg::DisconnectReason::ServiceRejected:
        return "areg::DisconnectReason::ServiceRejected";
    case areg::DisconnectReason::ProviderDisconnected:
        return "areg::DisconnectReason::ProviderDisconnected";
    case areg::DisconnectReason::ProviderLost:
        return "areg::DisconnectReason::ProviderLost";
    case areg::DisconnectReason::ProviderRejected:
        return "areg::DisconnectReason::ProviderRejected";
    case areg::DisconnectReason::ConsumerDisconnected:
        return "areg::DisconnectReason::ConsumerDisconnected";
    case areg::DisconnectReason::ConsumerLost:
        return "areg::DisconnectReason::ConsumerLost";
    case areg::DisconnectReason::ConsumerNotSupported:
        return "areg::DisconnectReason::ConsumerNotSupported";
    case areg::DisconnectReason::SystemShutdown:
        return "areg::DisconnectReason::SystemShutdown";
    case areg::DisconnectReason::ClientConnectionLost:
        return "areg::DisconnectReason::ClientConnectionLost";
    case areg::DisconnectReason::ClientConnectionClosed:
        return "areg::DisconnectReason::ClientConnectionClosed";
    default:
        ASSERT( false );
        return "ERR: Undefined areg::DisconnectReason value!";
    }
}

inline constexpr const char* areg::as_string( areg::RegistrationAction value ) noexcept
{
    switch ( value )
    {
    case areg::RegistrationAction::RegisterConsumer:
        return "areg::RegistrationAction::RegisterConsumer";
    case areg::RegistrationAction::UnregisterConsumer:
        return "areg::RegistrationAction::UnregisterConsumer";
    case areg::RegistrationAction::RegisterProvider:
        return "areg::RegistrationAction::RegisterProvider";
    case areg::RegistrationAction::UnregisterProvider:
        return "areg::RegistrationAction::UnregisterProvider";
    default:
        return "ERR: Unexpected areg::RegistrationAction value!!!";
    }
}

inline constexpr const char* areg::as_string( areg::ServiceType value ) noexcept
{
    switch ( value )
    {
    case areg::ServiceType::Local:
        return "areg::ServiceType::Local";
    case areg::ServiceType::Public:
        return "areg::ServiceType::Public";
    case areg::ServiceType::Internet:
        return "areg::ServiceType::Internet";
    case areg::ServiceType::Any:
        return "areg::ServiceType::Any";
    case areg::ServiceType::Invalid:
        return "areg::ServiceType::Invalid";
    default:
        return "ERR: Unexpected areg::RegistrationAction value!!!";
    }
}

inline constexpr const char* areg::as_string(areg::InstanceBitness bitness) noexcept
{
    switch (bitness)
    {
    case areg::InstanceBitness::Bitness32:
        return "areg::InstanceBitness::Bitness32";
    case areg::InstanceBitness::Bitness64:
        return "areg::InstanceBitness::Bitness64";
    case areg::InstanceBitness::BitnessUnknown:
        return "areg::InstanceBitness::BitnessUnknown";
    default:
        return "ERR: Unexpected areg::InstanceBitness value!!!";
    }
}

inline constexpr const char* areg::as_string(areg::MessageSource value) noexcept
{
    switch (value)
    {
    case areg::MessageSource::SourceUndefined:
        return "areg::MessageSource::SourceUndefined";
    case areg::MessageSource::SourceClient:
        return "areg::MessageSource::SourceClient";
    case areg::MessageSource::SourceService:
        return "areg::MessageSource::SourceService";
    case areg::MessageSource::SourceObserver:
        return "areg::MessageSource::SourceObserver";
    case areg::MessageSource::SourceTest:
        return "areg::MessageSource::SourceTest";
    case areg::MessageSource::SourceSimulation:
        return "areg::MessageSource::SourceSimulation";
    default:
        return "ERR: Unexpected areg::MessageSource value!!!";
    }
}


inline constexpr const char* areg::as_string( areg::FuncIdRange value ) noexcept
{
    switch ( value )
    {
    case areg::FuncIdRange::EmptyFunctionId:
        return "areg::FuncIdRange::EmptyFunctionId";
    case areg::FuncIdRange::ComponentCleanup:
        return "areg::FuncIdRange::ComponentCleanup";
    case areg::FuncIdRange::RequestRegisterService:
        return "areg::FuncIdRange::RequestRegisterService";
    case areg::FuncIdRange::RequestServiceProviderVersion:
        return "areg::FuncIdRange::RequestServiceProviderVersion";
    case areg::FuncIdRange::ResponseServiceProviderVersion:
        return "areg::FuncIdRange::ResponseServiceProviderVersion";
    case areg::FuncIdRange::RequestServiceProviderConnection:
        return "areg::FuncIdRange::RequestServiceProviderConnection";
    case areg::FuncIdRange::ResponseServiceProviderConnection:
        return "areg::FuncIdRange::ResponseServiceProviderConnection";
    case areg::FuncIdRange::SystemServiceConnect:
        return "areg::FuncIdRange::SystemServiceConnect";
    case areg::FuncIdRange::SystemServiceDisconnect:
        return "areg::FuncIdRange::SystemServiceDisconnect";
    case areg::FuncIdRange::SystemServiceNotifyConnection:
        return "areg::FuncIdRange::SystemServiceNotifyConnection";
    case areg::FuncIdRange::SystemServiceQueryInstances:
        return "areg::FuncIdRange::SystemServiceQueryInstances";
    case areg::FuncIdRange::SystemServiceNotifyInstances:
        return "areg::FuncIdRange::SystemServiceNotifyInstances";
    case areg::FuncIdRange::SystemServiceRequestRegister:
        return "areg::FuncIdRange::SystemServiceRequestRegister";
    case areg::FuncIdRange::SystemServiceNotifyRegister:
        return "areg::FuncIdRange::SystemServiceNotifyRegister";
    case areg::FuncIdRange::ServiceLogRegisterScopes:
        return "areg::FuncIdRange::ServiceLogRegisterScopes";
    case areg::FuncIdRange::ServiceLogUpdateScopes:
        return "areg::FuncIdRange::ServiceLogUpdateScopes";
    case areg::FuncIdRange::ServiceLogQueryScopes:
        return "areg::FuncIdRange::ServiceLogQueryScopes";
    case areg::FuncIdRange::ServiceLogScopesUpdated:
        return "areg::FuncIdRange::ServiceLogScopesUpdated";
    case areg::FuncIdRange::ServiceSaveLogConfiguration:
        return "areg::FuncIdRange::ServiceSaveLogConfiguration";
    case areg::FuncIdRange::ServiceLogConfigurationSaved:
        return "areg::FuncIdRange::ServiceLogConfigurationSaved";
    case areg::FuncIdRange::ServiceLogMessage:
        return "areg::FuncIdRange::ServiceLogMessage";
    case areg::FuncIdRange::RequestFirstId:
        return "areg::FuncIdRange::RequestFirstId";
    case areg::FuncIdRange::ResponseFirstId:
        return "areg::FuncIdRange::ResponseFirstId";
    case areg::FuncIdRange::AttributeFirstId:
        return "areg::FuncIdRange::AttributeFirstId";
    case areg::FuncIdRange::RequestLastId:
        return "areg::FuncIdRange::RequestLastId";
    case areg::FuncIdRange::ResponseLastId:
        return "areg::FuncIdRange::ResponseLastId";
    case areg::FuncIdRange::AttributeLastId:
        return "areg::FuncIdRange::AttributeLastId";
    case areg::FuncIdRange::ServiceLastId:
        return "areg::FuncIdRange::ServiceLastId";

    default:
        {
            if ( (static_cast<uint32_t>(value) > static_cast<uint32_t>(areg::FuncIdRange::RequestFirstId)) && (static_cast<uint32_t>(value) < static_cast<uint32_t>(areg::FuncIdRange::RequestLastId)) )
                return "Request ID range";
            else if ( (static_cast<uint32_t>(value) > static_cast<uint32_t>(areg::FuncIdRange::ResponseFirstId)) && (static_cast<uint32_t>(value) < static_cast<uint32_t>(areg::FuncIdRange::ResponseLastId)) )
                return "Response ID range";
            else if ( (static_cast<uint32_t>(value) > static_cast<uint32_t>(areg::FuncIdRange::AttributeFirstId)) && (static_cast<uint32_t>(value) < static_cast<uint32_t>(areg::FuncIdRange::AttributeLastId)) )
                return "Attribute ID range";
            else if ( (static_cast<uint32_t>(value) > static_cast<uint32_t>(areg::FuncIdRange::RequestRegisterService)) && (static_cast<uint32_t>(value) < static_cast<uint32_t>(areg::FuncIdRange::ServiceLastId)) )
                return "Service registration ID";
            else
                return "ERR: Unexpected ID";
        }
    }
}

#endif // AREG_COMPONENT_SERVICE_CONVERT_HPP
