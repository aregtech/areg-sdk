#ifndef AREG_COMPONENT_SERVICE_TYPES_HPP
#define AREG_COMPONENT_SERVICE_TYPES_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/service/types.hpp
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

//////////////////////////////////////////////////////////////////////////
// areg namespace for services
//////////////////////////////////////////////////////////////////////////
namespace areg {
  
/**
 * \brief   Result types.
 *          Used when sending response service event
 *          and needs to define validation of execution.
 **/
enum class ResultType   : uint16_t
{
    /* not used */
      Undefined          =     0 /*0x0000*/  //!< undefined result. not used             Bits: 0000 0000 0000 0000
    /* additional bits */
    , Error              =     1 /*0x0001*/  //!< indicate error. contains error bit     Bits: 0000 0000 0000 0001
    , Undelivered        =    32 /*0x0020*/  //!< message was not delivered              Bits: 0000 0000 0010 0000
    , NotProcessed       =    64 /*0x0040*/  //!< call did not reach target              Bits: 0000 0000 0100 0000
    , Processed          =   128 /*0x0080*/  //!< call reached target                    Bits: 0000 0000 1000 0000

    /* Message did not reach target, used in remote messaging */
    , MessageUndelivered =  4129 /*0x1021*/  //!< request failed to reach target.        Bits: 0001 0000 0010 0001

    /* request calls result */
    , RequestOK          =  8320 /*0x2080*/  //!< indicates success of request call.     Bits: 0010 0000 1000 0000
    , RequestInvalid     =  8257 /*0x2041*/  //!< indicates failure of request call.     Bits: 0010 0000 0100 0001
    , RequestError       =  8321 /*0x2081*/  //!< indicates request execution failure.   Bits: 0010 0000 1000 0001
    , RequestBusy        =  8323 /*0x2083*/  //!< request cannot execute, it is busy.    Bits: 0010 0000 1000 0011
    , RequestCanceled    =  8325 /*0x2085*/  //!< request is canceled and not executed.  Bits: 0010 0000 1000 0101

    /* data update result */
    , DataOK             = 16512 /*0x4080*/  //!< indicates data validation.             Bits: 0100 0000 1000 0000
    , DataInvalid        = 16449 /*0x4041*/  //!< indicates data invalid.                Bits: 0100 0000 0100 0001

    /* service call result */
    , ServiceOK          = 32896 /*0x8080*/  //!< service call processed.                Bits: 1000 0000 1000 0000
    , ServiceUnavailable = 32833 /*0x8041*/  //!< service is unavailable.                Bits: 1000 0000 0100 0001
    , Invalid            = 32897 /*0x8081*/  //!< service invalid (check cookie).        Bits: 1000 0000 1000 0001
    , ServiceRejected    = 32899 /*0x8083*/  //!< service rejected (unsupported).        Bits: 1000 0000 1000 0011
};

/**
 * \brief   Returns string representation of ResultType.
 **/
[[nodiscard]]
inline constexpr const char* as_string(areg::ResultType value) noexcept;

/**
 * \brief   Data types. Used getting data
 **/
enum class DataState    : uint16_t
{
      DataIsUndefined       =     0 /*0x0000*/  //!< undefined type, not used
    , DataIsOK              = 16384 /*0x4000*/  //!< data valid
    , DataIsInvalid         = 16385 /*0x4001*/  //!< data is invalid
    , DataIsUnavailable     = 16387 /*0x4003*/  //!< data is unavailable, no such data
    , DataUnexpectedError   = 16389 /*0x4005*/  //!< other errors
};
/**
 * \brief   Returns string representation of DataState.
 **/
[[nodiscard]]
inline constexpr const char* as_string(areg::DataState value) noexcept;

/**
 * \brief   Type of request. Used sending request event by proxy
 **/
enum class RequestType : uint16_t
{
      Unprocessed       =     0 /*0x0000*/  //!< request is unprocessed
    , StartNotify       =  8193 /*0x2001*/  //!< request start notify on attribute update
    , StopNotify        =  8194 /*0x2002*/  //!< request stop notify on attribute update
    , RemoveAllNotify   =  8198 /*0x2006*/  //!< request remove all notifications
    , CallFunction      = 16385 /*0x4001*/  //!< request function call
    , ServiceConnection = 16387 /*0x4003*/  //!< request connection status update
    , ClientConnection  = 16389 /*0x4005*/  //!< request client connection status update
    , LoadComponent     = 16391 /*0x4007*/  //!< request load component
};
/**
 * \brief   Returns string representation of RequestType.
 **/
[[nodiscard]]
inline constexpr const char* as_string( areg::RequestType value ) noexcept;

/**
 * \brief   Message Data types
 *          Used to identify type of event data, which
 *          used either in service request or in service response
 *          event objects
 **/
enum class MessageDataType  : uint16_t
{
      UndefinedData = 0x0000u   //!< data type is undefined
    , RequestData   = 0x1000u   //!< data type of request event
    , ResponseData  = 0x2000u   //!< data type response event
    , AttributeData = 0x4000u   //!< data type of attribute event
    , ServiceData   = 0x8000u   //!< data type service call

};
/**
 * \brief   Returns string representation of MessageDataType.
 **/
[[nodiscard]]
inline constexpr const char* as_string( areg::MessageDataType value ) noexcept;

/**
 * \brief   Returns message data type from message ID.
 **/
[[nodiscard]]
inline constexpr areg::MessageDataType message_data_type( uint32_t msgId ) noexcept;

/**
 * \brief   Service Connections. Used in service calls
 **/
enum class ServiceConnectionState   : uint16_t
{
      Unknown           =    0  /*0x0000*/  //!< Connection is unknown.             Bit set: 0000 0000 0000 0000
    , Connected         =    1  /*0x0001*/  //!< Service connected, ready to serve. Bit set: 0000 0000 0000 0001
    , Disconnected      =  256  /*0x0100*/  //!< Service disconnected.              Bit set: 0000 0001 0000 0000
    , Pending           =  257  /*0x0101*/  //!< Service connection is pending.     Bit set: 0000 0001 0000 0001
    , ConnectionLost    =  768  /*0x0300*/  //!< Service lost connection.           Bit set: 0000 0011 0000 0000
    , Rejected          = 1280  /*0x0500*/  //!< Service connection rejected.       Bit set: 0000 0101 0000 0000
    , Failed            = 2304  /*0x0900*/  //!< Service connection failed.         Bit set: 0000 1001 0000 0000
    , Shutdown          = 4352  /*0x1100*/  //!< Service shut down, no connection.  Bit set: 0001 0001 0000 0000
};

/**
 * \brief   Returns string representation of ServiceConnectionState.
 **/
[[nodiscard]]
inline constexpr const char* as_string( areg::ServiceConnectionState value ) noexcept;

/**
 * \brief   Returns true if service is connected.
 **/
[[nodiscard]]
inline constexpr bool is_service_connected( areg::ServiceConnectionState state ) noexcept;

/**
 * \brief   Returns true if service connection is pending.
 **/
[[nodiscard]]
inline constexpr bool is_connection_pending( areg::ServiceConnectionState state ) noexcept;

/**
 * \brief   Returns true if service connection is rejected.
 **/
[[nodiscard]]
inline constexpr bool is_service_rejected( areg::ServiceConnectionState state ) noexcept;

/**
 * \brief   Returns true if service connection is lost.
 **/
[[nodiscard]]
inline constexpr bool is_connection_lost( areg::ServiceConnectionState state ) noexcept;

/**
 * \brief   Returns true if service is not connected.
 **/
[[nodiscard]]
inline constexpr bool is_service_disconnected( areg::ServiceConnectionState state ) noexcept;

/**
 * \brief   The service provider and service consumer disconnect reason.
 *          Valid only when service is not connected. Otherwise, it should be ignored.
 **/
enum class DisconnectReason : uint16_t
{
      UndefinedReason       = 0     //!< Undefined disconnect reason.
    , ServiceDisconnected   = 1     //!< The service is disconnected.
    , ServiceLost           = 2     //!< The service connection is lost.
    , ServiceRejected       = 4     //!< The service rejected connection.
    , ProviderDisconnected  = 8     //!< The service provider disconnected.
    , ProviderLost          = 16    //!< The connection with service provider is lost.
    , ProviderRejected      = 32    //!< The service provider rejected the service consumer.
    , ConsumerDisconnected  = 64    //!< The service consumer is disconnected.
    , ConsumerLost          = 128   //!< The connection with service consumer is lost.
    , ConsumerNotSupported  = 256   //!< The service consumer is rejected because is not supported (wrong version, for example).
    , SystemShutdown        = 512   //!< The system is shutting down.
    , ClientConnectionLost  = 1024  //!< The system lost connection with the client. General reason.
    , ClientConnectionClosed= 2048  //!< The client requested to disconnect. General reason.
};

/**
 * \brief   Returns string representation of DisconnectReason.
 **/
[[nodiscard]]
inline constexpr const char* as_string( areg::DisconnectReason reason ) noexcept;

/**
 * \brief   Returns service connection state from disconnect reason.
 **/
[[nodiscard]]
inline constexpr areg::ServiceConnectionState service_connection( areg::DisconnectReason reason ) noexcept;

/**
 * \brief   The service request types.
 **/
enum class RegistrationAction   : uint16_t
{
      RegisterConsumer   = 0x0010   //!< Service Consumer requests to register.     Bit set: 0001 0000
    , UnregisterConsumer = 0x0011   //!< Service Consumer requests to unregister.   Bit set: 0001 0001
    , RegisterProvider   = 0x0020   //!< Service Provider requests to register.     Bit set: 0010 0000
    , UnregisterProvider = 0x0021   //!< Service Provider requests to unregister.   Bit set: 0010 0001
};

/**
 * \brief   Returns string representation of RegistrationAction.
 **/
[[nodiscard]]
inline constexpr const char* as_string( areg::RegistrationAction value ) noexcept;

/**
 * \brief   Service Type. Either local or Remote.
 **/
enum class ServiceType  : uint16_t
{
      Invalid   = 0x0000    //!< Invalid Service.  Bit set: 0000 0000 0000 0000
    , Local     = 0x0040    //!< Local Service.    Bit set: 0000 0000 0100 0000
    , Public    = 0x0080    //!< Public Service.   Bit set: 0000 0000 1000 0000
    , Internet  = 0x0100    //!< Internet Service. Bit set: 0000 0001 0000 0000
    , Any       = 0x01C0    //!< Any service.      Bit set: 0000 0001 1100 0000
};

/**
 * \brief   Returns string representation of ServiceType.
 **/
[[nodiscard]]
inline constexpr const char* as_string( areg::ServiceType value ) noexcept;

/**
 * \brief   The source of the communication message
 **/
enum class MessageSource    : uint16_t
{
      SourceUndefined   = 0     //!< The source of the message is undefined.
    , SourceClient      = 1     //!< The source of the message is connected client application.
    , SourceService     = 2     //!< The source of the message is the system service.
    , SourceObserver    = 4     //!< The source of the message is any observer, including viewer.
    , SourceTest        = 8     //!< The source of the message is a testing application.
    , SourceSimulation  = 16    //!< The source of the message is a simulation application.
};

/**
 * \brief   Returns string representation of MessageSource.
 **/
[[nodiscard]]
inline constexpr const char* as_string(areg::MessageSource value) noexcept;

/**
 * \brief   The bitness of the executable instance.
 **/
enum class InstanceBitness  : uint16_t
{
      BitnessUnknown    = 0     //!< Unknown bitness
    , Bitness32         = 32    //!< 32-bit system
    , Bitness64         = 64    //!< 64-bit system
};

/**
 * \brief   Returns string representation of InstanceBitness.
 **/
[[nodiscard]]
inline constexpr const char* as_string(areg::InstanceBitness bitness) noexcept;

/**
 * \brief   Specifies the service call type
 **/
enum class ServiceCallType : uint16_t
{
      NoFunction            = static_cast<uint16_t>(MessageDataType::UndefinedData) //!< No function call
    , RequestFunction       = static_cast<uint16_t>(MessageDataType::RequestData  ) //!< Call of service request function
    , ResponseFunction      = static_cast<uint16_t>(MessageDataType::ResponseData ) //!< Call of service response function
    , AttributeUpdate       = static_cast<uint16_t>(MessageDataType::AttributeData) //!< Call of service attribute update function
    , ServiceRegisteration  = static_cast<uint16_t>(MessageDataType::ServiceData  ) //!< Call of service registration
};

/**
 * \brief   Predefined range of function message IDs
 **/
constexpr uint32_t  FUNC_RANGE          { static_cast<uint32_t>(4095) };  /*0x0FFF*/

/**
 * \brief   The first ID of valid service interface function call
 **/
constexpr uint32_t  SERVICE_FUNCTION    { static_cast<uint32_t>(ServiceCallType::RequestFunction) };    /*0x1000*/

/**
 * \brief   The first ID in request call.
 **/
constexpr uint32_t  REQUEST_ID_FIRST    { static_cast<uint32_t>(ServiceCallType::RequestFunction) };
/**
 * \brief   The last ID in request call.
 **/
constexpr uint32_t  REQUEST_ID_LAST     { REQUEST_ID_FIRST + FUNC_RANGE };

/**
 * \brief   The first ID in response call.
 **/
constexpr uint32_t  RESPONSE_ID_FIRST   { static_cast<uint32_t>(ServiceCallType::ResponseFunction) };
/**
 * \brief   The last ID in response call.
 **/
constexpr uint32_t  RESPONSE_ID_LAST    { RESPONSE_ID_FIRST + FUNC_RANGE };

/**
 * \brief   The first ID in attribute call.
 **/
constexpr uint32_t  ATTRIBUTE_ID_FIRST  { static_cast<uint32_t>(ServiceCallType::AttributeUpdate) };
/**
 * \brief   The last ID in attribute call.
 **/
constexpr uint32_t  ATTRIBUTE_ID_LAST   { ATTRIBUTE_ID_FIRST + FUNC_RANGE };

/**
 * \brief   The last ID in service call.
 **/
constexpr uint32_t  SERVICE_ID_FIRST    { static_cast<uint32_t>(areg::ServiceCallType::ServiceRegisteration) };
/**
 * \brief   The last ID in service call.
 **/
constexpr uint32_t  SERVICE_ID_LAST     { SERVICE_ID_FIRST + FUNC_RANGE };

/**
 * \brief   Constant no response. Used to indicate that the request has no response.
 **/
constexpr uint32_t  RESPONSE_ID_NONE    { static_cast<uint32_t>(ServiceCallType::NoFunction) };

/**
 * \brief   Predefined range of function calls
 **/
enum class FuncIdRange  : uint32_t
{
    // Reserved system calls.

    //!< Empty function ID.
      EmptyFunctionId       = RESPONSE_ID_NONE
    //!< A call to remove pointer in a component thread context.
    , ComponentCleanup      = 0x00000100

    // The developer defined calls.
    
    //!< The first ID or request call range. Requests should start from this ID.
    , RequestFirstId        = REQUEST_ID_FIRST
    //!< The last ID of request calls.
    , RequestLastId         = REQUEST_ID_LAST

    //!< The first ID of response call range. Responses should start from this ID
    , ResponseFirstId       = RESPONSE_ID_FIRST
    //!< The last ID of request calls.
    , ResponseLastId        = RESPONSE_ID_LAST
    
    //!< The first ID of Attribute update call range. Attribute updates should start from this ID
    , AttributeFirstId      = ATTRIBUTE_ID_FIRST
    , AttributeLastId       = ATTRIBUTE_ID_LAST

    // Reserved system calls.

    //!< The service registration call. Service calls should start from this ID.
    , RequestRegisterService= SERVICE_ID_FIRST
    //!< Sent by client to Stub to get supported version information
    , RequestServiceProviderVersion
    //!< Sent by Stub to clients as a reply to get service version and notifies interface implemented version
    , ResponseServiceProviderVersion
    //!< Sent by client or stub to request service manager connection.
    , RequestServiceProviderConnection
    //!< Sent by service manager to targets (client or stub) to notify connection status update.
    , ResponseServiceProviderConnection
    //!< Called by service manager when connecting Remove Service like message router or log collector (handshake procedure)
    , SystemServiceConnect
    //!< Called by service manager when disconnecting Remove Service like message router or log collector (graceful shutdown procedure)
    , SystemServiceDisconnect
    //!< Sent by System Service executable to notify the connection status
    , SystemServiceNotifyConnection
    //!< Called by connected observer applications to the system service application to query the connected instances.
    , SystemServiceQueryInstances
    //!< Triggered by system service application to notify the observers about the connected instances.
    , SystemServiceNotifyInstances
    //!< Called by service manager to register available client or stub services
    , SystemServiceRequestRegister
    //!< Sent by Routing Service as a reply to register service and notifies the registered service availability
    , SystemServiceNotifyRegister
    //!< Sent by log collector service or client applications to register the scopes. This resets and overwrites all scope states.
    , ServiceLogRegisterScopes
    //!< Sent by observer, log collector service or client applications to update the scopes. This updates only given scopes.
    , ServiceLogUpdateScopes
    //!< Sent by observer or log collector service to the client application to query the list of scopes
    , ServiceLogQueryScopes
    //!< Sent by log source clients to notify that the log scopes have been updated.
    , ServiceLogScopesUpdated
    //!< Sent by observer or log collector service to the client application to save the log configuration file.
    , ServiceSaveLogConfiguration
    //!< Sent by log source to notify that the configuration file is saved.
    , ServiceLogConfigurationSaved
    //!< Sent by log collector service or client applications to log the messages.
    , ServiceLogMessage
    //!< The last ID of service calls.
    , ServiceLastId         = SERVICE_ID_LAST  //!< Servicing call last ID

};

/**
 * \brief   Returns string representation of FuncIdRange.
 **/
[[nodiscard]]
inline constexpr const char* as_string( areg::FuncIdRange value ) noexcept;

/**
 * \brief   Converts request message ID to array index. Returns the array index; INVALID_INDEX if empty.
 **/
[[nodiscard]]
inline constexpr uint32_t req_index(uint32_t msgId) noexcept;

/**
 * \brief   Converts response message ID to array index. Returns the array index; INVALID_INDEX if empty.
 **/
[[nodiscard]]
inline constexpr uint32_t resp_index(uint32_t msgId) noexcept;

/**
 * \brief   Converts attribute message ID to array index. Returns the array index; INVALID_INDEX if empty.
 **/
[[nodiscard]]
inline constexpr uint32_t attr_index(uint32_t msgId) noexcept;

/**
 * \brief   Returns true if message ID is an attribute call.
 **/
[[nodiscard]]
inline constexpr bool is_attribute_id(uint32_t msgId) noexcept;

/**
 * \brief   Returns true if message ID is a response call.
 **/
[[nodiscard]]
inline constexpr bool is_response_id(uint32_t msgId) noexcept;

/**
 * \brief   Returns true if message ID is a request call.
 **/
[[nodiscard]]
inline constexpr bool is_request_id(uint32_t msgId) noexcept;

/**
 * \brief   Returns true if message ID is a service registration call.
 **/
[[nodiscard]]
inline constexpr bool is_registry_id( uint32_t msgId ) noexcept;

/**
 * \brief   Returns true if message ID is a version notification.
 **/
[[nodiscard]]
inline constexpr bool is_version_id(uint32_t msgId) noexcept;

/**
 * \brief   Returns true if message ID is a service connect notification.
 **/
[[nodiscard]]
inline constexpr bool is_connect_id(uint32_t msgId) noexcept;

/**
 * \brief   Returns true if message ID is a request, response, or attribute notification.
 **/
[[nodiscard]]
inline constexpr bool is_executable_id( uint32_t msgId ) noexcept;

/**
 * \brief   Returns true if message ID is an empty function.
 **/
[[nodiscard]]
inline constexpr bool is_empty_id(uint32_t msgId) noexcept;


} // namespace areg


#endif // AREG_COMPONENT_SERVICE_TYPES_HPP
