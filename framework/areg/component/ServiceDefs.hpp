#ifndef AREG_COMPONENT_SERVICEDEFS_HPP
#define AREG_COMPONENT_SERVICEDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ServiceDefs.hpp
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
#include "areg/base/IOStream.hpp"
#include "areg/base/OrderedMap.hpp"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/FixedArray.hpp"
#include "areg/base/Version.hpp"

#include <limits>


/************************************************************************
 * Dependencies
 ************************************************************************/

namespace areg {
    class MessageEnvelope;
    class Channel;
}

/**
 * \brief       areg namespace contains defined and fixed constants,
 *              structures, types and classes required to define Service
 *              Interface and interface data used by Proxy and Stub objects.
 * 
 * \details     In this namespace are defined range of request, response
 *              and attribute messages. Also data validation flags, error 
 *              codes, request types, helper function and classes, as well
 *              as class of Service Interface data.
 *
 **/
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
enum class RegistrationAction   : uint8_t
{
      RegisterConsumer   = 0x10 //!< Service Consumer requests to register.     Bit set: 0001 0000
    , UnregisterConsumer = 0x11 //!< Service Consumer requests to unregister.   Bit set: 0001 0001
    , RegisterProvider   = 0x20 //!< Service Provider requests to register.     Bit set: 0010 0000
    , UnregisterProvider = 0x21 //!< Service Provider requests to unregister.   Bit set: 0010 0001
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

enum class MessageCallType  : uint8_t
{
      CallUndefined   = 0
    , CallService
    , CallRegistration
    , CallSystem
    , CallRequest
    , CallResponse
    , CallBroadcast
    , CallData
};

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

constexpr uint32_t  EXECUTABLE_MESSAGE{ static_cast<uint32_t>(areg::ServiceCallType::RequestFunction)  |
                                        static_cast<uint32_t>(areg::ServiceCallType::ResponseFunction) |
                                        static_cast<uint32_t>(areg::ServiceCallType::AttributeUpdate) };
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

class ParameterArray;

//////////////////////////////////////////////////////////////////////////
// StateArray class declaration
//////////////////////////////////////////////////////////////////////////
using StateArrayBase    = FixedArray<areg::DataState>;
/**
 * \brief   Fixed-size array of data states for tracking the state of attributes, responses, and
 *          their parameters.
 **/
class AREG_API StateArray : private StateArrayBase
{
    friend class ParameterArray;
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes a fixed-size array with the given capacity. The size cannot be changed dynamically.
     **/
    StateArray(uint32_t count);

    /**
     * \brief   Initializes the array to reference an existing buffer without making a copy.
     **/
    StateArray(uint8_t* thisBffer, int32_t elemCount);

    ~StateArray();

//////////////////////////////////////////////////////////////////////////
// Operators, attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns a read-only reference to the element at the specified index.
     **/
    [[nodiscard]]
    inline const areg::DataState& operator [] (uint32_t index) const noexcept;
    /**
     * \brief   Returns a modifiable reference to the element at the specified index.
     **/
    [[nodiscard]]
    inline areg::DataState& operator [] (uint32_t index) noexcept;

    /**
     * \brief   Returns the number of elements in the array.
     **/
    [[nodiscard]]
    inline uint32_t size() const noexcept;
    /**
     * \brief   Resets all states in the array to areg::DataIsUnavailable.
     **/
    inline void reset() noexcept;

    /**
     * \brief   Returns true if the array contains parameters (size is not zero).
     **/
    [[nodiscard]]
    inline bool has_params() const noexcept;

    /**
     * \brief   Sets the state of the element at the specified index.
     *
     * \param   whichIndex      The index of the element to modify.
     * \param   newState        The state to set.
     **/
    inline void set_state(int32_t whichIndex, areg::DataState newState) noexcept;

    /**
     * \brief   Sets all elements in the array to the specified state.
     *
     * \param   newState    The state to set for all elements.
     **/
    inline void set_all_state(areg::DataState newState) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //! Flag, indicating whether the state buffer is external or not.
    //! In case of external buffer, it will not be deleted when release.
    const bool  mExternal;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    StateArray() = delete;
    AREG_NOCOPY_NOMOVE(StateArray);
};

//////////////////////////////////////////////////////////////////////////
// InterfaceData structure declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface data structure. Contains information of service
 *          interface required by Proxy object. Should be initialized
 *          for every service interface.
 **/
struct AREG_API InterfaceData
{
    const char *    idServiceName{ nullptr };               //!< The name of service interface.
    Version         idVersion{ 0, 0, 0 };                   //!< Service interface implementation version
    ServiceType     idServiceType{ ServiceType::Invalid };  //!< The service type. Should be either local or public / remote.
    uint32_t        idRequestCount{ 0u };                   //!< Number of requests in service interface
    uint32_t        idResponseCount{ 0u };                  //!< Number of responses in service interface, including broadcasts
    uint32_t        idAttributeCount{ 0u };                 //!< Number of attributes in service interface
    const uint32_t* idRequestList{ nullptr };               //!< The list of requests. It is nullptr if the list is empty.
    const uint32_t* idResponseList{ nullptr };              //!< The list of responses. It is nullptr if the list is empty.
    const uint32_t* idAttributeList{ nullptr };             //!< The list of attributes. It is nullptr if the list is empty.

    /**
     * \brief   Request to Response map. All requests are accessed by index 
     *          and every request indexed is calculated by calculating 
     *          ('request ID' - areg::FuncIdRange::RequestFirstId)
     *          Every request should have appropriate response value. If request does not
     *          have response, it should have value areg::RESPONSE_ID_NONE
     *          The size of this map should be equal to idRequestCount.
     **/
    const uint32_t* idRequestToResponseMap{ nullptr };

    /**
     * \brief   Map of parameter count in every response. Every response index
     *          is calculated by formula ('response ID' - areg::FuncIdRange::ResponseFirstId)
     *          The size of this map should be equal to idResponseCount.
     **/
    const uint32_t* idResponseParamCountMap{ nullptr };
};

//////////////////////////////////////////////////////////////////////////
// Empty service interface
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The name of empty service
 **/
constexpr char  EmptyServiceName[]  { "AREG_EmptyService_" };
/**
 * \brief   Empty service version
 **/
constexpr Version EmptyServiceVersion   { 1, 0, 0 };
/**
 * \brief   Returns the empty local service interface.
 *
 * \return  Returns a reference to the empty interface.
 **/
AREG_API const InterfaceData& empty_interface() noexcept;

//////////////////////////////////////////////////////////////////////////
// Invalid service
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Invalid service name
 **/
constexpr char InvalidServiceName[] { "" };

//////////////////////////////////////////////////////////////////////////
// ParameterArray class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Manages parameter state information for all responses; tracks state for each
 *          response parameter indexed by response ID.
 *
 * \example Parameter Array
 *
 *          For example, if there is a response function prototype
 *          response_areg(const bool &param1, const int32_t &param2)
 *          the param1 state can be accessed by the index pair:
 *          param1State = states[areg::resp_index(UPD_ID_response_areg)][0];
 **/
class AREG_API ParameterArray
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes the parameter array from interface data containing parameter count mapping.
     **/
    ParameterArray(const areg::InterfaceData& ifData);

    /**
     * \brief   Initializes the parameter array from a parameter count map.
     **/
    ParameterArray(const uint32_t* paramCountMap, int32_t count);

    ParameterArray( ParameterArray && src ) noexcept;

    ~ParameterArray();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    ParameterArray & operator = ( ParameterArray && src ) noexcept;

    /**
     * \brief   Returns writable access to the state array at the given index.
     **/
    [[nodiscard]]
    inline areg::StateArray & operator [] (uint32_t index) noexcept;
    /**
     * \brief   Returns read-only access to the state array at the given index.
     **/
    [[nodiscard]]
    inline const areg::StateArray & operator [] (uint32_t index) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the state of a parameter at the given response and parameter indices.
     *
     * \param   row     The response index (areg::resp_index(response_id)).
     * \param   col     The parameter index (zero-based).
     **/
    [[nodiscard]]
    inline areg::DataState at(uint32_t row, uint32_t col) const noexcept;

    /**
     * \brief   Sets the state of a parameter at the given response and parameter indices.
     *
     * \param   row         The response index (areg::resp_index(response_id)).
     * \param   col         The parameter index (zero-based).
     **/
    inline void set_value_at(uint32_t row, uint32_t col, areg::DataState newValue) noexcept;

    /**
     * \brief   Returns true if the response at the given index has parameters.
     **/
    [[nodiscard]]
    inline bool has_parameters(uint32_t whichResp) const noexcept;
    /**
     * \brief   Returns true if the given response index is valid.
     **/
    [[nodiscard]]
    inline bool is_valid_index(uint32_t whichResp) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Resets all parameter states to their initial values.
     **/
    inline void reset() noexcept;

    /**
     * \brief   Sets all parameter states for the response at the given index.
     *
     * \param   whichParam      The response index (areg::resp_index(response_id)).
     * \param   newState        The state to set for all parameters.
     **/
    inline void set_param_state(uint32_t whichParam, areg::DataState newState) noexcept;

    /**
     * \brief   Resets parameter states for the response at the given index.
     *
     * \param   whichParam      The response index (areg::resp_index(response_id)).
     **/
    inline void reset(uint32_t whichParam) noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Constructs the parameter array from a parameter count map.
     *
     * \param   params      The parameter count map.
     * \param   count       The number of entries in the map.
     **/
    void construct(const uint32_t* params, uint32_t count) noexcept;

    /**
     * \brief   Returns the size in bytes required to store parameter states.
     *
     * \param   params      The parameter count map.
     * \param   count       The number of entries in the map.
     **/
    inline uint32_t count_param_space(const uint32_t* params, uint32_t count) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Number of entries in parameter array.
     *          Should be equal to number of response in service interface
     **/
    int32_t             mElemCount;

    /**
     * \brief   Table of parameter states
     **/
    areg::StateArray**  mParamList;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ParameterArray() = delete;
    AREG_NOCOPY( ParameterArray );
};

/**
 * \brief   Type of parameter array
 **/
using ParamState    = areg::ParameterArray;
/**
 * \brief   Type of state array
 **/
using AttrState     = areg::StateArray;

//////////////////////////////////////////////////////////////////////////
// areg::ProxyData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Container for proxy-related data derived from service interface metadata.
 **/
class AREG_API ProxyData
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes proxy data from the given service interface metadata.
     **/
    inline ProxyData(const areg::InterfaceData & ifData);

    ~ProxyData() = default;

public:

    /**
     * \brief   Resets all proxy data state to default values.
     **/
    inline void reset() noexcept;

    /**
     * \brief   Sets the data state for a given message (attribute or response) ID.
     *          Automatically determines whether the ID is an attribute or response and converts
     *          it to an index.
     *
     * \param   msgId       A function ID in the areg::FuncIdRange (attribute or response
     *                      ID). Invalid IDs are ignored. areg::ATTRIBUTE_SI_VERSION is
     *                      handled specially.
     * \param   newState    The state to set. For response IDs, the state is set for all
     *                      response parameters.
     **/
    inline void set_data_state(uint32_t msgId, areg::DataState newState) noexcept;

    /**
     * \brief   Returns the data state of the given message (attribute or response) ID.
     * \param   msgId       The message ID to query.
     **/
    [[nodiscard]]
    inline areg::DataState data_state(uint32_t msgId) const noexcept;

    /**
     * \brief   Returns the response message ID corresponding to the given request message ID.
     *
     * \param   reqId       The request message ID to map.
     * \return  The corresponding response ID, or areg::INVALID_MESSAGE_ID if the request
     *          ID is invalid or not mapped.
     **/
    [[nodiscard]]
    inline uint32_t response_id(uint32_t reqId) const noexcept;

    /**
     * \brief   Returns the data state of the given attribute ID.
     *
     * \param   msgId       The attribute message ID to query.
     * \return  The data state of the specified attribute.
     **/
    [[nodiscard]]
    inline areg::DataState attribute_state(uint32_t msgId) const noexcept;

    /**
     * \brief   Returns the parameter state of the given response ID.
     *
     * \param   msgId       The response message ID to query.
     * \return  The parameter state of the specified response.
     **/
    [[nodiscard]]
    inline areg::DataState param_state(uint32_t msgId) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
public:

    areg::DataState             mImplVersion;   //!< Implementation version
    const areg::InterfaceData&  mIfData;        //!< Service Interface data
    areg::AttrState             mAttrState;     //!< Table of attribute states
    areg::ParamState            mParamState;    //!< Table of response parameter state

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ProxyData() = delete;
    AREG_NOCOPY_NOMOVE( ProxyData );
};


/**
 * \brief   Service connected instance of application.
 **/
struct ConnectedInstance
{
    areg::MessageSource     ciSource    { areg::MessageSource::SourceUndefined };   //!< The type of the application
    areg::InstanceBitness   ciBitness   { areg::InstanceBitness::BitnessUnknown };  //!< The bit-set of connected instance
    ITEM_ID                 ciCookie    { areg::COOKIE_UNKNOWN };   //!< The cookie of the connected instance.
    TIME64                  ciTimestamp { 0 };  //!< The connection timestamp
    std::string             ciInstance  { "" }; //!< The name of the application
    std::string             ciLocation  { "" }; //!< The optional file location
};

/**
 * \brief   Sends a pre-serialized MessageEnvelope directly to the IPC send thread,
 *          bypassing all event dispatch and serialization overhead.
 *          The caller is responsible for ensuring the message was built while the
 *          connection was valid and the target cookie is still active.
 *
 * \param   msg     The pre-built message to send.
 * \return  Returns true if the message was accepted by the send thread.
 **/
AREG_API bool send_raw_message(const areg::MessageEnvelope& msg) noexcept;

/**
 * \brief   Returns the active IPC connection channel used to route messages
 *          to the message router.  Valid only after the connection handshake
 *          completes; check is_manager_started() before using.
 *
 * \return  Const reference to the current Channel object.
 **/
AREG_API const areg::Channel& connection_channel() noexcept;

/**
 * \brief   The map of key-value connected instances, where the key is an instance ID and the value is connected instance information.
 **/
using MapInstances = OrderedMap<ITEM_ID, areg::ConnectedInstance>;

//////////////////////////////////////////////////////////////////////////
// Global namespace areg inline function implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_STREAMABLE(areg::ResultType)
AREG_IMPLEMENT_STREAMABLE(areg::DataState)
AREG_IMPLEMENT_STREAMABLE(areg::RequestType)
AREG_IMPLEMENT_STREAMABLE(areg::MessageDataType)
AREG_IMPLEMENT_STREAMABLE(areg::ServiceConnectionState)
AREG_IMPLEMENT_STREAMABLE(areg::DisconnectReason)
AREG_IMPLEMENT_STREAMABLE(areg::RegistrationAction)
AREG_IMPLEMENT_STREAMABLE(areg::ServiceType)
AREG_IMPLEMENT_STREAMABLE(areg::InstanceBitness)
AREG_IMPLEMENT_STREAMABLE(areg::MessageSource)
AREG_IMPLEMENT_STREAMABLE(areg::FuncIdRange)

//////////////////////////////////////////////////////////////////////////
// class areg::ConnectedInstance serialization
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Serializes the instance structure to the stream.
 * \param   stream  The streaming object to serialize.
 * \param   output  The single structure of instance to serialize.
 **/
inline areg::OutStream& operator << (areg::OutStream& stream, const areg::ConnectedInstance& output)
{
    stream << output.ciSource << output.ciBitness << output.ciCookie << output.ciTimestamp << output.ciInstance << output.ciLocation;
    return stream;
}

/**
 * \brief   De-serializes the instance structure from the stream.
 * \param   stream  The streaming object that contains the information of the connected instance.
 * \param   input  The single structure of instance to initialize.
 **/
inline const areg::InStream& operator >> (const areg::InStream& stream, areg::ConnectedInstance& input)
{
    stream >> input.ciSource >> input.ciBitness >> input.ciCookie >> input.ciTimestamp >> input.ciInstance >> input.ciLocation;
    return stream;
}

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// namespace areg inline function implementation
//////////////////////////////////////////////////////////////////////////

inline constexpr bool areg::is_service_connected(areg::ServiceConnectionState state) noexcept
{
    return (state == areg::ServiceConnectionState::Connected);
}

inline constexpr bool areg::is_connection_pending( areg::ServiceConnectionState state ) noexcept
{
    return (state == areg::ServiceConnectionState::Pending);
}

inline constexpr bool areg::is_service_rejected( areg::ServiceConnectionState state ) noexcept
{
    return (state == areg::ServiceConnectionState::Rejected);
}

inline constexpr bool areg::is_connection_lost( areg::ServiceConnectionState state ) noexcept
{
    return (state == areg::ServiceConnectionState::ConnectionLost);
}

inline constexpr bool areg::is_service_disconnected( areg::ServiceConnectionState state ) noexcept
{
    return (state != areg::ServiceConnectionState::Connected);
}

inline constexpr areg::ServiceConnectionState areg::service_connection( areg::DisconnectReason reason ) noexcept
{
    switch ( reason )
    {
    case areg::DisconnectReason::UndefinedReason:
        return areg::ServiceConnectionState::Unknown;
    
    case areg::DisconnectReason::ServiceDisconnected:
    case areg::DisconnectReason::ServiceLost:
    case areg::DisconnectReason::ProviderLost:
    case areg::DisconnectReason::ConsumerLost:
    case areg::DisconnectReason::ClientConnectionLost:
        return areg::ServiceConnectionState::ConnectionLost;

    case areg::DisconnectReason::ServiceRejected:
    case areg::DisconnectReason::ProviderRejected:
    case areg::DisconnectReason::ConsumerNotSupported:
        return areg::ServiceConnectionState::Rejected;

    case areg::DisconnectReason::ConsumerDisconnected:
    case areg::DisconnectReason::ProviderDisconnected:
    case areg::DisconnectReason::ClientConnectionClosed:
        return areg::ServiceConnectionState::Disconnected;

    case areg::DisconnectReason::SystemShutdown:
        return areg::ServiceConnectionState::Shutdown;

    default:
        return areg::ServiceConnectionState::Connected;
    }
}

inline constexpr uint32_t areg::req_index(uint32_t msgId) noexcept
{
    return (msgId != static_cast<uint32_t>(FuncIdRange::EmptyFunctionId))
                        ? msgId - static_cast<uint32_t>(FuncIdRange::RequestFirstId)
                        : static_cast<uint32_t>(areg::INVALID_INDEX);
}

inline constexpr uint32_t areg::resp_index(uint32_t msgId) noexcept
{
    return (msgId != static_cast<uint32_t>(areg::FuncIdRange::EmptyFunctionId))
                        ? msgId - static_cast<uint32_t>(areg::FuncIdRange::ResponseFirstId)
                        : static_cast<uint32_t>(areg::INVALID_INDEX);
}

inline constexpr uint32_t areg::attr_index(uint32_t msgId) noexcept
{
    return (msgId != static_cast<uint32_t>(FuncIdRange::EmptyFunctionId))
                        ? msgId - static_cast<uint32_t>(FuncIdRange::AttributeFirstId)
                        : static_cast<uint32_t>(areg::INVALID_INDEX);
}

inline constexpr bool areg::is_request_id(uint32_t msgId) noexcept
{
    return ((msgId & static_cast<uint32_t>(areg::ServiceCallType::RequestFunction)) != 0);
}

inline constexpr bool areg::is_response_id(uint32_t msgId) noexcept
{
    return ((msgId & static_cast<uint32_t>(areg::ServiceCallType::ResponseFunction)) != 0);
}

inline constexpr bool areg::is_attribute_id(uint32_t msgId) noexcept
{
    return ((msgId & static_cast<uint32_t>(areg::ServiceCallType::AttributeUpdate)) != 0);
}

inline constexpr bool areg::is_registry_id( uint32_t msgId ) noexcept
{
    return ((msgId & static_cast<uint32_t>(areg::ServiceCallType::ServiceRegisteration)) != 0);
}

inline constexpr bool areg::is_empty_id(uint32_t msgId) noexcept
{
    return (msgId == static_cast<uint32_t>(areg::FuncIdRange::EmptyFunctionId));
}

inline constexpr bool areg::is_version_id( uint32_t msgId ) noexcept
{
    return (msgId == static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderVersion));
}

inline constexpr bool areg::is_connect_id( uint32_t msgId ) noexcept
{
    return (msgId == static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection));
}

inline constexpr bool areg::is_executable_id(uint32_t msgId) noexcept
{
    return ( (msgId  & areg::EXECUTABLE_MESSAGE) != 0) || (msgId == static_cast<uint32_t>(areg::ServiceCallType::NoFunction) );
}


inline constexpr areg::MessageDataType areg::message_data_type( uint32_t msgId ) noexcept
{
    if ( areg::is_request_id(msgId) )
        return areg::MessageDataType::RequestData;
    else if (areg::is_response_id(msgId))
        return areg::MessageDataType::ResponseData;
    else if (areg::is_attribute_id(msgId))
        return areg::MessageDataType::AttributeData;
    else if (areg::is_registry_id(msgId))
        return areg::MessageDataType::ServiceData;
    else
        return areg::MessageDataType::UndefinedData;
}

//////////////////////////////////////////////////////////////////////////
// class areg::StateArray inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const areg::DataState& areg::StateArray::operator [] (uint32_t index) const noexcept
{
    return StateArrayBase::operator[](index);
}

inline areg::DataState& areg::StateArray::operator [] (uint32_t index) noexcept
{
    return StateArrayBase::operator[](index);
}

inline uint32_t areg::StateArray::size() const noexcept
{
    return StateArrayBase::size();
}

inline void areg::StateArray::reset() noexcept
{
    set_all_state(areg::DataState::DataIsUnavailable);
}

inline bool areg::StateArray::has_params() const noexcept
{
    return (is_empty() == false);
}

inline void areg::StateArray::set_state(int32_t whichIndex, areg::DataState newState) noexcept
{
    mValueList[whichIndex] = newState;
}

inline void areg::StateArray::set_all_state(areg::DataState newState) noexcept
{
    for ( uint32_t i = 0; i < mElemCount; ++ i )
        mValueList[i] = newState;
}

//////////////////////////////////////////////////////////////////////////
// class areg::ParameterArray inline function implementation
//////////////////////////////////////////////////////////////////////////

inline areg::StateArray& areg::ParameterArray::operator [] ( uint32_t index ) noexcept
{
    ASSERT(is_valid_index(index));
    return *(mParamList[index]);
}

inline const areg::StateArray& areg::ParameterArray::operator [] ( uint32_t index ) const noexcept
{
    ASSERT(is_valid_index(index));
    return *(mParamList[index]);
}

inline areg::DataState areg::ParameterArray::at( uint32_t row, uint32_t col ) const noexcept
{
    ASSERT(is_valid_index(row) && mParamList[row]->is_valid_index(static_cast<uint32_t>(col)));
    return mParamList[row]->value_at(static_cast<uint32_t>(col));
}

inline void areg::ParameterArray::set_value_at( uint32_t row, uint32_t col, areg::DataState newValue ) noexcept
{
    ASSERT(is_valid_index(row) && mParamList[row]->is_valid_index(static_cast<uint32_t>(col)));
    mParamList[row]->set_value_at(static_cast<uint32_t>(col), newValue);
}

inline bool areg::ParameterArray::has_parameters( uint32_t whichResp ) const noexcept
{
    ASSERT(is_valid_index(whichResp));
    return mParamList[whichResp]->has_params();
}

inline void areg::ParameterArray::reset() noexcept
{
    for (int col = 0; col < mElemCount; ++col)
    {
        mParamList[col]->reset();
    }
}

inline bool areg::ParameterArray::is_valid_index(uint32_t whichResp) const noexcept
{
    return ((static_cast<int32_t>(whichResp) >= 0) && (static_cast<int32_t>(whichResp) < mElemCount));
}

inline void areg::ParameterArray::set_param_state(uint32_t whichParam, areg::DataState newState) noexcept
{
    ASSERT(is_valid_index(whichParam));
    mParamList[whichParam]->set_all_state(newState);
}

inline uint32_t areg::ParameterArray::count_param_space(const uint32_t* params, uint32_t count) noexcept
{
    uint32_t result = 0;
    // space for size of class areg::StateArray + 
    // space for size of areg::DataState multiplied on number of parameters.
    // If number of parameters is zero, do not reserve.
    for (uint32_t i = 0; i < count; ++i)
    {
        result += params[i] != 0 ? static_cast<uint32_t>(sizeof(areg::StateArray) + params[i] * sizeof(areg::DataState)) : 0;
    }

    return result;
}

inline void areg::ParameterArray::reset(uint32_t whichParam) noexcept
{
    ASSERT((static_cast<int32_t>(whichParam) >= 0) && (static_cast<int32_t>(whichParam) < mElemCount));
    mParamList[whichParam]->reset();
}

//////////////////////////////////////////////////////////////////////////
// class areg::ProxyData inline function implementation
//////////////////////////////////////////////////////////////////////////

inline areg::ProxyData::ProxyData(const areg::InterfaceData& ifData)
    : mImplVersion(areg::DataState::DataIsUnavailable)
    , mIfData     (ifData)
    , mAttrState  (static_cast<uint32_t>(ifData.idAttributeCount))
    , mParamState (ifData)
{
    reset();
}

inline void areg::ProxyData::reset() noexcept
{
    mImplVersion = areg::DataState::DataIsUnavailable;
    mAttrState.reset();
    mParamState.reset();
}

inline void areg::ProxyData::set_data_state(uint32_t msgId, areg::DataState newState) noexcept
{
    if (areg::is_attribute_id(msgId))
    {
        if (areg::is_version_id(msgId))
        {
            mImplVersion = newState;
        }
        else
        {
            mAttrState[areg::attr_index(msgId)] = newState;
        }
    }
    else if (areg::is_response_id(msgId))
    {
        mParamState.set_param_state(areg::resp_index(msgId), newState);
    }
    // else ignore
}

inline areg::DataState areg::ProxyData::data_state(uint32_t msgId) const noexcept
{
    areg::DataState result = areg::DataState::DataUnexpectedError;
    if (areg::is_attribute_id(msgId))
        result = attribute_state(msgId);
    else if (areg::is_response_id(msgId))
        result = param_state(msgId);
    // else, ignore

    return result;
}

inline uint32_t areg::ProxyData::response_id(uint32_t reqId) const noexcept
{
    uint32_t index = areg::req_index(reqId);
    return  ((static_cast<int32_t>(index) >= 0) && (index < mIfData.idRequestCount)
                    ? static_cast<uint32_t>(mIfData.idRequestToResponseMap[index])
                    : areg::INVALID_MESSAGE_ID);
}

inline areg::DataState areg::ProxyData::attribute_state( uint32_t msgId ) const noexcept
{
    return areg::is_version_id(msgId) ? mImplVersion : mAttrState[areg::attr_index(msgId)];
}

inline areg::DataState areg::ProxyData::param_state( uint32_t msgId ) const noexcept
{
    const areg::StateArray& param = mParamState[areg::resp_index(msgId)];
    return (param.has_params() ? param[0u] : areg::DataState::DataIsUnavailable);
}

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

#endif  // AREG_COMPONENT_SERVICEDEFS_HPP
