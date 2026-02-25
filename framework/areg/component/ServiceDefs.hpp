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
 * \brief       Areg Platform, NEService namespace contains
 *              collection of classes, structures and types defining
 *              Service Interface.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IOStream.hpp"
#include "areg/base/OrderedMap.hpp"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/FixedArray.hpp"
#include "areg/base/Version.hpp"

/************************************************************************
 * predefined macro
 ************************************************************************/

/**
 * \brief       NEService namespace contains defined and fixed constants,
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
// NEService namespace declaration
//////////////////////////////////////////////////////////////////////////
namespace NEService
{
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
        , Invalid     = 32897 /*0x8081*/  //!< service invalid (check cookie).        Bits: 1000 0000 1000 0001
        , ServiceRejected    = 32899 /*0x8083*/  //!< service rejected (unsupported).        Bits: 1000 0000 1000 0011
    };

    /**
     * \brief   Returns string representation of ResultType.
     *
     * \param   resultType      The result type to convert.
     * \return  Returns the string value.
     **/
    inline const char* as_string(NEService::ResultType resultType);

    /**
     * \brief   Data types
     *          Used getting data
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
     *
     * \param   dataState       The data state to convert.
     * \return  Returns the string value.
     **/
    inline const char* as_string(NEService::DataState dataState);

    /**
     * \brief   Type of request.
     *          Used sending request event by proxy
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
     *
     * \param   resultType      The request type to convert.
     * \return  Returns the string value.
     **/
    inline const char* as_string( NEService::RequestType resultType );

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
     *
     * \param   dataType    The message data type to convert.
     * \return  Returns the string value.
     **/
    inline const char* as_string( NEService::MessageDataType dataType );

    /**
     * \brief   Returns message data type from message ID.
     *
     * \param   msgId       The message ID.
     * \return  Returns the message data type.
     **/
    inline NEService::MessageDataType message_data_type( uint32_t msgId );

    /**
     * \brief   NEService::ServiceConnectionState
     *          Service Connections. Used in service calls
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
     *
     * \param   service_connection      The connection state to convert.
     * \return  Returns the string value.
     **/
    inline const char* as_string( NEService::ServiceConnectionState service_connection );

    /**
     * \brief   Returns true if service is connected.
     *
     * \param   connectionStatus    The connection status.
     **/
    inline bool is_service_connected( NEService::ServiceConnectionState connectionStatus );

    /**
     * \brief   Returns true if service connection is pending.
     *
     * \param   connectionStatus    The connection status.
     **/
    inline bool is_connection_pending( NEService::ServiceConnectionState connectionStatus );

    /**
     * \brief   Returns true if service connection is rejected.
     *
     * \param   connectionStatus    The connection status.
     **/
    inline bool is_service_rejected( NEService::ServiceConnectionState connectionStatus );

    /**
     * \brief   Returns true if service connection is lost.
     *
     * \param   connectionStatus    The connection status.
     **/
    inline bool is_connection_lost( NEService::ServiceConnectionState connectionStatus );

    /**
     * \brief   Returns true if service is not connected.
     *
     * \param   connectionStatus    The connection status.
     **/
    inline bool is_service_disconnected( NEService::ServiceConnectionState connectionStatus );

    /**
     * \brief   NEService::DisconnectReason
     *          The service provider and service consumer disconnect reason.
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
     *
     * \param   reason      The disconnect reason.
     * \return  Returns the string value.
     **/
    inline const char * as_string( NEService::DisconnectReason reason );

    /**
     * \brief   Returns service connection state from disconnect reason.
     *
     * \param   reason      The disconnect reason.
     * \return  Returns the connection state.
     **/
    inline NEService::ServiceConnectionState service_connection( NEService::DisconnectReason reason );

    /**
     * \brief   NEService::RegistrationAction
     *          The service request types.
     **/
    enum class RegistrationAction   : uint16_t
    {
          RegisterClient    = 0x0010    //!< Client requests to register.           Bit set: 0001 0000
        , UnregisterClient  = 0x0011    //!< Client requests to unregister.         Bit set: 0001 0001
        , RegisterStub      = 0x0020    //!< Server requests to register.           Bit set: 0010 0000
        , UnregisterStub    = 0x0021    //!< Server requests to unregister.         Bit set: 0010 0001
    };
    
    /**
     * \brief   Returns string representation of RegistrationAction.
     *
     * \param   svcRequestType      The registration action.
     * \return  Returns the string value.
     **/
    inline const char * as_string( NEService::RegistrationAction svcRequestType );

    /**
     * \brief   NEService::ServiceType
     *          Service Type. Either local or Remote.
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
     *
     * \param   srvcType    The service type.
     * \return  Returns the string value.
     **/
    inline const char * as_string( NEService::ServiceType srvcType );

    /**
     * \brief   NEService::MessageSource
     *          The source of the communication message
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
     *
     * \param   msgSource       The message source.
     * \return  Returns the string value.
     **/
    inline const char * as_string(NEService::MessageSource msgSource);

    /**
     * \brief   NEService::InstanceBitness
     *          The bitness of the executable instance.
     **/
    enum class InstanceBitness  : uint16_t
    {
          BitnessUnknown    = 0     //!< Unknown bitness
        , Bitness32         = 32    //!< 32-bit system
        , Bitness64         = 64    //!< 64-bit system
    };

    /**
     * \brief   Returns string representation of InstanceBitness.
     *
     * \param   bitness     The bitness.
     * \return  Returns the string value.
     **/
    inline const char* as_string(NEService::InstanceBitness bitness);

    /**
     * \brief   NEService::SEQUENCE_NUMBER_NOTIFY
     *          Sequence number predefining notification message ID
     **/
    constexpr SequenceNumber    SEQUENCE_NUMBER_NOTIFY  { static_cast<SequenceNumber>(0) };    /*0x00000000*/
    /**
     * \brief   NEService::SEQUENCE_NUMBER_ANY
     *          Any sequence number, used in messages. "Any sequence number" used to find any listener object with same message ID.
     **/
    constexpr SequenceNumber    SEQUENCE_NUMBER_ANY     { static_cast<SequenceNumber>(~0) };    /*0xFFFFFFFF*/

    /**
     * \brief   NEService::COOKIE_UNKNOWN
     *          Unknown cookie
     **/
    constexpr ITEM_ID   COOKIE_UNKNOWN              { static_cast<ITEM_ID>(NECommon::Cookie::Invalid) };
    /**
     * \brief   NEService::COOKIE_LOCAL
     *          The indication of local service.
     **/
    constexpr ITEM_ID   COOKIE_LOCAL                { static_cast<ITEM_ID>(NECommon::Cookie::Local) };
    /**
     * \brief   NEService::COOKIE_ROUTER
     *          Indicates message router cookie
     **/
    constexpr ITEM_ID   COOKIE_ROUTER               { static_cast<ITEM_ID>(NECommon::Cookie::Router) };
    /**
     * \brief   NEService::COOKIE_LOGGER
     *          Indicates log collector cookie
     **/
    constexpr ITEM_ID   COOKIE_LOGGER               { static_cast<ITEM_ID>(NECommon::Cookie::Logger) };
    /**
     * \brief   NEService::COOKIE_ANY
     *          Indicates any valid cookie
     **/
    constexpr ITEM_ID   COOKIE_ANY                  { static_cast<ITEM_ID>(NECommon::Cookie::Any) };
    /**
     * \brief   NEService::TARGET_UNKNOWN
     *          The unknown target ID
     **/
    constexpr ITEM_ID   TARGET_UNKNOWN              { static_cast<ITEM_ID>(NECommon::Cookie::Invalid) };
    /**
     * \brief   NEService::TARGET_LOCAL
     *          The local target ID
     **/
    constexpr ITEM_ID   TARGET_LOCAL                { static_cast<ITEM_ID>(NECommon::Cookie::Local) };
    /**
     * \brief   NEService::TARGET_ALL
     *          The undefined (all) target ID
     **/
    constexpr ITEM_ID   TARGET_ALL                  { static_cast<ITEM_ID>(NEService::COOKIE_ANY) };
    /**
     * \brief   NEService::SOURCE_UNKNOWN
     *          The unknown source ID
     **/
    constexpr ITEM_ID   SOURCE_UNKNOWN              { static_cast<ITEM_ID>(NECommon::Cookie::Invalid) };
    /**
     * \brief   NEService::SOURCE_UNKNOWN
     *          The unknown source ID.
     **/
    constexpr ITEM_ID   SOURCE_LOCAL                { static_cast<ITEM_ID>(NECommon::Cookie::Local) };
    /**
     * \brief   NEService::COOKIE_REMOTE_SERVICE
     *          The ID of first valid remote cookie.
     **/
    constexpr ITEM_ID   COOKIE_REMOTE_SERVICE       { static_cast<ITEM_ID>(NECommon::Cookie::FirstRemote) };

    /**
     * \brief   NEService::ServiceCallType
     *          Specifies the service call type
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
     * \brief   NEService::REQUEST_ID_FIRST
     *          The first ID in request call.
     **/
    constexpr uint32_t  REQUEST_ID_FIRST    { static_cast<uint32_t>(ServiceCallType::RequestFunction) };
    /**
     * \brief   NEService::REQUEST_ID_LAST
     *          The last ID in request call.
     **/
    constexpr uint32_t  REQUEST_ID_LAST     { REQUEST_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   NEService::RESPONSE_ID_FIRST
     *          The first ID in response call.
     **/
    constexpr uint32_t  RESPONSE_ID_FIRST   { static_cast<uint32_t>(ServiceCallType::ResponseFunction) };
    /**
     * \brief   NEService::RESPONSE_ID_LAST
     *          The last ID in response call.
     **/
    constexpr uint32_t  RESPONSE_ID_LAST    { RESPONSE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   NEService::ATTRIBUTE_ID_FIRST
     *          The first ID in attribute call.
     **/
    constexpr uint32_t  ATTRIBUTE_ID_FIRST  { static_cast<uint32_t>(ServiceCallType::AttributeUpdate) };
    /**
     * \brief   NEService::ATTRIBUTE_ID_LAST
     *          The last ID in attribute call.
     **/
    constexpr uint32_t  ATTRIBUTE_ID_LAST   { ATTRIBUTE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   NEService::SERVICE_ID_FIRST
     *          The last ID in service call.
     **/
    constexpr uint32_t  SERVICE_ID_FIRST    { static_cast<uint32_t>(NEService::ServiceCallType::ServiceRegisteration) };
    /**
     * \brief   NEService::SERVICE_ID_LAST
     *          The last ID in service call.
     **/
    constexpr uint32_t  SERVICE_ID_LAST     { SERVICE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   NEService::RESPONSE_ID_NONE
     *          Constant no response. Used to indicate that the request has no response.
     **/
    constexpr uint32_t  RESPONSE_ID_NONE    { static_cast<uint32_t>(ServiceCallType::NoFunction) };

    /**
     * \brief   NEService::INVALID_MESSAGE_ID
     *          The invalid message ID
     **/
    constexpr uint32_t  INVALID_MESSAGE_ID  { static_cast<uint32_t>(~0) };    /*0xFFFFFFFF*/

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
     *
     * \param   funcId      The function ID range.
     * \return  Returns the string value.
     **/
    inline const char * as_string( NEService::FuncIdRange funcId );

    /**
     * \brief   Converts request message ID to array index.
     *
     * \param   msgId       The message ID.
     * \return  Returns the array index; INVALID_INDEX if empty.
     **/
    inline constexpr uint32_t req_index(uint32_t msgId)
    {
        return (msgId != static_cast<uint32_t>(FuncIdRange::EmptyFunctionId))
                ? msgId - static_cast<uint32_t>(FuncIdRange::RequestFirstId)
                : static_cast<uint32_t>(NECommon::INVALID_INDEX);
    }

    /**
     * \brief   Converts response message ID to array index.
     *
     * \param   msgId       The message ID.
     * \return  Returns the array index; INVALID_INDEX if empty.
     **/
    inline constexpr uint32_t resp_index(uint32_t msgId)
    {
        return (msgId != static_cast<uint32_t>(FuncIdRange::EmptyFunctionId))
                ? msgId - static_cast<uint32_t>(FuncIdRange::ResponseFirstId)
                : static_cast<uint32_t>(NECommon::INVALID_INDEX);
    }

    /**
     * \brief   Converts attribute message ID to array index.
     *
     * \param   msgId       The message ID.
     * \return  Returns the array index; INVALID_INDEX if empty.
     **/
    inline constexpr uint32_t attr_index(uint32_t msgId)
    {
        return (msgId != static_cast<uint32_t>(FuncIdRange::EmptyFunctionId))
                ? msgId - static_cast<uint32_t>(FuncIdRange::AttributeFirstId)
                : static_cast<uint32_t>(NECommon::INVALID_INDEX);
    }

    /**
     * \brief   Returns true if message ID is an attribute call.
     *
     * \param   msgId       The message ID.
     **/
    inline bool is_attribute_id(uint32_t msgId);

    /**
     * \brief   Returns true if message ID is a response call.
     *
     * \param   msgId       The message ID.
     **/
    inline bool is_response_id(uint32_t msgId);

    /**
     * \brief   Returns true if message ID is a request call.
     *
     * \param   msgId       The message ID.
     **/
    inline bool is_request_id(uint32_t msgId);

    /**
     * \brief   Returns true if message ID is a service registration call.
     *
     * \param   msgId       The message ID.
     **/
    inline bool is_registry_id( uint32_t msgId );

    /**
     * \brief   Returns true if message ID is a version notification.
     *
     * \param   msgId       The message ID.
     **/
    inline bool is_version_id(uint32_t msgId);

    /**
     * \brief   Returns true if message ID is a service connect notification.
     *
     * \param   msgId       The message ID.
     **/
    inline bool is_connect_id(uint32_t msgId);

    /**
     * \brief   Returns true if message ID is a request, response, or attribute notification.
     *
     * \param   msgId       The message ID.
     **/
    inline bool is_executable_id( uint32_t msgId );

    /**
     * \brief   Returns true if message ID is an empty function.
     *
     * \param   msgId       The message ID.
     **/
    inline bool is_empty_id(uint32_t msgId);

    class ParameterArray;

    //////////////////////////////////////////////////////////////////////////
    // StateArray class declaration
    //////////////////////////////////////////////////////////////////////////
    using StateArrayBase    = FixedArray<NEService::DataState>;
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
         * \brief   Initializes a fixed-size array with the given capacity. The size cannot be
         *          changed dynamically.
         *
         * \param   count       The number of state elements in the array.
         **/
        StateArray(uint32_t count);

        /**
         * \brief   Initializes the array to reference an existing buffer without making a copy.
         *
         * \param   thisBffer       The existing buffer to reference.
         * \param   elemCount       The number of elements in the buffer.
         **/
        StateArray(uint8_t* thisBffer, int32_t elemCount);

        /**
         * \brief   Destructor
         **/
        ~StateArray();

    //////////////////////////////////////////////////////////////////////////
    // Operators, attributes and operations
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Returns a read-only reference to the element at the specified index. The index
         *          must be valid.
         *
         * \param   index       The array index (must be less than array size).
         **/
        inline const NEService::DataState& operator [] (uint32_t index) const;
        /**
         * \brief   Returns a modifiable reference to the element at the specified index. The index
         *          must be valid.
         *
         * \param   index       The array index (must be less than array size).
         **/
        inline NEService::DataState& operator [] (uint32_t index);

        /**
         * \brief   Returns the number of elements in the array.
         **/
        inline uint32_t size() const;
        /**
         * \brief   Resets all states in the array to NEService::DataIsUnavailable.
         **/
        inline void reset();

        /**
         * \brief   Returns true if the array contains parameters (size is not zero).
         **/
        inline bool has_params() const;

        /**
         * \brief   Sets the state of the element at the specified index.
         *
         * \param   whichIndex      The index of the element to modify.
         * \param   newState        The state to set.
         **/
        inline void set_state(int32_t whichIndex, NEService::DataState newState);

        /**
         * \brief   Sets all elements in the array to the specified state.
         *
         * \param   newState    The state to set for all elements.
         **/
        inline void set_all_state(NEService::DataState newState);

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
        /**
         * \brief
         **/
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
        /**
         * \brief   The name of service interface.
         **/
        const char *        idServiceName{ nullptr };

        /**
         * \brief   Service interface implementation version
         **/
        Version             idVersion{ 0, 0, 0 };

        /**
         * \brief   The service type. Should be either local or public / remote.
         **/
        ServiceType        idServiceType{ ServiceType::Invalid };

        /**
         * \brief   Number of requests in service interface
         **/
        uint32_t        idRequestCount{ 0u };

        /**
         * \brief   Number of responses in service interface, including broadcasts
         **/
        uint32_t        idResponseCount{ 0u };

        /**
         * \brief   Number of attributes in service interface
         **/
        uint32_t        idAttributeCount{ 0u };

        /**
         * \brief   The list of requests. It is nullptr if the list is empty.
         **/
        const uint32_t* idRequestList{ nullptr };

        /**
         * \brief   The list of responses. It is nullptr if the list is empty.
         **/
        const uint32_t* idResponseList{ nullptr };

        /**
         * \brief   The list of attributes. It is nullptr if the list is empty.
         **/
        const uint32_t* idAttributeList{ nullptr };

        /**
         * \brief   Request to Response map. All requests are accessed by index 
         *          and every request indexed is calculated by calculating 
         *          ('request ID' - NEService::FuncIdRange::RequestFirstId)
         *          Every request should have appropriate response value. If request does not
         *          have response, it should have value NEService::RESPONSE_ID_NONE
         *          The size of this map should be equal to idRequestCount.
         **/
        const uint32_t* idRequestToResponseMap{ nullptr };

        /**
         * \brief   Map of parameter count in every response. Every response index
         *          is calculated by formula ('response ID' - NEService::FuncIdRange::ResponseFirstId)
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
    extern AREG_API const Version EmptyServiceVersion /*(1, 0, 0)*/;
    /**
     * \brief   Returns the empty local service interface.
     *
     * \return  Returns a reference to the empty interface.
     **/
    AREG_API NEService::InterfaceData & empty_interface();

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
      * @\example    Parameter Array
      *
      *          For example, if there is a response function prototype
      *          responseAREG(const bool &param1, const int32_t &param2)
      *          the param1 state can be accessed by the index pair:
      *          param1State = states[NEService::resp_index(UPD_ID_responseAREG)][0];
     **/
    class AREG_API ParameterArray
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Initializes the parameter array from interface data containing parameter count
         *          mapping.
         *
         * \param   ifData      The service interface data containing parameter count mapping.
         **/
        ParameterArray(const NEService::InterfaceData& ifData);


        /**
         * \brief   Initializes the parameter array from a parameter count map.
         *
         * \param   paramCountMap       The map of parameter counts for each response.
         * \param   count               The number of entries in the map.
         **/
        ParameterArray(const uint32_t* paramCountMap, int32_t count);

        /**
         * \brief   Moves data from the given source.
         *
         * \param   src     The source of data to move.
         **/
        ParameterArray( ParameterArray && src ) noexcept;

        /**
         * \brief   Destructor
         **/
        ~ParameterArray();

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Moves data from the given source.
         *
         * \param   src     The source of data to move.
         **/
        ParameterArray & operator = ( ParameterArray && src ) noexcept;

        /**
         * \brief   Returns writable access to the state array at the given index.
         *
         * \param   index       The response index (calculated as response_id - ResponseFirstId or
         *                      via NEService::resp_index()).
         **/
        inline NEService::StateArray & operator [] (uint32_t index);
        /**
         * \brief   Returns read-only access to the state array at the given index.
         *
         * \param   index       The response index (calculated as response_id - ResponseFirstId or
         *                      via NEService::resp_index()).
         **/
        inline const NEService::StateArray & operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Returns the state of a parameter at the given response and parameter indices.
         *
         * \param   row     The response index (NEService::resp_index(response_id)).
         * \param   col     The parameter index (zero-based).
         * \return  The state of the parameter.
         **/
        inline NEService::DataState at(uint32_t row, uint32_t col) const;

        /**
         * \brief   Sets the state of a parameter at the given response and parameter indices.
         *
         * \param   row         The response index (NEService::resp_index(response_id)).
         * \param   col         The parameter index (zero-based).
         * \param   newValue    The state to set.
         **/
        inline void set_at(uint32_t row, uint32_t col, NEService::DataState newValue);

        /**
         * \brief   Returns true if the response at the given index has parameters.
         *
         * \param   whichRespIndex      The response index (NEService::resp_index(response_id)).
         **/
        inline bool has_parameters(uint32_t whichRespIndex) const;
        /**
         * \brief   Returns true if the given response index is valid.
         *
         * \param   whichRespIndex      The response index (NEService::resp_index(response_id)).
         * \return  Returns true if the index is valid; false otherwise.
         **/
        inline bool is_valid_index(uint32_t whichRespIndex) const;

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Resets all parameter states to their initial values.
         **/
        inline void reset();

        /**
         * \brief   Sets all parameter states for the response at the given index.
         *
         * \param   whichParam      The response index (NEService::resp_index(response_id)).
         * \param   newState        The state to set for all parameters.
         **/
        inline void set_param_state(uint32_t whichParam, NEService::DataState newState);

        /**
         * \brief   Resets parameter states for the response at the given index.
         *
         * \param   whichParam      The response index (NEService::resp_index(response_id)).
         **/
        void reset(uint32_t whichParam);

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
        void construct(const uint32_t* params, int32_t count);

        /**
         * \brief   Returns the size in bytes required to store parameter states.
         *
         * \param   params      The parameter count map.
         * \param   count       The number of entries in the map.
         **/
        uint32_t count_param_space(const uint32_t* params, int32_t count);

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    private:

        /**
         * \brief   Number of entries in parameter array.
         *          Should be equal to number of response in service interface
         **/
        int32_t                         mElemCount;

        /**
         * \brief   Table of parameter states
         **/
        NEService::StateArray **    mParamList;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief
         **/
        ParameterArray() = delete;
        AREG_NOCOPY( ParameterArray );
    };

    /**
     * \brief   Type of parameter array
     **/
    using ParamState    = NEService::ParameterArray;
    /**
     * \brief   Type of state array
     **/
    using AttrState     = NEService::StateArray;

    //////////////////////////////////////////////////////////////////////////
    // NEService::ProxyData class declaration
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
         *
         * \param   ifData      The service interface metadata object.
         **/
        ProxyData(const NEService::InterfaceData & ifData);
        /**
         * \brief   Destructor
         **/
        ~ProxyData() = default;

    public:

        /**
         * \brief   Resets all proxy data state to default values.
         **/
        void reset();

        /**
         * \brief   Sets the data state for a given message (attribute or response) ID.
         *          Automatically determines whether the ID is an attribute or response and converts
         *          it to an index.
         *
         * \param   msgId       A function ID in the NEService::FuncIdRange (attribute or response
         *                      ID). Invalid IDs are ignored. NEService::ATTRIBUTE_SI_VERSION is
         *                      handled specially.
         * \param   newState    The state to set. For response IDs, the state is set for all
         *                      response parameters.
         **/
        void set_data_state(uint32_t msgId, NEService::DataState newState);

        /**
         * \brief   Returns the data state of the given message (attribute or response) ID.
         *
         * \param   msgId       The message ID to query.
         * \return  The data state of the specified message.
         **/
        NEService::DataState data_state(uint32_t msgId) const;

        /**
         * \brief   Returns the response message ID corresponding to the given request message ID.
         *
         * \param   requestId       The request message ID to map.
         * \return  The corresponding response ID, or NEService::INVALID_MESSAGE_ID if the request
         *          ID is invalid or not mapped.
         **/
        uint32_t response_id(uint32_t requestId) const;

        /**
         * \brief   Returns the data state of the given attribute ID.
         *
         * \param   msgId       The attribute message ID to query.
         * \return  The data state of the specified attribute.
         **/
        inline NEService::DataState attribute_state(uint32_t msgId) const;

        /**
         * \brief   Returns the parameter state of the given response ID.
         *
         * \param   msgId       The response message ID to query.
         * \return  The parameter state of the specified response.
         **/
        inline NEService::DataState param_state(uint32_t msgId) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Implementation version
         **/
        NEService::DataState           mImplVersion;   // implementation version

        /**
         * \brief   Service Interface data
         **/
        const NEService::InterfaceData &   mIfData;        // fixed service interface data

        /**
         * \brief   Table of attribute states
         **/
        NEService::AttrState                mAttrState;     // state of attributes

        /**
         * \brief   Table of response parameter state
         **/
        NEService::ParamState               mParamState;    // state of parameters in response call.

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief
         **/
        ProxyData() = delete;
        AREG_NOCOPY_NOMOVE( ProxyData );
    };


    /**
     * \brief   NEService::ConnectedInstance
     *          Service connected instance of application.
     **/
    struct ConnectedInstance
    {
        //!< The type of the application
        NEService::MessageSource   ciSource    { NEService::MessageSource::SourceUndefined };
        //!< The bit-set of connected instance
        NEService::InstanceBitness ciBitness   { NEService::InstanceBitness::BitnessUnknown };
        //!< The cookie of the connected instance.
        ITEM_ID                     ciCookie    { NEService::COOKIE_UNKNOWN };
        //!< The connection timestamp
        TIME64                      ciTimestamp { 0 };
        //!< The name of the application
        std::string                 ciInstance  { "" };
        //!< The optional file location
        std::string                 ciLocation  { "" };
    };

    /**
     * \brief   The map of key-value connected instances, where the key is an instance ID and the value is connected instance information.
     **/
    using MapInstances = OrderedMap<ITEM_ID, NEService::ConnectedInstance>;
}

//////////////////////////////////////////////////////////////////////////
// Global namespace NEService inline function implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_STREAMABLE(NEService::ResultType)
AREG_IMPLEMENT_STREAMABLE(NEService::DataState)
AREG_IMPLEMENT_STREAMABLE(NEService::RequestType)
AREG_IMPLEMENT_STREAMABLE(NEService::MessageDataType)
AREG_IMPLEMENT_STREAMABLE(NEService::ServiceConnectionState)
AREG_IMPLEMENT_STREAMABLE(NEService::DisconnectReason)
AREG_IMPLEMENT_STREAMABLE(NEService::RegistrationAction)
AREG_IMPLEMENT_STREAMABLE(NEService::ServiceType)
AREG_IMPLEMENT_STREAMABLE(NEService::InstanceBitness)
AREG_IMPLEMENT_STREAMABLE(NEService::MessageSource)
AREG_IMPLEMENT_STREAMABLE(NEService::FuncIdRange)

//////////////////////////////////////////////////////////////////////////
// namespace NEService inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool NEService::is_service_connected(NEService::ServiceConnectionState connectionStatus)
{
    return (connectionStatus == NEService::ServiceConnectionState::Connected);
}

inline bool NEService::is_connection_pending( NEService::ServiceConnectionState connectionStatus )
{
    return (connectionStatus == NEService::ServiceConnectionState::Pending);
}

inline bool NEService::is_service_rejected( NEService::ServiceConnectionState connectionStatus )
{
    return (connectionStatus == NEService::ServiceConnectionState::Rejected);
}

inline bool NEService::is_connection_lost( NEService::ServiceConnectionState connectionStatus )
{
    return (connectionStatus == NEService::ServiceConnectionState::ConnectionLost);
}

inline bool NEService::is_service_disconnected( NEService::ServiceConnectionState connectionStatus )
{
    return (connectionStatus != NEService::ServiceConnectionState::Connected);
}

inline NEService::ServiceConnectionState NEService::service_connection( NEService::DisconnectReason reason )
{
    switch ( reason )
    {
    case NEService::DisconnectReason::UndefinedReason:
        return NEService::ServiceConnectionState::Unknown;
    
    case NEService::DisconnectReason::ServiceDisconnected:
    case NEService::DisconnectReason::ServiceLost:
    case NEService::DisconnectReason::ProviderLost:
    case NEService::DisconnectReason::ConsumerLost:
    case NEService::DisconnectReason::ClientConnectionLost:
        return NEService::ServiceConnectionState::ConnectionLost;

    case NEService::DisconnectReason::ServiceRejected:
    case NEService::DisconnectReason::ProviderRejected:
    case NEService::DisconnectReason::ConsumerNotSupported:
        return NEService::ServiceConnectionState::Rejected;

    case NEService::DisconnectReason::ConsumerDisconnected:
    case NEService::DisconnectReason::ProviderDisconnected:
    case NEService::DisconnectReason::ClientConnectionClosed:
        return NEService::ServiceConnectionState::Disconnected;

    case NEService::DisconnectReason::SystemShutdown:
        return NEService::ServiceConnectionState::Shutdown;

    default:
        return NEService::ServiceConnectionState::Connected;
    }
}

inline bool NEService::is_request_id(uint32_t msgId)
{
    return ((msgId & static_cast<uint32_t>(NEService::ServiceCallType::RequestFunction)) != 0);
}

inline bool NEService::is_response_id(uint32_t msgId)
{
    return ((msgId & static_cast<uint32_t>(NEService::ServiceCallType::ResponseFunction)) != 0);
}

inline bool NEService::is_attribute_id(uint32_t msgId)
{
    return ((msgId & static_cast<uint32_t>(NEService::ServiceCallType::AttributeUpdate)) != 0);
}

inline bool NEService::is_registry_id( uint32_t msgId )
{
    return ((msgId & static_cast<uint32_t>(NEService::ServiceCallType::ServiceRegisteration)) != 0);
}

inline bool NEService::is_empty_id(uint32_t msgId)
{
    return (msgId == static_cast<uint32_t>(NEService::FuncIdRange::EmptyFunctionId));
}

inline bool NEService::is_version_id( uint32_t msgId )
{
    return (msgId == static_cast<uint32_t>(NEService::FuncIdRange::ResponseServiceProviderVersion));
}

inline bool NEService::is_connect_id( uint32_t msgId )
{
    return (msgId == static_cast<uint32_t>(NEService::FuncIdRange::ResponseServiceProviderConnection));
}

inline bool NEService::is_executable_id(uint32_t msgId)
{
    return ( (msgId  & static_cast<uint32_t>(NEService::ServiceCallType::RequestFunction)     ) != 0 ||
             (msgId  & static_cast<uint32_t>(NEService::ServiceCallType::ResponseFunction)    ) != 0 ||
             (msgId  & static_cast<uint32_t>(NEService::ServiceCallType::AttributeUpdate)   ) != 0 ||
             (msgId == static_cast<uint32_t>(NEService::ServiceCallType::NoFunction)  ) );
}


inline NEService::MessageDataType NEService::message_data_type( uint32_t msgId )
{
    if ( NEService::is_request_id(msgId) )
        return NEService::MessageDataType::RequestData;
    else if (NEService::is_response_id(msgId))
        return NEService::MessageDataType::ResponseData;
    else if (NEService::is_attribute_id(msgId))
        return NEService::MessageDataType::AttributeData;
    else if (NEService::is_registry_id(msgId))
        return NEService::MessageDataType::ServiceData;
    else
        return NEService::MessageDataType::UndefinedData;
}

//////////////////////////////////////////////////////////////////////////
// class NEService::StateArray inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const NEService::DataState& NEService::StateArray::operator [] (uint32_t index) const
{
    return StateArrayBase::operator[](index);
}

inline NEService::DataState& NEService::StateArray::operator [] (uint32_t index)
{
    return StateArrayBase::operator[](index);
}

inline uint32_t NEService::StateArray::size() const
{
    return StateArrayBase::size();
}

inline void NEService::StateArray::reset()
{
    set_all_state(NEService::DataState::DataIsUnavailable);
}

inline bool NEService::StateArray::has_params() const
{
    return (is_empty() == false);
}

inline void NEService::StateArray::set_state(int32_t whichIndex, NEService::DataState newState)
{
    mValueList[whichIndex] = newState;
}

inline void NEService::StateArray::set_all_state(NEService::DataState newState)
{
    for ( uint32_t i = 0; i < this->mElemCount; ++ i )
        mValueList[i] = newState;
}

//////////////////////////////////////////////////////////////////////////
// class NEService::ParameterArray inline function implementation
//////////////////////////////////////////////////////////////////////////
inline NEService::StateArray& NEService::ParameterArray::operator [] ( uint32_t index )
{
    ASSERT(is_valid_index(index));
    return *(mParamList[index]);
}

inline const NEService::StateArray& NEService::ParameterArray::operator [] ( uint32_t index ) const
{
    ASSERT(is_valid_index(index));
    return *(mParamList[index]);
}

inline NEService::DataState NEService::ParameterArray::at( uint32_t row, uint32_t col ) const
{
    ASSERT(is_valid_index(row) && mParamList[row]->is_valid_index(static_cast<uint32_t>(col)));
    return mParamList[row]->at(static_cast<uint32_t>(col));
}

inline void NEService::ParameterArray::set_at( uint32_t row, uint32_t col, NEService::DataState newValue )
{
    ASSERT(is_valid_index(row) && mParamList[row]->is_valid_index(static_cast<uint32_t>(col)));
    mParamList[row]->set_at(static_cast<uint32_t>(col), newValue);
}

inline bool NEService::ParameterArray::has_parameters( uint32_t whichRespIndex ) const
{
    ASSERT(is_valid_index(whichRespIndex));
    return mParamList[whichRespIndex]->has_params();
}

inline void NEService::ParameterArray::reset()
{
    for (int col = 0; col < mElemCount; ++col)
    {
        mParamList[col]->reset();
    }
}

inline bool NEService::ParameterArray::is_valid_index(uint32_t whichRespIndex) const
{
    return ((static_cast<int32_t>(whichRespIndex) >= 0) && (static_cast<int32_t>(whichRespIndex) < mElemCount));
}

inline void NEService::ParameterArray::set_param_state(uint32_t whichParam, NEService::DataState newState)
{
    ASSERT(is_valid_index(whichParam));
    mParamList[whichParam]->set_all_state(newState);
}

//////////////////////////////////////////////////////////////////////////
// class NEService::ProxyData inline function implementation
//////////////////////////////////////////////////////////////////////////

inline NEService::DataState NEService::ProxyData::attribute_state( uint32_t msgId ) const
{
    return NEService::is_version_id(msgId) ? mImplVersion : mAttrState[NEService::attr_index(msgId)];
}

inline NEService::DataState NEService::ProxyData::param_state( uint32_t msgId ) const
{
    const NEService::StateArray& param = mParamState[NEService::resp_index(msgId)];
    return (param.has_params() ? param[0u] : NEService::DataState::DataIsUnavailable);
}

//////////////////////////////////////////////////////////////////////////
// class NEService::ConnectedInstance serialization
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Serializes the instance structure to the stream.
 * \param   stream  The streaming object to serialize.
 * \param   output  The single structure of instance to serialize.
 **/
inline OutStream& operator << (OutStream& stream, const NEService::ConnectedInstance & output)
{
    stream << output.ciSource << output.ciBitness << output.ciCookie << output.ciTimestamp << output.ciInstance << output.ciLocation;
    return stream;
}

/**
 * \brief   De-serializes the instance structure from the stream.
 * \param   stream  The streaming object that contains the information of the connected instance.
 * \param   input  The single structure of instance to initialize.
 **/
inline const InStream& operator >> (const InStream& stream, NEService::ConnectedInstance & input)
{
    stream >> input.ciSource >> input.ciBitness >> input.ciCookie >> input.ciTimestamp >> input.ciInstance >> input.ciLocation;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// class NEService enumerations string conversion
//////////////////////////////////////////////////////////////////////////

inline const char* NEService::as_string(NEService::ResultType resultType)
{
    switch (resultType)
    {
    case    NEService::ResultType::Undefined:
        return "NEService::ResultType::Undefined";

    case    NEService::ResultType::Error:
        return "NEService::ResultType::Error";
    case NEService::ResultType::Undelivered:
        return "NEService::ResultType::Undelivered";
    case    NEService::ResultType::NotProcessed:
        return "NEService::ResultType::NotProcessed";
    case NEService::ResultType::Processed:
        return "NEService::ResultType::Processed";

    case NEService::ResultType::MessageUndelivered:
        return "NEService::ResultType::MessageUndelivered";

    case    NEService::ResultType::RequestOK:
        return "NEService::ResultType::RequestOK";
    case    NEService::ResultType::RequestInvalid:
        return "NEService::ResultType::RequestInvalid";
    case    NEService::ResultType::RequestError:
        return "NEService::ResultType::RequestError";
    case    NEService::ResultType::RequestBusy:
        return "NEService::ResultType::RequestBusy";
    case    NEService::ResultType::RequestCanceled:
        return "NEService::ResultType::RequestCanceled";

    case    NEService::ResultType::DataOK:
        return "NEService::ResultType::DataOK";
    case    NEService::ResultType::DataInvalid:
        return "NEService::ResultType::DataInvalid";

    case    NEService::ResultType::ServiceOK:
        return "NEService::ResultType::ServiceOK";
    case    NEService::ResultType::ServiceUnavailable:
        return "NEService::ResultType::ServiceUnavailable";
    case    NEService::ResultType::Invalid:
        return "NEService::ResultType::Invalid";
    case    NEService::ResultType::ServiceRejected:
        return "NEService::ResultType::ServiceRejected";

    default:
        ASSERT(false);
        return "ERR: Undefined NEService::ResultType value!";
    }
}

inline const char* NEService::as_string(NEService::DataState dataState)
{
    switch (dataState)
    {
    case    NEService::DataState::DataIsUndefined:
        return "NEService::DataState::DataIsUndefined";

    case    NEService::DataState::DataIsOK:
        return "NEService::DataState::DataIsOK";

    case    NEService::DataState::DataIsInvalid:
        return "NEService::DataState::DataIsInvalid";

    case    NEService::DataState::DataIsUnavailable:
        return "NEService::DataState::DataIsUnavailable";

    case    NEService::DataState::DataUnexpectedError:
        return "NEService::DataState::DataUnexpectedError";

    default:
        ASSERT(false);
        return "ERR: Undefined NEService::DataState value!";
    }
}

inline const char* NEService::as_string( NEService::RequestType resultType )
{
    switch (resultType)
    {
    case    NEService::RequestType::Unprocessed:
        return "NEService::RequestType::Unprocessed";
    case    NEService::RequestType::StartNotify:
        return "NEService::RequestType::StartNotify";
    case    NEService::RequestType::StopNotify:
        return "NEService::RequestType::StopNotify";        
    case    NEService::RequestType::RemoveAllNotify:
        return "NEService::RequestType::RemoveAllNotify";
    case    NEService::RequestType::CallFunction:
        return "NEService::RequestType::CallFunction";
    case    NEService::RequestType::ServiceConnection:
        return "NEService::RequestType::ServiceConnection";
    case    NEService::RequestType::ClientConnection:
        return "NEService::RequestType::ClientConnection";
    case    NEService::RequestType::LoadComponent:
        return "NEService::RequestType::LoadComponent";
    default:
        ASSERT(false);
        return "ERR: Undefined NEService::RequestType value!";
    }
}

inline const char* NEService::as_string( NEService::MessageDataType dataType )
{
    switch (dataType)
    {
    case NEService::MessageDataType::UndefinedData:
        return "NEService::MessageDataType::UndefinedData";
    case NEService::MessageDataType::RequestData:
        return "NEService::MessageDataType::RequestData";
    case NEService::MessageDataType::ResponseData:
        return "NEService::MessageDataType::ResponseData";
    case NEService::MessageDataType::AttributeData:
        return "NEService::MessageDataType::AttributeData";
    case NEService::MessageDataType::ServiceData:
        return "NEService::MessageDataType::ServiceData";

    default:
        ASSERT(false);
        return "ERR: Undefined NEService::MessageDataType value!";
    }
}

inline const char* NEService::as_string( NEService::ServiceConnectionState service_connection )
{
    switch (service_connection)
    {
    case NEService::ServiceConnectionState::Unknown:
        return "NEService::ServiceConnectionState::Unknown";
    case NEService::ServiceConnectionState::Connected:
        return "NEService::ServiceConnectionState::Connected";
    case NEService::ServiceConnectionState::Pending:
        return "NEService::ServiceConnectionState::Pending";
    case NEService::ServiceConnectionState::Disconnected:
        return "NEService::ServiceConnectionState::Disconnected";
    case NEService::ServiceConnectionState::ConnectionLost:
        return "NEService::ServiceConnectionState::ConnectionLost";
    case NEService::ServiceConnectionState::Rejected:
        return "NEService::ServiceConnectionState::Rejected";
    case NEService::ServiceConnectionState::Failed:
        return "NEService::ServiceConnectionState::Failed";
    case NEService::ServiceConnectionState::Shutdown:
        return "NEService::ServiceConnectionState::Shutdown";
    default:
        ASSERT(false);
        return "ERR: Undefined NEService::ServiceConnectionState value!";
    }
}

inline const char * NEService::as_string( NEService::DisconnectReason reason )
{
    switch ( reason )
    {
    case NEService::DisconnectReason::UndefinedReason:
        return "NEService::DisconnectReason::UndefinedReason";
    case NEService::DisconnectReason::ServiceDisconnected:
        return "NEService::DisconnectReason::ServiceDisconnected";
    case NEService::DisconnectReason::ServiceLost:
        return "NEService::DisconnectReason::ServiceLost";
    case NEService::DisconnectReason::ServiceRejected:
        return "NEService::DisconnectReason::ServiceRejected";
    case NEService::DisconnectReason::ProviderDisconnected:
        return "NEService::DisconnectReason::ProviderDisconnected";
    case NEService::DisconnectReason::ProviderLost:
        return "NEService::DisconnectReason::ProviderLost";
    case NEService::DisconnectReason::ProviderRejected:
        return "NEService::DisconnectReason::ProviderRejected";
    case NEService::DisconnectReason::ConsumerDisconnected:
        return "NEService::DisconnectReason::ConsumerDisconnected";
    case NEService::DisconnectReason::ConsumerLost:
        return "NEService::DisconnectReason::ConsumerLost";
    case NEService::DisconnectReason::ConsumerNotSupported:
        return "NEService::DisconnectReason::ConsumerNotSupported";
    case NEService::DisconnectReason::SystemShutdown:
        return "NEService::DisconnectReason::SystemShutdown";
    case NEService::DisconnectReason::ClientConnectionLost:
        return "NEService::DisconnectReason::ClientConnectionLost";
    case NEService::DisconnectReason::ClientConnectionClosed:
        return "NEService::DisconnectReason::ClientConnectionClosed";
    default:
        ASSERT( false );
        return "ERR: Undefined NEService::DisconnectReason value!";
    }
}

inline const char * NEService::as_string( NEService::RegistrationAction svcRequestType )
{
    switch ( svcRequestType )
    {
    case NEService::RegistrationAction::RegisterClient:
        return "NEService::RegistrationAction::RegisterClient";
    case NEService::RegistrationAction::UnregisterClient:
        return "NEService::RegistrationAction::UnregisterClient";
    case NEService::RegistrationAction::RegisterStub:
        return "NEService::RegisterStub";
    case NEService::RegistrationAction::UnregisterStub:
        return "NEService::RegistrationAction::UnregisterStub";
    default:
        return "ERR: Unexpected NEService::RegistrationAction value!!!";
    }
}

inline const char * NEService::as_string( NEService::ServiceType srvcType )
{
    switch ( srvcType )
    {
    case NEService::ServiceType::Local:
        return "NEService::ServiceType::Local";
    case NEService::ServiceType::Public:
        return "NEService::ServiceType::Public";
    case NEService::ServiceType::Any:
        return "NEService::ServiceType::Any";
    case NEService::ServiceType::Invalid:
        return "NEService::ServiceType::Invalid";
    default:
        return "ERR: Unexpected NEService::RegistrationAction value!!!";
    }
}

const char* NEService::as_string(NEService::InstanceBitness bitness)
{
    switch (bitness)
    {
    case NEService::InstanceBitness::Bitness32:
        return "NEService::InstanceBitness::Bitness32";
    case NEService::InstanceBitness::Bitness64:
        return "NEService::InstanceBitness::Bitness64";
    case NEService::InstanceBitness::BitnessUnknown:
        return "NEService::InstanceBitness::BitnessUnknown";
    default:
        return "ERR: Unexpected NEService::InstanceBitness value!!!";
    }
}


const char * NEService::as_string(NEService::MessageSource msgSource)
{
    switch (msgSource)
    {
    case NEService::MessageSource::SourceUndefined:
        return "NEService::MessageSource::SourceUndefined";
    case NEService::MessageSource::SourceClient:
        return "NEService::MessageSource::SourceClient";
    case NEService::MessageSource::SourceService:
        return "NEService::MessageSource::SourceService";
    case NEService::MessageSource::SourceObserver:
        return "NEService::MessageSource::SourceObserver";
    case NEService::MessageSource::SourceTest:
        return "NEService::MessageSource::SourceTest";
    case NEService::MessageSource::SourceSimulation:
        return "NEService::MessageSource::SourceSimulation";
    default:
        return "ERR: Unexpected NEService::MessageSource value!!!";
    }
}


inline const char * NEService::as_string( NEService::FuncIdRange funcId )
{
    switch ( funcId )
    {
    case NEService::FuncIdRange::EmptyFunctionId:
        return "NEService::FuncIdRange::EmptyFunctionId";
    case NEService::FuncIdRange::ComponentCleanup:
        return "NEService::FuncIdRange::ComponentCleanup";
    case NEService::FuncIdRange::RequestRegisterService:
        return "NEService::FuncIdRange::RequestRegisterService";
    case NEService::FuncIdRange::RequestServiceProviderVersion:
        return "NEService::FuncIdRange::RequestServiceProviderVersion";
    case NEService::FuncIdRange::ResponseServiceProviderVersion:
        return "NEService::FuncIdRange::ResponseServiceProviderVersion";
    case NEService::FuncIdRange::RequestServiceProviderConnection:
        return "NEService::FuncIdRange::RequestServiceProviderConnection";
    case NEService::FuncIdRange::ResponseServiceProviderConnection:
        return "NEService::FuncIdRange::ResponseServiceProviderConnection";
    case NEService::FuncIdRange::SystemServiceConnect:
        return "NEService::FuncIdRange::SystemServiceConnect";
    case NEService::FuncIdRange::SystemServiceDisconnect:
        return "NEService::FuncIdRange::SystemServiceDisconnect";
    case NEService::FuncIdRange::SystemServiceNotifyConnection:
        return "NEService::FuncIdRange::SystemServiceNotifyConnection";
    case NEService::FuncIdRange::SystemServiceQueryInstances:
        return "NEService::FuncIdRange::SystemServiceQueryInstances";
    case NEService::FuncIdRange::SystemServiceNotifyInstances:
        return "NEService::FuncIdRange::SystemServiceNotifyInstances";
    case NEService::FuncIdRange::SystemServiceRequestRegister:
        return "NEService::FuncIdRange::SystemServiceRequestRegister";
    case NEService::FuncIdRange::SystemServiceNotifyRegister:
        return "NEService::FuncIdRange::SystemServiceNotifyRegister";
    case NEService::FuncIdRange::ServiceLogRegisterScopes:
        return "NEService::FuncIdRange::ServiceLogRegisterScopes";
    case NEService::FuncIdRange::ServiceLogUpdateScopes:
        return "NEService::FuncIdRange::ServiceLogUpdateScopes";
    case NEService::FuncIdRange::ServiceLogQueryScopes:
        return "NEService::FuncIdRange::ServiceLogQueryScopes";
    case NEService::FuncIdRange::ServiceLogScopesUpdated:
        return "NEService::FuncIdRange::ServiceLogScopesUpdated";
    case NEService::FuncIdRange::ServiceSaveLogConfiguration:
        return "NEService::FuncIdRange::ServiceSaveLogConfiguration";
    case NEService::FuncIdRange::ServiceLogConfigurationSaved:
        return "NEService::FuncIdRange::ServiceLogConfigurationSaved";
    case NEService::FuncIdRange::ServiceLogMessage:
        return "NEService::FuncIdRange::ServiceLogMessage";
    case NEService::FuncIdRange::RequestFirstId:
        return "NEService::FuncIdRange::RequestFirstId";
    case NEService::FuncIdRange::ResponseFirstId:
        return "NEService::FuncIdRange::ResponseFirstId";
    case NEService::FuncIdRange::AttributeFirstId:
        return "NEService::FuncIdRange::AttributeFirstId";
    case NEService::FuncIdRange::RequestLastId:
        return "NEService::FuncIdRange::RequestLastId";
    case NEService::FuncIdRange::ResponseLastId:
        return "NEService::FuncIdRange::ResponseLastId";
    case NEService::FuncIdRange::AttributeLastId:
        return "NEService::FuncIdRange::AttributeLastId";
    case NEService::FuncIdRange::ServiceLastId:
        return "NEService::FuncIdRange::ServiceLastId";

    default:
        {
            if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(NEService::FuncIdRange::RequestFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(NEService::FuncIdRange::RequestLastId)) )
                return "Request ID range";
            else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(NEService::FuncIdRange::ResponseFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(NEService::FuncIdRange::ResponseLastId)) )
                return "Response ID range";
            else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(NEService::FuncIdRange::AttributeFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(NEService::FuncIdRange::AttributeLastId)) )
                return "Attribute ID range";
            else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(NEService::FuncIdRange::RequestRegisterService)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(NEService::FuncIdRange::ServiceLastId)) )
                return "Service registration ID";
            else
                return "ERR: Unexpected ID";
        }
    }
}

#endif  // AREG_COMPONENT_SERVICEDEFS_HPP
