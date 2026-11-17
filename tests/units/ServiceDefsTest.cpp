/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ServiceDefsTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the service definitions: enumeration names,
 *              message ID helpers, connection states, state arrays, proxy data and the
 *              headers of the remote service messages.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Version.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"

#include <cstring>
#include <new>
#include <string>

namespace
{
    //!< A pair of an enumerator and its expected name.
    template <typename ENUM>
    struct NameRow
    {
        ENUM        value;
        const char* name;
    };

    //!< Returns true if the text starts with the prefix.
    bool starts_with(const char* text, const char* prefix)
    {
        return (std::strncmp(text, prefix, std::strlen(prefix)) == 0);
    }

    //!< Checks every row of the table.
    template <typename ENUM, size_t N>
    void check_names(const NameRow<ENUM> (&table)[N])
    {
        for (const auto& row : table)
        {
            EXPECT_STREQ(areg::as_string(row.value), row.name);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// 1. Names of the enumerations
//////////////////////////////////////////////////////////////////////////

TEST(ServiceDefsTest, result_type_names)
{
    static const NameRow<areg::ResultType> table[] =
    {
          { areg::ResultType::Undefined,          "areg::ResultType::Undefined" }
        , { areg::ResultType::Error,              "areg::ResultType::Error" }
        , { areg::ResultType::Undelivered,        "areg::ResultType::Undelivered" }
        , { areg::ResultType::NotProcessed,       "areg::ResultType::NotProcessed" }
        , { areg::ResultType::Processed,          "areg::ResultType::Processed" }
        , { areg::ResultType::MessageUndelivered, "areg::ResultType::MessageUndelivered" }
        , { areg::ResultType::RequestOK,          "areg::ResultType::RequestOK" }
        , { areg::ResultType::RequestInvalid,     "areg::ResultType::RequestInvalid" }
        , { areg::ResultType::RequestError,       "areg::ResultType::RequestError" }
        , { areg::ResultType::RequestBusy,        "areg::ResultType::RequestBusy" }
        , { areg::ResultType::RequestCanceled,    "areg::ResultType::RequestCanceled" }
        , { areg::ResultType::RequestUnsupported, "areg::ResultType::RequestUnsupported" }
        , { areg::ResultType::DataOK,             "areg::ResultType::DataOK" }
        , { areg::ResultType::DataInvalid,        "areg::ResultType::DataInvalid" }
        , { areg::ResultType::ServiceOK,          "areg::ResultType::ServiceOK" }
        , { areg::ResultType::ServiceUnavailable, "areg::ResultType::ServiceUnavailable" }
        , { areg::ResultType::Invalid,            "areg::ResultType::Invalid" }
        , { areg::ResultType::ServiceRejected,    "areg::ResultType::ServiceRejected" }
    };

    check_names(table);
    EXPECT_TRUE(starts_with(areg::as_string(static_cast<areg::ResultType>(0x7777u)), "ERR:"));
}

TEST(ServiceDefsTest, data_state_names)
{
    static const NameRow<areg::DataState> table[] =
    {
          { areg::DataState::DataIsUndefined,     "areg::DataState::DataIsUndefined" }
        , { areg::DataState::DataIsOK,            "areg::DataState::DataIsOK" }
        , { areg::DataState::DataIsInvalid,       "areg::DataState::DataIsInvalid" }
        , { areg::DataState::DataIsUnavailable,   "areg::DataState::DataIsUnavailable" }
        , { areg::DataState::DataUnexpectedError, "areg::DataState::DataUnexpectedError" }
    };

    check_names(table);
    EXPECT_TRUE(starts_with(areg::as_string(static_cast<areg::DataState>(0x7777u)), "ERR:"));
}

TEST(ServiceDefsTest, request_type_names)
{
    static const NameRow<areg::RequestType> table[] =
    {
          { areg::RequestType::Unprocessed,       "areg::RequestType::Unprocessed" }
        , { areg::RequestType::StartNotify,       "areg::RequestType::StartNotify" }
        , { areg::RequestType::StopNotify,        "areg::RequestType::StopNotify" }
        , { areg::RequestType::RemoveAllNotify,   "areg::RequestType::RemoveAllNotify" }
        , { areg::RequestType::CallFunction,      "areg::RequestType::CallFunction" }
        , { areg::RequestType::ServiceConnection, "areg::RequestType::ServiceConnection" }
        , { areg::RequestType::ClientConnection,  "areg::RequestType::ClientConnection" }
        , { areg::RequestType::LoadComponent,     "areg::RequestType::LoadComponent" }
    };

    check_names(table);
    EXPECT_TRUE(starts_with(areg::as_string(static_cast<areg::RequestType>(0x7777u)), "ERR:"));
}

TEST(ServiceDefsTest, service_connection_state_names)
{
    static const NameRow<areg::ServiceConnectionState> table[] =
    {
          { areg::ServiceConnectionState::Unknown,        "areg::ServiceConnectionState::Unknown" }
        , { areg::ServiceConnectionState::Connected,      "areg::ServiceConnectionState::Connected" }
        , { areg::ServiceConnectionState::Disconnected,   "areg::ServiceConnectionState::Disconnected" }
        , { areg::ServiceConnectionState::Pending,        "areg::ServiceConnectionState::Pending" }
        , { areg::ServiceConnectionState::ConnectionLost, "areg::ServiceConnectionState::ConnectionLost" }
        , { areg::ServiceConnectionState::Rejected,       "areg::ServiceConnectionState::Rejected" }
        , { areg::ServiceConnectionState::Failed,         "areg::ServiceConnectionState::Failed" }
        , { areg::ServiceConnectionState::Shutdown,       "areg::ServiceConnectionState::Shutdown" }
    };

    check_names(table);
    EXPECT_TRUE(starts_with(areg::as_string(static_cast<areg::ServiceConnectionState>(0x7777u)), "ERR:"));
}

TEST(ServiceDefsTest, disconnect_reason_names)
{
    static const NameRow<areg::DisconnectReason> table[] =
    {
          { areg::DisconnectReason::UndefinedReason,        "areg::DisconnectReason::UndefinedReason" }
        , { areg::DisconnectReason::ServiceDisconnected,    "areg::DisconnectReason::ServiceDisconnected" }
        , { areg::DisconnectReason::ServiceLost,            "areg::DisconnectReason::ServiceLost" }
        , { areg::DisconnectReason::ServiceRejected,        "areg::DisconnectReason::ServiceRejected" }
        , { areg::DisconnectReason::ProviderDisconnected,   "areg::DisconnectReason::ProviderDisconnected" }
        , { areg::DisconnectReason::ProviderLost,           "areg::DisconnectReason::ProviderLost" }
        , { areg::DisconnectReason::ProviderRejected,       "areg::DisconnectReason::ProviderRejected" }
        , { areg::DisconnectReason::ConsumerDisconnected,   "areg::DisconnectReason::ConsumerDisconnected" }
        , { areg::DisconnectReason::ConsumerLost,           "areg::DisconnectReason::ConsumerLost" }
        , { areg::DisconnectReason::ConsumerNotSupported,   "areg::DisconnectReason::ConsumerNotSupported" }
        , { areg::DisconnectReason::SystemShutdown,         "areg::DisconnectReason::SystemShutdown" }
        , { areg::DisconnectReason::ClientConnectionLost,   "areg::DisconnectReason::ClientConnectionLost" }
        , { areg::DisconnectReason::ClientConnectionClosed, "areg::DisconnectReason::ClientConnectionClosed" }
        , { areg::DisconnectReason::ProviderRestarting,     "areg::DisconnectReason::ProviderRestarting" }
    };

    check_names(table);
    EXPECT_TRUE(starts_with(areg::as_string(static_cast<areg::DisconnectReason>(0x7777u)), "ERR:"));
}

TEST(ServiceDefsTest, service_type_names)
{
    static const NameRow<areg::ServiceType> table[] =
    {
          { areg::ServiceType::Invalid,  "areg::ServiceType::Invalid" }
        , { areg::ServiceType::Local,    "areg::ServiceType::Local" }
        , { areg::ServiceType::Public,   "areg::ServiceType::Public" }
        , { areg::ServiceType::Internet, "areg::ServiceType::Internet" }
        , { areg::ServiceType::Any,      "areg::ServiceType::Any" }
    };

    check_names(table);
    EXPECT_TRUE(starts_with(areg::as_string(static_cast<areg::ServiceType>(0x7777u)), "ERR:"));
}

TEST(ServiceDefsTest, message_source_names)
{
    static const NameRow<areg::MessageSource> table[] =
    {
          { areg::MessageSource::SourceUndefined,  "areg::MessageSource::SourceUndefined" }
        , { areg::MessageSource::SourceClient,     "areg::MessageSource::SourceClient" }
        , { areg::MessageSource::SourceService,    "areg::MessageSource::SourceService" }
        , { areg::MessageSource::SourceObserver,   "areg::MessageSource::SourceObserver" }
        , { areg::MessageSource::SourceTest,       "areg::MessageSource::SourceTest" }
        , { areg::MessageSource::SourceSimulation, "areg::MessageSource::SourceSimulation" }
    };

    check_names(table);
    EXPECT_TRUE(starts_with(areg::as_string(static_cast<areg::MessageSource>(0x7777u)), "ERR:"));
}

TEST(ServiceDefsTest, instance_bitness_names)
{
    static const NameRow<areg::InstanceBitness> table[] =
    {
          { areg::InstanceBitness::BitnessUnknown, "areg::InstanceBitness::BitnessUnknown" }
        , { areg::InstanceBitness::Bitness32,      "areg::InstanceBitness::Bitness32" }
        , { areg::InstanceBitness::Bitness64,      "areg::InstanceBitness::Bitness64" }
    };

    check_names(table);
    EXPECT_TRUE(starts_with(areg::as_string(static_cast<areg::InstanceBitness>(7u)), "ERR:"));
}

TEST(ServiceDefsTest, function_id_names)
{
    static const NameRow<areg::FuncIdRange> table[] =
    {
          { areg::FuncIdRange::EmptyFunctionId,                 "areg::FuncIdRange::EmptyFunctionId" }
        , { areg::FuncIdRange::ResponseServiceProviderConnection,"areg::FuncIdRange::ResponseServiceProviderConnection" }
        , { areg::FuncIdRange::SystemServiceConnect,            "areg::FuncIdRange::SystemServiceConnect" }
        , { areg::FuncIdRange::SystemServiceDisconnect,         "areg::FuncIdRange::SystemServiceDisconnect" }
        , { areg::FuncIdRange::SystemServiceNotifyConnection,   "areg::FuncIdRange::SystemServiceNotifyConnection" }
        , { areg::FuncIdRange::SystemServiceQueryInstances,     "areg::FuncIdRange::SystemServiceQueryInstances" }
        , { areg::FuncIdRange::SystemServiceNotifyInstances,    "areg::FuncIdRange::SystemServiceNotifyInstances" }
        , { areg::FuncIdRange::SystemServiceRequestRegister,    "areg::FuncIdRange::SystemServiceRequestRegister" }
        , { areg::FuncIdRange::SystemServiceNotifyRegister,     "areg::FuncIdRange::SystemServiceNotifyRegister" }
        , { areg::FuncIdRange::ServiceLogRegisterScopes,        "areg::FuncIdRange::ServiceLogRegisterScopes" }
        , { areg::FuncIdRange::ServiceLogUpdateScopes,          "areg::FuncIdRange::ServiceLogUpdateScopes" }
        , { areg::FuncIdRange::ServiceLogQueryScopes,           "areg::FuncIdRange::ServiceLogQueryScopes" }
        , { areg::FuncIdRange::ServiceLogScopesUpdated,         "areg::FuncIdRange::ServiceLogScopesUpdated" }
        , { areg::FuncIdRange::ServiceSaveLogConfiguration,     "areg::FuncIdRange::ServiceSaveLogConfiguration" }
        , { areg::FuncIdRange::ServiceLogConfigurationSaved,    "areg::FuncIdRange::ServiceLogConfigurationSaved" }
        , { areg::FuncIdRange::ServiceLogMessage,               "areg::FuncIdRange::ServiceLogMessage" }
        , { areg::FuncIdRange::ServiceLogUpdateSourceState,     "areg::FuncIdRange::ServiceLogUpdateSourceState" }
        , { areg::FuncIdRange::ServiceLogSourceStateUpdated,    "areg::FuncIdRange::ServiceLogSourceStateUpdated" }
        , { areg::FuncIdRange::ServiceLogRestoreConfiguration,  "areg::FuncIdRange::ServiceLogRestoreConfiguration" }
        , { areg::FuncIdRange::ServiceLogConfigurationRestored, "areg::FuncIdRange::ServiceLogConfigurationRestored" }
        , { areg::FuncIdRange::SystemServiceNotifyDuplicate,    "areg::FuncIdRange::SystemServiceNotifyDuplicate" }
    };

    check_names(table);
}

TEST(ServiceDefsTest, function_id_names_of_unnamed_values)
{
    using areg::FuncIdRange;
    EXPECT_TRUE(starts_with(areg::as_string(static_cast<FuncIdRange>(0x0123u)), "ERR:"));
    EXPECT_TRUE(starts_with(areg::as_string(static_cast<FuncIdRange>(0x00010000u)), "ERR:"));
    EXPECT_TRUE(starts_with(areg::as_string(static_cast<FuncIdRange>(areg::INVALID_MESSAGE_ID)), "ERR:"));
}

//////////////////////////////////////////////////////////////////////////
// 2. Message ID values
//////////////////////////////////////////////////////////////////////////

TEST(ServiceDefsTest, system_ids_are_in_the_reserved_range)
{
    EXPECT_EQ(areg::RESPONSE_ID_NONE, 0u);
    EXPECT_EQ(static_cast<uint32_t>(areg::FuncIdRange::EmptyFunctionId), 0u);
    EXPECT_EQ(static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection), 0x8004u);
    for (uint32_t id = static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection); id <= static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyDuplicate); ++id)
    {
        EXPECT_LE(id, areg::SYSTEM_ID_LAST);
        EXPECT_FALSE(areg::is_interface_id(id));
    }
}

TEST(ServiceDefsTest, find_no_message_knows_no_id)
{
    const uint32_t ids[] = { 0u, 1u, 0xFFFFu, 0x10000u, 0x7FFFFFFFu, areg::INVALID_MESSAGE_ID, 0xFFFFFFFFu };
    for (uint32_t id : ids)
    {
        const areg::MessageInfo info{ areg::find_no_message(id) };
        EXPECT_FALSE(info.is_valid());
        EXPECT_EQ(info.miKind, areg::CallType::Undefined);
        EXPECT_EQ(info.miIndex, 0xFFFFu);
        EXPECT_EQ(info.miParams, 0u);
    }

    EXPECT_EQ(areg::empty_interface().idFind, &areg::find_no_message);
}

//////////////////////////////////////////////////////////////////////////
// 3. Connection states
//////////////////////////////////////////////////////////////////////////

TEST(ServiceDefsTest, connection_state_predicates)
{
    using areg::ServiceConnectionState;
    static const ServiceConnectionState all[] =
    {
          ServiceConnectionState::Unknown, ServiceConnectionState::Connected
        , ServiceConnectionState::Disconnected, ServiceConnectionState::Pending
        , ServiceConnectionState::ConnectionLost, ServiceConnectionState::Rejected
        , ServiceConnectionState::Failed, ServiceConnectionState::Shutdown
    };

    for (const auto state : all)
    {
        EXPECT_EQ(areg::is_service_connected(state), state == ServiceConnectionState::Connected);
        EXPECT_EQ(areg::is_connection_pending(state), state == ServiceConnectionState::Pending);
        EXPECT_EQ(areg::is_service_rejected(state), state == ServiceConnectionState::Rejected);
        EXPECT_EQ(areg::is_connection_lost(state), state == ServiceConnectionState::ConnectionLost);
        EXPECT_EQ(areg::is_service_disconnected(state), state != ServiceConnectionState::Connected);
    }
}

TEST(ServiceDefsTest, connection_state_of_disconnect_reason)
{
    using areg::DisconnectReason;
    using areg::ServiceConnectionState;
    static const struct
    {
        DisconnectReason        reason;
        ServiceConnectionState  state;
    } table[] =
    {
          { DisconnectReason::UndefinedReason,        ServiceConnectionState::Unknown }
        , { DisconnectReason::ServiceDisconnected,    ServiceConnectionState::ConnectionLost }
        , { DisconnectReason::ServiceLost,            ServiceConnectionState::ConnectionLost }
        , { DisconnectReason::ProviderLost,           ServiceConnectionState::ConnectionLost }
        , { DisconnectReason::ConsumerLost,           ServiceConnectionState::ConnectionLost }
        , { DisconnectReason::ClientConnectionLost,   ServiceConnectionState::ConnectionLost }
        , { DisconnectReason::ServiceRejected,        ServiceConnectionState::Rejected }
        , { DisconnectReason::ProviderRejected,       ServiceConnectionState::Rejected }
        , { DisconnectReason::ConsumerNotSupported,   ServiceConnectionState::Rejected }
        , { DisconnectReason::ConsumerDisconnected,   ServiceConnectionState::Disconnected }
        , { DisconnectReason::ProviderDisconnected,   ServiceConnectionState::Disconnected }
        , { DisconnectReason::ClientConnectionClosed, ServiceConnectionState::Disconnected }
        , { DisconnectReason::ProviderRestarting,     ServiceConnectionState::Disconnected }
        , { DisconnectReason::SystemShutdown,         ServiceConnectionState::Shutdown }
    };

    for (const auto& row : table)
    {
        EXPECT_EQ(areg::service_connection(row.reason), row.state) << areg::as_string(row.reason);
    }
}

//////////////////////////////////////////////////////////////////////////
// 4. StateArray
//////////////////////////////////////////////////////////////////////////

TEST(ServiceDefsTest, state_array_owns_its_states)
{
    areg::StateArray states(3u);

    EXPECT_EQ(states.size(), 3u);
    EXPECT_TRUE(states.has_params());
    for (uint32_t i = 0; i < states.size(); ++i)
    {
        EXPECT_EQ(states[i], areg::DataState::DataIsUnavailable);
    }

    states.set_state(1, areg::DataState::DataIsOK);
    EXPECT_EQ(states[0u], areg::DataState::DataIsUnavailable);
    EXPECT_EQ(states[1u], areg::DataState::DataIsOK);
    EXPECT_EQ(states[2u], areg::DataState::DataIsUnavailable);

    const areg::StateArray& view = states;
    EXPECT_EQ(view[1u], areg::DataState::DataIsOK);
}

TEST(ServiceDefsTest, state_array_set_all_and_reset)
{
    areg::StateArray states(4u);

    states.set_all_state(areg::DataState::DataIsInvalid);
    for (uint32_t i = 0; i < states.size(); ++i)
    {
        EXPECT_EQ(states[i], areg::DataState::DataIsInvalid);
    }

    states[2u] = areg::DataState::DataIsOK;
    EXPECT_EQ(states[2u], areg::DataState::DataIsOK);

    states.reset();
    for (uint32_t i = 0; i < states.size(); ++i)
    {
        EXPECT_EQ(states[i], areg::DataState::DataIsUnavailable);
    }
}

TEST(ServiceDefsTest, state_array_without_elements_has_no_params)
{
    areg::StateArray states(0u);

    EXPECT_EQ(states.size(), 0u);
    EXPECT_FALSE(states.has_params());
    states.set_all_state(areg::DataState::DataIsOK);
    states.reset();
    EXPECT_EQ(states.size(), 0u);
}

TEST(ServiceDefsTest, state_array_on_external_buffer)
{
    alignas(areg::DataState) uint8_t buffer[4 * sizeof(areg::DataState)];
    std::memset(buffer, 0, sizeof(buffer));
    const areg::DataState* raw = reinterpret_cast<const areg::DataState*>(buffer);

    {
        areg::StateArray states(buffer, 4);
        EXPECT_EQ(states.size(), 4u);
        EXPECT_TRUE(states.has_params());
        for (uint32_t i = 0; i < 4u; ++i)
        {
            EXPECT_EQ(raw[i], areg::DataState::DataIsUnavailable);
        }

        states.set_state(3, areg::DataState::DataIsOK);
        EXPECT_EQ(raw[3], areg::DataState::DataIsOK);
        EXPECT_EQ(states[3u], areg::DataState::DataIsOK);

        states.set_all_state(areg::DataState::DataIsInvalid);
        EXPECT_EQ(raw[0], areg::DataState::DataIsInvalid);
    }

    // The buffer belongs to the caller and is still readable after the array is gone.
    EXPECT_EQ(raw[3], areg::DataState::DataIsInvalid);
}

//////////////////////////////////////////////////////////////////////////
// 5. ParameterArray and ProxyData
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< Request IDs of the test interface.
    constexpr uint32_t REQ_A{ areg::crc32_calculate("request_alpha") };
    constexpr uint32_t REQ_B{ areg::crc32_calculate("request_beta") };
    //!< Response IDs of the test interface; RESP_C is a broadcast.
    constexpr uint32_t RESP_A{ areg::crc32_calculate("response_alpha") };
    constexpr uint32_t RESP_B{ areg::crc32_calculate("response_beta") };
    constexpr uint32_t RESP_C{ areg::crc32_calculate("broadcast_gamma") };
    //!< Attribute IDs of the test interface.
    constexpr uint32_t ATTR_A{ areg::crc32_calculate("attribute_Alpha") };
    constexpr uint32_t ATTR_B{ areg::crc32_calculate("attribute_Beta") };
    //!< An ID the test interface does not have.
    constexpr uint32_t UNKNOWN{ areg::crc32_calculate("request_unknown") };

    const uint32_t gRequestList[]       { REQ_A, REQ_B };
    const uint32_t gResponseList[]      { RESP_A, RESP_B, RESP_C };
    const uint32_t gAttributeList[]     { ATTR_A, ATTR_B };
    const uint32_t gRequestToResponse[] { RESP_A, areg::RESPONSE_ID_NONE };
    const uint32_t gParamCount[]        { 2u, 0u, 1u };

    //!< The lookup of the test interface.
    areg::MessageInfo find_test_message(uint32_t msgId) noexcept
    {
        switch (msgId)
        {
        case REQ_A:     return areg::MessageInfo{ 0u, areg::CallType::Request, 2u };
        case REQ_B:     return areg::MessageInfo{ 1u, areg::CallType::Request, 0u };
        case RESP_A:    return areg::MessageInfo{ 0u, areg::CallType::Response, 2u };
        case RESP_B:    return areg::MessageInfo{ 1u, areg::CallType::Response, 0u };
        case RESP_C:    return areg::MessageInfo{ 2u, areg::CallType::Broadcast, 1u };
        case ATTR_A:    return areg::MessageInfo{ 0u, areg::CallType::Attribute, 1u };
        case ATTR_B:    return areg::MessageInfo{ 1u, areg::CallType::Attribute, 1u };
        default:        return areg::MessageInfo{ };
        }
    }

    //!< The lookup of an interface that reports indexes beyond its lists.
    areg::MessageInfo find_out_of_range(uint32_t msgId) noexcept
    {
        switch (msgId)
        {
        case REQ_A:     return areg::MessageInfo{ 7u, areg::CallType::Request, 0u };
        case RESP_A:    return areg::MessageInfo{ 7u, areg::CallType::Response, 0u };
        case ATTR_A:    return areg::MessageInfo{ 7u, areg::CallType::Attribute, 1u };
        default:        return areg::MessageInfo{ };
        }
    }

    const areg::InterfaceData& test_interface()
    {
        static const areg::InterfaceData ifData =
        {
              "ServiceDefsTestInterface"
            , areg::Version(1, 0, 0)
            , areg::ServiceType::Local
            , 2u, 3u, 2u
            , gRequestList, gResponseList, gAttributeList
            , gRequestToResponse
            , gParamCount
            , &find_test_message
        };

        return ifData;
    }

    const areg::InterfaceData& out_of_range_interface()
    {
        static const areg::InterfaceData ifData =
        {
              "ServiceDefsTestOutOfRange"
            , areg::Version(1, 0, 0)
            , areg::ServiceType::Local
            , 2u, 3u, 2u
            , gRequestList, gResponseList, gAttributeList
            , gRequestToResponse
            , gParamCount
            , &find_out_of_range
        };

        return ifData;
    }
}

TEST(ServiceDefsTest, parameter_array_from_interface_data)
{
    areg::ParameterArray params(test_interface());

    EXPECT_TRUE(params.is_valid_index(0u));
    EXPECT_TRUE(params.is_valid_index(2u));
    EXPECT_FALSE(params.is_valid_index(3u));

    EXPECT_EQ(params[0u].size(), 2u);
    EXPECT_EQ(params[1u].size(), 0u);
    EXPECT_EQ(params[2u].size(), 1u);
    EXPECT_TRUE(params.has_parameters(0u));
    EXPECT_FALSE(params.has_parameters(1u));
    EXPECT_TRUE(params.has_parameters(2u));

    EXPECT_EQ(params.at(0u, 0u), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(params.at(0u, 1u), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(params.at(2u, 0u), areg::DataState::DataIsUnavailable);
}

TEST(ServiceDefsTest, parameter_array_from_count_map)
{
    areg::ParameterArray params(gParamCount, 3);

    EXPECT_EQ(params[0u].size(), 2u);
    EXPECT_EQ(params[1u].size(), 0u);
    EXPECT_EQ(params[2u].size(), 1u);
}

TEST(ServiceDefsTest, parameter_array_state_changes_stay_in_their_row)
{
    areg::ParameterArray params(test_interface());

    params.set_value_at(0u, 1u, areg::DataState::DataIsOK);
    EXPECT_EQ(params.at(0u, 0u), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(params.at(0u, 1u), areg::DataState::DataIsOK);
    EXPECT_EQ(params.at(2u, 0u), areg::DataState::DataIsUnavailable);

    params.set_param_state(2u, areg::DataState::DataIsInvalid);
    EXPECT_EQ(params.at(2u, 0u), areg::DataState::DataIsInvalid);
    EXPECT_EQ(params.at(0u, 1u), areg::DataState::DataIsOK);

    params.reset(0u);
    EXPECT_EQ(params.at(0u, 1u), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(params.at(2u, 0u), areg::DataState::DataIsInvalid);

    params.set_param_state(0u, areg::DataState::DataIsOK);
    params.reset();
    EXPECT_EQ(params.at(0u, 0u), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(params.at(2u, 0u), areg::DataState::DataIsUnavailable);

    const areg::ParameterArray& view = params;
    EXPECT_EQ(view[2u].size(), 1u);
}

TEST(ServiceDefsTest, parameter_array_move_transfers_the_rows)
{
    areg::ParameterArray source(test_interface());
    source.set_value_at(0u, 0u, areg::DataState::DataIsOK);

    areg::ParameterArray moved(std::move(source));
    EXPECT_FALSE(source.is_valid_index(0u));
    ASSERT_TRUE(moved.is_valid_index(2u));
    EXPECT_EQ(moved.at(0u, 0u), areg::DataState::DataIsOK);

    areg::ParameterArray target(gParamCount, 3);
    target = std::move(moved);
    EXPECT_FALSE(moved.is_valid_index(0u));
    ASSERT_TRUE(target.is_valid_index(2u));
    EXPECT_EQ(target.at(0u, 0u), areg::DataState::DataIsOK);
}

TEST(ServiceDefsTest, parameter_array_without_responses_is_empty)
{
    const areg::InterfaceData ifData = areg::empty_interface();
    areg::ParameterArray params(ifData);

    EXPECT_FALSE(params.is_valid_index(0u));
}

TEST(ServiceDefsTest, parameter_array_from_null_map_is_empty)
{
    // The storage is filled with a pattern; a constructor that leaves a member unset shows it.
    alignas(areg::ParameterArray) unsigned char storage[sizeof(areg::ParameterArray)];
    std::memset(storage, 0x11, sizeof(storage));

    areg::ParameterArray* params = new (storage) areg::ParameterArray(nullptr, 0);
    EXPECT_FALSE(params->is_valid_index(0u));
    params->~ParameterArray();
}

TEST(ServiceDefsTest, parameter_array_from_negative_count_is_empty)
{
    const uint32_t map[] = { 1u, 2u };
    areg::ParameterArray params(map, -1);

    EXPECT_FALSE(params.is_valid_index(0u));
}

TEST(ServiceDefsTest, empty_interface_describes_no_calls)
{
    const areg::InterfaceData& ifData = areg::empty_interface();

    EXPECT_STREQ(ifData.idServiceName, areg::EmptyServiceName);
    EXPECT_EQ(ifData.idVersion, areg::EmptyServiceVersion);
    EXPECT_EQ(ifData.idServiceType, areg::ServiceType::Public);
    EXPECT_EQ(ifData.idRequestCount, 0u);
    EXPECT_EQ(ifData.idResponseCount, 0u);
    EXPECT_EQ(ifData.idAttributeCount, 0u);
    EXPECT_EQ(ifData.idRequestList, nullptr);
    EXPECT_EQ(ifData.idResponseList, nullptr);
    EXPECT_EQ(ifData.idAttributeList, nullptr);
    EXPECT_EQ(&areg::empty_interface(), &ifData);
}

TEST(ServiceDefsTest, proxy_data_starts_unavailable)
{
    areg::ProxyData data(test_interface());

    EXPECT_EQ(data.attribute_state(ATTR_A), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(data.attribute_state(ATTR_B), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(data.param_state(RESP_A), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(data.param_state(RESP_C), areg::DataState::DataIsUnavailable);
}

TEST(ServiceDefsTest, proxy_data_attribute_state)
{
    areg::ProxyData data(test_interface());

    data.set_data_state(ATTR_B, areg::DataState::DataIsOK);
    EXPECT_EQ(data.data_state(ATTR_B), areg::DataState::DataIsOK);
    EXPECT_EQ(data.attribute_state(ATTR_B), areg::DataState::DataIsOK);
    EXPECT_EQ(data.data_state(ATTR_A), areg::DataState::DataIsUnavailable);
}

TEST(ServiceDefsTest, proxy_data_response_state)
{
    areg::ProxyData data(test_interface());

    data.set_data_state(RESP_A, areg::DataState::DataIsOK);
    EXPECT_EQ(data.data_state(RESP_A), areg::DataState::DataIsOK);
    EXPECT_EQ(data.mParamState.at(0u, 0u), areg::DataState::DataIsOK);
    EXPECT_EQ(data.mParamState.at(0u, 1u), areg::DataState::DataIsOK);
    EXPECT_EQ(data.data_state(RESP_C), areg::DataState::DataIsUnavailable);

    // A response without parameters reports unavailable data.
    data.set_data_state(RESP_B, areg::DataState::DataIsOK);
    EXPECT_EQ(data.data_state(RESP_B), areg::DataState::DataIsUnavailable);
}

TEST(ServiceDefsTest, proxy_data_ignores_ids_of_other_kinds)
{
    areg::ProxyData data(test_interface());

    data.set_data_state(REQ_A, areg::DataState::DataIsOK);
    data.set_data_state(areg::RESPONSE_ID_NONE, areg::DataState::DataIsOK);
    EXPECT_EQ(data.data_state(REQ_A), areg::DataState::DataUnexpectedError);
    EXPECT_EQ(data.data_state(areg::RESPONSE_ID_NONE), areg::DataState::DataUnexpectedError);
    EXPECT_EQ(data.attribute_state(ATTR_A), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(data.param_state(RESP_A), areg::DataState::DataIsUnavailable);
}

TEST(ServiceDefsTest, proxy_data_reset)
{
    areg::ProxyData data(test_interface());
    data.set_data_state(ATTR_A, areg::DataState::DataIsOK);
    data.set_data_state(RESP_C, areg::DataState::DataIsInvalid);

    data.reset();

    EXPECT_EQ(data.attribute_state(ATTR_A), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(data.param_state(RESP_C), areg::DataState::DataIsUnavailable);
}

TEST(ServiceDefsTest, proxy_data_response_of_request)
{
    areg::ProxyData data(test_interface());

    EXPECT_EQ(data.response_id(REQ_A), RESP_A);
    EXPECT_EQ(data.response_id(REQ_B), areg::RESPONSE_ID_NONE);
    EXPECT_EQ(data.response_id(UNKNOWN), areg::INVALID_MESSAGE_ID);
    EXPECT_EQ(data.response_id(RESP_A), areg::INVALID_MESSAGE_ID);
    EXPECT_EQ(data.response_id(areg::RESPONSE_ID_NONE), areg::INVALID_MESSAGE_ID);
}

TEST(ServiceDefsTest, proxy_data_broadcast_state)
{
    areg::ProxyData data(test_interface());

    data.set_data_state(RESP_C, areg::DataState::DataIsOK);
    EXPECT_EQ(data.data_state(RESP_C), areg::DataState::DataIsOK);
    EXPECT_EQ(data.param_state(RESP_C), areg::DataState::DataIsOK);
    EXPECT_EQ(data.mParamState.at(2u, 0u), areg::DataState::DataIsOK);
    EXPECT_EQ(data.mParamState.at(0u, 0u), areg::DataState::DataIsUnavailable);
}

TEST(ServiceDefsTest, proxy_data_unknown_id_changes_nothing)
{
    areg::ProxyData data(test_interface());
    const uint32_t ids[] = { UNKNOWN, 0u, 0xFFFFu, areg::INVALID_MESSAGE_ID, 0xFFFFFFFFu, static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection) };
    for (uint32_t id : ids)
    {
        data.set_data_state(id, areg::DataState::DataIsOK);
        EXPECT_EQ(data.data_state(id), areg::DataState::DataUnexpectedError);
        EXPECT_EQ(data.attribute_state(id), areg::DataState::DataUnexpectedError);
        EXPECT_EQ(data.param_state(id), areg::DataState::DataUnexpectedError);
        EXPECT_EQ(data.response_id(id), areg::INVALID_MESSAGE_ID);
    }

    EXPECT_EQ(data.attribute_state(ATTR_A), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(data.attribute_state(ATTR_B), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(data.param_state(RESP_A), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(data.param_state(RESP_C), areg::DataState::DataIsUnavailable);
}

TEST(ServiceDefsTest, proxy_data_ids_of_the_wrong_kind_read_nothing)
{
    areg::ProxyData data(test_interface());
    data.set_data_state(ATTR_A, areg::DataState::DataIsOK);
    data.set_data_state(RESP_A, areg::DataState::DataIsOK);

    EXPECT_EQ(data.attribute_state(RESP_A), areg::DataState::DataUnexpectedError);
    EXPECT_EQ(data.param_state(ATTR_A), areg::DataState::DataUnexpectedError);
    EXPECT_EQ(data.attribute_state(REQ_A), areg::DataState::DataUnexpectedError);
    EXPECT_EQ(data.param_state(REQ_A), areg::DataState::DataUnexpectedError);
}

TEST(ServiceDefsTest, proxy_data_index_out_of_range_is_ignored)
{
    areg::ProxyData data(out_of_range_interface());

    data.set_data_state(ATTR_A, areg::DataState::DataIsOK);
    data.set_data_state(RESP_A, areg::DataState::DataIsOK);
    data.set_attribute_state(2u, areg::DataState::DataIsOK);
    data.set_param_state(3u, areg::DataState::DataIsOK);
    data.set_param_state(0xFFFFFFFFu, areg::DataState::DataIsOK);

    EXPECT_EQ(data.attribute_state(ATTR_A), areg::DataState::DataUnexpectedError);
    EXPECT_EQ(data.param_state(RESP_A), areg::DataState::DataUnexpectedError);
    EXPECT_EQ(data.data_state(ATTR_A), areg::DataState::DataUnexpectedError);
    EXPECT_EQ(data.response_id(REQ_A), areg::INVALID_MESSAGE_ID);
    EXPECT_EQ(data.mAttrState[0u], areg::DataState::DataIsUnavailable);
    EXPECT_EQ(data.mAttrState[1u], areg::DataState::DataIsUnavailable);
    EXPECT_EQ(data.mParamState.at(0u, 0u), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(data.mParamState.at(2u, 0u), areg::DataState::DataIsUnavailable);
}

TEST(ServiceDefsTest, proxy_data_set_by_index)
{
    areg::ProxyData data(test_interface());

    data.set_attribute_state(1u, areg::DataState::DataIsOK);
    data.set_param_state(2u, areg::DataState::DataIsInvalid);
    EXPECT_EQ(data.attribute_state(ATTR_B), areg::DataState::DataIsOK);
    EXPECT_EQ(data.attribute_state(ATTR_A), areg::DataState::DataIsUnavailable);
    EXPECT_EQ(data.param_state(RESP_C), areg::DataState::DataIsInvalid);
    EXPECT_EQ(data.param_state(RESP_A), areg::DataState::DataIsUnavailable);
}

//////////////////////////////////////////////////////////////////////////
// 6. Connected instance
//////////////////////////////////////////////////////////////////////////

TEST(ServiceDefsTest, connected_instance_defaults)
{
    const areg::ConnectedInstance instance{};

    EXPECT_EQ(instance.ciSource, areg::MessageSource::SourceUndefined);
    EXPECT_EQ(instance.ciBitness, areg::InstanceBitness::BitnessUnknown);
    EXPECT_EQ(instance.ciCookie, areg::COOKIE_UNKNOWN);
    EXPECT_EQ(instance.ciTimestamp, 0);
    EXPECT_TRUE(instance.ciInstance.empty());
    EXPECT_TRUE(instance.ciLocation.empty());
}

TEST(ServiceDefsTest, connected_instance_stream_round_trip)
{
    areg::ConnectedInstance source{};
    source.ciSource     = areg::MessageSource::SourceObserver;
    source.ciBitness    = areg::InstanceBitness::Bitness64;
    source.ciCookie     = areg::COOKIE_REMOTE_SERVICE;
    source.ciTimestamp  = 123456789;
    source.ciInstance   = "instance";
    source.ciLocation   = "location";

    areg::SharedBuffer buffer;
    buffer << source;
    buffer.move_to_begin();

    areg::ConnectedInstance target{};
    buffer >> target;

    EXPECT_EQ(target.ciSource, source.ciSource);
    EXPECT_EQ(target.ciBitness, source.ciBitness);
    EXPECT_EQ(target.ciCookie, source.ciCookie);
    EXPECT_EQ(target.ciTimestamp, source.ciTimestamp);
    EXPECT_EQ(target.ciInstance, source.ciInstance);
    EXPECT_EQ(target.ciLocation, source.ciLocation);
}

//////////////////////////////////////////////////////////////////////////
// 7. Remote service messages
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< The cookie of a remote instance that is a valid message source or target.
    const ITEM_ID REMOTE_COOKIE{ areg::COOKIE_REMOTE_SERVICE };

    areg::StubAddress make_stub(areg::ServiceType type)
    {
        return areg::StubAddress("RemoteDefsService", areg::Version(1, 2, 3), type, "remoteRole", "remoteThread");
    }

    areg::ProxyAddress make_proxy(areg::ServiceType type)
    {
        return areg::ProxyAddress("RemoteDefsService", areg::Version(1, 2, 3), type, "remoteRole", "remoteThread");
    }

    uint32_t id_of(areg::FuncIdRange id)
    {
        return static_cast<uint32_t>(id);
    }
}

TEST(RemoteServiceDefsTest, register_service_message)
{
    const areg::StubAddress stub = make_stub(areg::ServiceType::Public);
    const areg::MessageEnvelope msg = areg::router_register_service(stub, REMOTE_COOKIE, areg::COOKIE_ROUTER);

    ASSERT_NE(msg.header(), nullptr);
    EXPECT_EQ(msg.message_id(), id_of(areg::FuncIdRange::SystemServiceRequestRegister));
    EXPECT_EQ(msg.source(), static_cast<uint32_t>(REMOTE_COOKIE));
    EXPECT_EQ(msg.target(), static_cast<uint32_t>(areg::COOKIE_ROUTER));
    EXPECT_EQ(msg.call_type(), static_cast<uint8_t>(areg::CallType::RegisterProvider));
    EXPECT_EQ(msg.result(), static_cast<uint32_t>(areg::DisconnectReason::UndefinedReason));
    EXPECT_TRUE(areg::is_register_message(msg));

    areg::RawService raw;
    areg::Endpoint endpoint;
    stub.to_endpoint(raw, endpoint);
    EXPECT_EQ(msg.raw_service().service, raw.service);
    EXPECT_EQ(msg.raw_service().role, raw.role);
    EXPECT_EQ(msg.provider().number, endpoint.number);
    EXPECT_EQ(msg.provider().thread, endpoint.thread);
    EXPECT_EQ(msg.provider().major, 1u);
    EXPECT_EQ(msg.provider().minor, 2u);
    EXPECT_EQ(msg.provider().patch, 3u);
    EXPECT_EQ(msg.consumer().number, 0u);
}

TEST(RemoteServiceDefsTest, register_consumer_message)
{
    const areg::ProxyAddress proxy = make_proxy(areg::ServiceType::Public);
    const areg::MessageEnvelope msg = areg::router_register_consumer(proxy, REMOTE_COOKIE, areg::COOKIE_ROUTER);

    ASSERT_NE(msg.header(), nullptr);
    EXPECT_EQ(msg.message_id(), id_of(areg::FuncIdRange::SystemServiceRequestRegister));
    EXPECT_EQ(msg.call_type(), static_cast<uint8_t>(areg::CallType::RegisterConsumer));
    EXPECT_EQ(msg.source(), static_cast<uint32_t>(REMOTE_COOKIE));
    EXPECT_EQ(msg.target(), static_cast<uint32_t>(areg::COOKIE_ROUTER));

    areg::RawService raw;
    areg::Endpoint endpoint;
    proxy.to_endpoint(raw, endpoint);
    EXPECT_EQ(msg.raw_service().service, raw.service);
    EXPECT_EQ(msg.raw_service().role, raw.role);
    EXPECT_EQ(msg.consumer().number, endpoint.number);
    EXPECT_EQ(msg.consumer().thread, endpoint.thread);
    EXPECT_EQ(msg.provider().number, 0u);
}

TEST(RemoteServiceDefsTest, unregister_messages_carry_the_reason)
{
    const areg::StubAddress stub = make_stub(areg::ServiceType::Public);
    const areg::ProxyAddress proxy = make_proxy(areg::ServiceType::Public);

    const areg::MessageEnvelope msgStub = areg::router_unregister_service(stub, areg::DisconnectReason::ServiceLost, REMOTE_COOKIE, areg::COOKIE_ROUTER);
    ASSERT_NE(msgStub.header(), nullptr);
    EXPECT_EQ(msgStub.call_type(), static_cast<uint8_t>(areg::CallType::UnregisterProvider));
    EXPECT_EQ(msgStub.result(), static_cast<uint32_t>(areg::DisconnectReason::ServiceLost));
    EXPECT_EQ(msgStub.message_id(), id_of(areg::FuncIdRange::SystemServiceRequestRegister));

    const areg::MessageEnvelope msgProxy = areg::router_unregister_consumer(proxy, areg::DisconnectReason::ConsumerDisconnected, REMOTE_COOKIE, areg::COOKIE_ROUTER);
    ASSERT_NE(msgProxy.header(), nullptr);
    EXPECT_EQ(msgProxy.call_type(), static_cast<uint8_t>(areg::CallType::UnregisterConsumer));
    EXPECT_EQ(msgProxy.result(), static_cast<uint32_t>(areg::DisconnectReason::ConsumerDisconnected));
}

TEST(RemoteServiceDefsTest, router_requests_refuse_local_services)
{
    const areg::StubAddress stub = make_stub(areg::ServiceType::Local);
    const areg::ProxyAddress proxy = make_proxy(areg::ServiceType::Local);

    EXPECT_EQ(areg::router_register_service(stub, REMOTE_COOKIE, areg::COOKIE_ROUTER).header(), nullptr);
    EXPECT_EQ(areg::router_register_consumer(proxy, REMOTE_COOKIE, areg::COOKIE_ROUTER).header(), nullptr);
    EXPECT_EQ(areg::router_unregister_service(stub, areg::DisconnectReason::ServiceLost, REMOTE_COOKIE, areg::COOKIE_ROUTER).header(), nullptr);
    EXPECT_EQ(areg::router_unregister_consumer(proxy, areg::DisconnectReason::ConsumerLost, REMOTE_COOKIE, areg::COOKIE_ROUTER).header(), nullptr);
}

TEST(RemoteServiceDefsTest, router_requests_refuse_invalid_sources)
{
    const areg::StubAddress stub = make_stub(areg::ServiceType::Public);
    const areg::ProxyAddress proxy = make_proxy(areg::ServiceType::Public);
    const ITEM_ID invalidSources[] = { areg::COOKIE_UNKNOWN, areg::COOKIE_LOCAL };

    for (const ITEM_ID source : invalidSources)
    {
        EXPECT_EQ(areg::router_register_service(stub, source, areg::COOKIE_ROUTER).header(), nullptr);
        EXPECT_EQ(areg::router_register_consumer(proxy, source, areg::COOKIE_ROUTER).header(), nullptr);
        EXPECT_EQ(areg::router_unregister_service(stub, areg::DisconnectReason::ServiceLost, source, areg::COOKIE_ROUTER).header(), nullptr);
        EXPECT_EQ(areg::router_unregister_consumer(proxy, areg::DisconnectReason::ConsumerLost, source, areg::COOKIE_ROUTER).header(), nullptr);
    }
}

TEST(RemoteServiceDefsTest, registered_notifications)
{
    const areg::StubAddress stub = make_stub(areg::ServiceType::Public);
    const areg::ProxyAddress proxy = make_proxy(areg::ServiceType::Public);

    const areg::MessageEnvelope msgStub = areg::service_registered_event(stub, areg::COOKIE_ROUTER, REMOTE_COOKIE);
    ASSERT_NE(msgStub.header(), nullptr);
    EXPECT_EQ(msgStub.message_id(), id_of(areg::FuncIdRange::SystemServiceNotifyRegister));
    EXPECT_EQ(msgStub.call_type(), static_cast<uint8_t>(areg::CallType::RegisterProvider));
    EXPECT_EQ(msgStub.source(), static_cast<uint32_t>(areg::COOKIE_ROUTER));
    EXPECT_EQ(msgStub.target(), static_cast<uint32_t>(REMOTE_COOKIE));

    const areg::MessageEnvelope msgProxy = areg::client_registered_event(proxy, areg::COOKIE_ROUTER, REMOTE_COOKIE);
    ASSERT_NE(msgProxy.header(), nullptr);
    EXPECT_EQ(msgProxy.message_id(), id_of(areg::FuncIdRange::SystemServiceNotifyRegister));
    EXPECT_EQ(msgProxy.call_type(), static_cast<uint8_t>(areg::CallType::RegisterConsumer));
    EXPECT_EQ(msgProxy.target(), static_cast<uint32_t>(REMOTE_COOKIE));
}

TEST(RemoteServiceDefsTest, unregistered_notifications_carry_the_reason)
{
    const areg::StubAddress stub = make_stub(areg::ServiceType::Public);
    const areg::ProxyAddress proxy = make_proxy(areg::ServiceType::Public);

    const areg::MessageEnvelope msgStub = areg::service_unregistered_event(stub, areg::DisconnectReason::ProviderLost, areg::COOKIE_ROUTER, REMOTE_COOKIE);
    ASSERT_NE(msgStub.header(), nullptr);
    EXPECT_EQ(msgStub.call_type(), static_cast<uint8_t>(areg::CallType::UnregisterProvider));
    EXPECT_EQ(msgStub.result(), static_cast<uint32_t>(areg::DisconnectReason::ProviderLost));

    const areg::MessageEnvelope msgProxy = areg::client_unregistered_event(proxy, areg::DisconnectReason::ConsumerLost, areg::COOKIE_ROUTER, REMOTE_COOKIE);
    ASSERT_NE(msgProxy.header(), nullptr);
    EXPECT_EQ(msgProxy.call_type(), static_cast<uint8_t>(areg::CallType::UnregisterConsumer));
    EXPECT_EQ(msgProxy.result(), static_cast<uint32_t>(areg::DisconnectReason::ConsumerLost));
}

TEST(RemoteServiceDefsTest, notifications_refuse_local_services_and_invalid_targets)
{
    const areg::StubAddress localStub = make_stub(areg::ServiceType::Local);
    const areg::ProxyAddress localProxy = make_proxy(areg::ServiceType::Local);
    const areg::StubAddress stub = make_stub(areg::ServiceType::Public);
    const areg::ProxyAddress proxy = make_proxy(areg::ServiceType::Public);
    const areg::ConnectedInstance other{};

    EXPECT_EQ(areg::service_registered_event(localStub, areg::COOKIE_ROUTER, REMOTE_COOKIE).header(), nullptr);
    EXPECT_EQ(areg::client_registered_event(localProxy, areg::COOKIE_ROUTER, REMOTE_COOKIE).header(), nullptr);
    EXPECT_EQ(areg::service_unregistered_event(localStub, areg::DisconnectReason::ServiceLost, areg::COOKIE_ROUTER, REMOTE_COOKIE).header(), nullptr);
    EXPECT_EQ(areg::client_unregistered_event(localProxy, areg::DisconnectReason::ServiceLost, areg::COOKIE_ROUTER, REMOTE_COOKIE).header(), nullptr);
    EXPECT_EQ(areg::service_duplicate_event(localStub, other, areg::COOKIE_ROUTER, REMOTE_COOKIE).header(), nullptr);

    const ITEM_ID invalidTargets[] = { areg::COOKIE_UNKNOWN, areg::COOKIE_LOCAL };
    for (const ITEM_ID target : invalidTargets)
    {
        EXPECT_EQ(areg::service_registered_event(stub, areg::COOKIE_ROUTER, target).header(), nullptr);
        EXPECT_EQ(areg::client_registered_event(proxy, areg::COOKIE_ROUTER, target).header(), nullptr);
        EXPECT_EQ(areg::service_unregistered_event(stub, areg::DisconnectReason::ServiceLost, areg::COOKIE_ROUTER, target).header(), nullptr);
        EXPECT_EQ(areg::client_unregistered_event(proxy, areg::DisconnectReason::ServiceLost, areg::COOKIE_ROUTER, target).header(), nullptr);
        EXPECT_EQ(areg::service_duplicate_event(stub, other, areg::COOKIE_ROUTER, target).header(), nullptr);
    }
}

TEST(RemoteServiceDefsTest, duplicate_event_carries_the_other_instance)
{
    const areg::StubAddress stub = make_stub(areg::ServiceType::Public);
    areg::ConnectedInstance other{};
    other.ciSource   = areg::MessageSource::SourceClient;
    other.ciBitness  = areg::InstanceBitness::Bitness32;
    other.ciCookie   = REMOTE_COOKIE;
    other.ciInstance = "other-app";
    other.ciLocation = "other-path";

    const areg::MessageEnvelope msg = areg::service_duplicate_event(stub, other, areg::COOKIE_ROUTER, REMOTE_COOKIE);
    ASSERT_NE(msg.header(), nullptr);
    EXPECT_EQ(msg.message_id(), id_of(areg::FuncIdRange::SystemServiceNotifyDuplicate));
    EXPECT_EQ(msg.source(), static_cast<uint32_t>(areg::COOKIE_ROUTER));
    EXPECT_EQ(msg.target(), static_cast<uint32_t>(REMOTE_COOKIE));

    areg::MessageEnvelope payload = msg.clone();
    payload.move_to_begin();
    areg::ConnectedInstance read{};
    payload >> read;
    EXPECT_EQ(read.ciSource, other.ciSource);
    EXPECT_EQ(read.ciBitness, other.ciBitness);
    EXPECT_EQ(read.ciCookie, other.ciCookie);
    EXPECT_EQ(read.ciInstance, other.ciInstance);
    EXPECT_EQ(read.ciLocation, other.ciLocation);
}

TEST(RemoteServiceDefsTest, connect_request_and_server_hello)
{
    const areg::MessageEnvelope msg = areg::create_connect_request(REMOTE_COOKIE, areg::COOKIE_ROUTER, areg::MessageSource::SourceTest);

    ASSERT_NE(msg.header(), nullptr);
    EXPECT_EQ(msg.message_id(), id_of(areg::FuncIdRange::SystemServiceConnect));
    EXPECT_EQ(msg.source(), static_cast<uint32_t>(areg::SOURCE_UNKNOWN));
    EXPECT_EQ(msg.target(), static_cast<uint32_t>(areg::COOKIE_ROUTER));
    EXPECT_TRUE(areg::is_server_hello(msg));
    EXPECT_FALSE(areg::is_server_bye(msg));
    EXPECT_FALSE(areg::is_notify_client(msg));
    EXPECT_FALSE(areg::is_register_message(msg));

    areg::MessageEnvelope payload = msg.clone();
    payload.move_to_begin();
    areg::ConnectedInstance read{};
    payload >> read;
    EXPECT_EQ(read.ciSource, areg::MessageSource::SourceTest);
    EXPECT_EQ(read.ciCookie, REMOTE_COOKIE);
    EXPECT_NE(read.ciBitness, areg::InstanceBitness::BitnessUnknown);
    EXPECT_FALSE(read.ciInstance.empty());
}

TEST(RemoteServiceDefsTest, disconnect_request_and_server_bye)
{
    const areg::MessageEnvelope msg = areg::create_disconnect_request(REMOTE_COOKIE, areg::COOKIE_ROUTER);

    ASSERT_NE(msg.header(), nullptr);
    EXPECT_EQ(msg.message_id(), id_of(areg::FuncIdRange::SystemServiceDisconnect));
    EXPECT_EQ(msg.source(), static_cast<uint32_t>(REMOTE_COOKIE));
    EXPECT_EQ(msg.target(), static_cast<uint32_t>(areg::COOKIE_ROUTER));
    EXPECT_TRUE(areg::is_server_bye(msg));
    EXPECT_FALSE(areg::is_server_hello(msg));

    areg::MessageEnvelope payload = msg.clone();
    payload.move_to_begin();
    ITEM_ID cookie{ areg::COOKIE_UNKNOWN };
    payload >> cookie;
    EXPECT_EQ(cookie, REMOTE_COOKIE);

    const areg::MessageEnvelope unknownSource = areg::create_disconnect_request(areg::COOKIE_UNKNOWN, areg::COOKIE_ROUTER);
    EXPECT_FALSE(areg::is_server_bye(unknownSource));
}

TEST(RemoteServiceDefsTest, connection_notifications)
{
    struct Row
    {
        areg::MessageEnvelope           msg;
        areg::ServiceConnectionState    state;
    };

    const Row rows[] =
    {
          { areg::create_connect_notify(areg::COOKIE_ROUTER, REMOTE_COOKIE),    areg::ServiceConnectionState::Connected }
        , { areg::create_disconnect_notify(areg::COOKIE_ROUTER, REMOTE_COOKIE), areg::ServiceConnectionState::Disconnected }
        , { areg::create_reject_notify(areg::COOKIE_ROUTER, REMOTE_COOKIE),     areg::ServiceConnectionState::Rejected }
    };

    for (const Row& row : rows)
    {
        ASSERT_NE(row.msg.header(), nullptr);
        EXPECT_EQ(row.msg.message_id(), id_of(areg::FuncIdRange::SystemServiceNotifyConnection));
        EXPECT_EQ(row.msg.source(), static_cast<uint32_t>(areg::COOKIE_ROUTER));
        EXPECT_EQ(row.msg.target(), static_cast<uint32_t>(REMOTE_COOKIE));
        EXPECT_TRUE(areg::is_notify_client(row.msg));
        EXPECT_FALSE(areg::is_server_hello(row.msg));

        areg::MessageEnvelope payload = row.msg.clone();
        payload.move_to_begin();
        ITEM_ID cookie{ areg::COOKIE_UNKNOWN };
        areg::ServiceConnectionState state{ areg::ServiceConnectionState::Unknown };
        payload >> cookie >> state;
        EXPECT_EQ(cookie, REMOTE_COOKIE);
        EXPECT_EQ(state, row.state);
    }

    // The same message from a source other than the router is not a notification of the router.
    const areg::MessageEnvelope fake = areg::create_connect_notify(REMOTE_COOKIE, REMOTE_COOKIE);
    EXPECT_FALSE(areg::is_notify_client(fake));
}

TEST(RemoteServiceDefsTest, register_message_check_needs_known_source_and_router_target)
{
    const areg::StubAddress stub = make_stub(areg::ServiceType::Public);

    const areg::MessageEnvelope toRouter = areg::router_register_service(stub, REMOTE_COOKIE, areg::COOKIE_ROUTER);
    EXPECT_TRUE(areg::is_register_message(toRouter));

    const areg::MessageEnvelope toOther = areg::router_register_service(stub, REMOTE_COOKIE, REMOTE_COOKIE);
    ASSERT_NE(toOther.header(), nullptr);
    EXPECT_FALSE(areg::is_register_message(toOther));

    areg::MessageEnvelope unknownSource = toRouter.clone();
    unknownSource.set_source(static_cast<uint32_t>(areg::COOKIE_UNKNOWN));
    EXPECT_FALSE(areg::is_register_message(unknownSource));
}

TEST(RemoteServiceDefsTest, message_checks_reject_envelopes_without_header)
{
    const areg::MessageEnvelope empty;

    EXPECT_EQ(empty.header(), nullptr);
    EXPECT_FALSE(areg::is_server_hello(empty));
    EXPECT_FALSE(areg::is_server_bye(empty));
    EXPECT_FALSE(areg::is_notify_client(empty));
    EXPECT_FALSE(areg::is_register_message(empty));
}

TEST(RemoteServiceDefsTest, server_hello_needs_unknown_source)
{
    areg::MessageEnvelope hello = areg::create_connect_request(REMOTE_COOKIE, areg::COOKIE_ROUTER, areg::MessageSource::SourceClient).clone();
    ASSERT_NE(hello.header(), nullptr);
    EXPECT_TRUE(areg::is_server_hello(hello));

    hello.set_source(static_cast<uint32_t>(REMOTE_COOKIE));
    EXPECT_FALSE(areg::is_server_hello(hello));
}

TEST(ServiceDefsTest, connection_channel_is_not_valid_before_the_handshake)
{
    EXPECT_FALSE(areg::connection_channel().is_valid());
}

TEST(ServiceDefsTest, parameter_array_move_assign_to_itself_and_to_a_moved_from_array)
{
    areg::ParameterArray params(test_interface());
    params.set_value_at(2u, 0u, areg::DataState::DataIsOK);

    areg::ParameterArray& alias = params;
    params = std::move(alias);
    ASSERT_TRUE(params.is_valid_index(2u));
    EXPECT_EQ(params.at(2u, 0u), areg::DataState::DataIsOK);

    areg::ParameterArray emptied(std::move(params));
    params = std::move(emptied);
    ASSERT_TRUE(params.is_valid_index(2u));
    EXPECT_EQ(params.at(2u, 0u), areg::DataState::DataIsOK);
    EXPECT_FALSE(emptied.is_valid_index(0u));
}
