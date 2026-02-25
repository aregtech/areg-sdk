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
namespace areg
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
     * \brief   Returns string value of areg::ResultType type
     **/
    inline const char* getString(areg::ResultType resultType);

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
     * \brief   Returns string value of areg::DataState type
     **/
    inline const char* getString(areg::DataState dataState);

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
     * \brief   Returns string value of areg::RequestType type
     **/
    inline const char* getString( areg::RequestType resultType );

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
     * \brief   Returns string value of areg::MessageDataType type
     **/
    inline const char* getString( areg::MessageDataType dataType );

    /**
     * \brief   From passed message ID finds data type
     **/
    inline areg::MessageDataType getMessageDataType( uint32_t msgId );

    /**
     * \brief   areg::ServiceConnectionState
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
     * \brief   Returns string value of areg::ServiceConnectionState type
     **/
    inline const char* getString( areg::ServiceConnectionState serviceConnection );

    /**
     * \brief   Returns true if the service connection status is connected.
     **/
    inline bool isServiceConnected( areg::ServiceConnectionState connectionStatus );

    /**
     * \brief   Returns true if the service connection status is connected.
     **/
    inline bool isServiceConnectionPending( areg::ServiceConnectionState connectionStatus );

    /**
     * \brief   Returns true if the service connection status is rejected.
     **/
    inline bool isServiceRejected( areg::ServiceConnectionState connectionStatus );

    /**
     * \brief   Returns true if the service connection status is connection lost.
     **/
    inline bool isServiceConnectionLost( areg::ServiceConnectionState connectionStatus );

    /**
     * \brief   Returns true if the status is not connected.
     **/
    inline bool isServiceDisconnected( areg::ServiceConnectionState connectionStatus );

    /**
     * \brief   areg::DisconnectReason
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

    inline const char * getString( areg::DisconnectReason reason );

    inline areg::ServiceConnectionState serviceConnection( areg::DisconnectReason reason );

    /**
     * \brief   areg::RegistrationAction
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
     * \brief   Returns string value of areg::RegistrationAction type
     **/
    inline const char * getString( areg::RegistrationAction svcRequestType );

    /**
     * \brief   areg::ServiceType
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
     * \brief   Returns string value of areg::ServiceType type
     **/
    inline const char * getString( areg::ServiceType srvcType );

    /**
     * \brief   areg::MessageSource
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
     * \brief   Returns string value of areg::ServiceType type
     **/
    inline const char * getString(areg::MessageSource msgSource);

    /**
     * \brief   areg::InstanceBitness
     *          The bitness of the executable instance.
     **/
    enum class InstanceBitness  : uint16_t
    {
          BitnessUnknown    = 0     //!< Unknown bitness
        , Bitness32         = 32    //!< 32-bit system
        , Bitness64         = 64    //!< 64-bit system
    };

    /**
     * \brief   Returns string value of areg::ServiceType type
     **/
    inline const char* getString(areg::InstanceBitness bitness);

    /**
     * \brief   areg::SEQUENCE_NUMBER_NOTIFY
     *          Sequence number predefining notification message ID
     **/
    constexpr SequenceNumber    SEQUENCE_NUMBER_NOTIFY  { static_cast<SequenceNumber>(0) };    /*0x00000000*/
    /**
     * \brief   areg::SEQUENCE_NUMBER_ANY
     *          Any sequence number, used in messages. "Any sequence number" used to find any listener object with same message ID.
     **/
    constexpr SequenceNumber    SEQUENCE_NUMBER_ANY     { static_cast<SequenceNumber>(~0) };    /*0xFFFFFFFF*/

    /**
     * \brief   areg::COOKIE_UNKNOWN
     *          Unknown cookie
     **/
    constexpr ITEM_ID   COOKIE_UNKNOWN              { static_cast<ITEM_ID>(areg::Cookie::Invalid) };
    /**
     * \brief   areg::COOKIE_LOCAL
     *          The indication of local service.
     **/
    constexpr ITEM_ID   COOKIE_LOCAL                { static_cast<ITEM_ID>(areg::Cookie::Local) };
    /**
     * \brief   areg::COOKIE_ROUTER
     *          Indicates message router cookie
     **/
    constexpr ITEM_ID   COOKIE_ROUTER               { static_cast<ITEM_ID>(areg::Cookie::Router) };
    /**
     * \brief   areg::COOKIE_LOGGER
     *          Indicates log collector cookie
     **/
    constexpr ITEM_ID   COOKIE_LOGGER               { static_cast<ITEM_ID>(areg::Cookie::Logger) };
    /**
     * \brief   areg::COOKIE_ANY
     *          Indicates any valid cookie
     **/
    constexpr ITEM_ID   COOKIE_ANY                  { static_cast<ITEM_ID>(areg::Cookie::Any) };
    /**
     * \brief   areg::TARGET_UNKNOWN
     *          The unknown target ID
     **/
    constexpr ITEM_ID   TARGET_UNKNOWN              { static_cast<ITEM_ID>(areg::Cookie::Invalid) };
    /**
     * \brief   areg::TARGET_LOCAL
     *          The local target ID
     **/
    constexpr ITEM_ID   TARGET_LOCAL                { static_cast<ITEM_ID>(areg::Cookie::Local) };
    /**
     * \brief   areg::TARGET_ALL
     *          The undefined (all) target ID
     **/
    constexpr ITEM_ID   TARGET_ALL                  { static_cast<ITEM_ID>(areg::COOKIE_ANY) };
    /**
     * \brief   areg::SOURCE_UNKNOWN
     *          The unknown source ID
     **/
    constexpr ITEM_ID   SOURCE_UNKNOWN              { static_cast<ITEM_ID>(areg::Cookie::Invalid) };
    /**
     * \brief   areg::SOURCE_UNKNOWN
     *          The unknown source ID.
     **/
    constexpr ITEM_ID   SOURCE_LOCAL                { static_cast<ITEM_ID>(areg::Cookie::Local) };
    /**
     * \brief   areg::COOKIE_REMOTE_SERVICE
     *          The ID of first valid remote cookie.
     **/
    constexpr ITEM_ID   COOKIE_REMOTE_SERVICE       { static_cast<ITEM_ID>(areg::Cookie::FirstRemote) };

    /**
     * \brief   areg::ServiceCallType
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
     * \brief   areg::REQUEST_ID_FIRST
     *          The first ID in request call.
     **/
    constexpr uint32_t  REQUEST_ID_FIRST    { static_cast<uint32_t>(ServiceCallType::RequestFunction) };
    /**
     * \brief   areg::REQUEST_ID_LAST
     *          The last ID in request call.
     **/
    constexpr uint32_t  REQUEST_ID_LAST     { REQUEST_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   areg::RESPONSE_ID_FIRST
     *          The first ID in response call.
     **/
    constexpr uint32_t  RESPONSE_ID_FIRST   { static_cast<uint32_t>(ServiceCallType::ResponseFunction) };
    /**
     * \brief   areg::RESPONSE_ID_LAST
     *          The last ID in response call.
     **/
    constexpr uint32_t  RESPONSE_ID_LAST    { RESPONSE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   areg::ATTRIBUTE_ID_FIRST
     *          The first ID in attribute call.
     **/
    constexpr uint32_t  ATTRIBUTE_ID_FIRST  { static_cast<uint32_t>(ServiceCallType::AttributeUpdate) };
    /**
     * \brief   areg::ATTRIBUTE_ID_LAST
     *          The last ID in attribute call.
     **/
    constexpr uint32_t  ATTRIBUTE_ID_LAST   { ATTRIBUTE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   areg::SERVICE_ID_FIRST
     *          The last ID in service call.
     **/
    constexpr uint32_t  SERVICE_ID_FIRST    { static_cast<uint32_t>(areg::ServiceCallType::ServiceRegisteration) };
    /**
     * \brief   areg::SERVICE_ID_LAST
     *          The last ID in service call.
     **/
    constexpr uint32_t  SERVICE_ID_LAST     { SERVICE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   areg::RESPONSE_ID_NONE
     *          Constant no response. Used to indicate that the request has no response.
     **/
    constexpr uint32_t  RESPONSE_ID_NONE    { static_cast<uint32_t>(ServiceCallType::NoFunction) };

    /**
     * \brief   areg::INVALID_MESSAGE_ID
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
     * \brief   Returns string value of areg::FuncIdRange type
     **/
    inline const char * getString( areg::FuncIdRange funcId );

    /**
     * \brief   Converts request message ID to array index. Returns INVALID_INDEX if empty.
     **/
    inline constexpr uint32_t reqIndex(uint32_t msgId)
    {
        return (msgId != static_cast<uint32_t>(FuncIdRange::EmptyFunctionId))
                ? msgId - static_cast<uint32_t>(FuncIdRange::RequestFirstId)
                : static_cast<uint32_t>(areg::INVALID_INDEX);
    }

    /**
     * \brief   Converts response message ID to array index. Returns INVALID_INDEX if empty.
     **/
    inline constexpr uint32_t respIndex(uint32_t msgId)
    {
        return (msgId != static_cast<uint32_t>(FuncIdRange::EmptyFunctionId))
                ? msgId - static_cast<uint32_t>(FuncIdRange::ResponseFirstId)
                : static_cast<uint32_t>(areg::INVALID_INDEX);
    }

    /**
     * \brief   Converts attribute message ID to array index. Returns INVALID_INDEX if empty.
     **/
    inline constexpr uint32_t attrIndex(uint32_t msgId)
    {
        return (msgId != static_cast<uint32_t>(FuncIdRange::EmptyFunctionId))
                ? msgId - static_cast<uint32_t>(FuncIdRange::AttributeFirstId)
                : static_cast<uint32_t>(areg::INVALID_INDEX);
    }

    /**
     * \brief   Returns true if message ID is in range of attribute call.
     **/
    inline bool isAttributeId(uint32_t msgId);

    /**
     * \brief   Returns true if message ID is in range of response call
     **/
    inline bool isResponseId(uint32_t msgId);

    /**
     * \brief   Returns true if message ID is in range of request call
     **/
    inline bool isRequestId(uint32_t msgId);

    /**
     * \brief   Returns true if message ID is a registration call
     **/
    inline bool isServiceRegistryId( uint32_t msgId );

    /**
     * \brief   Returns true if message ID is version notification
     **/
    inline bool isVersionNotifyId(uint32_t msgId);

    /**
     * \brief   Returns true if message ID is service connect
     **/
    inline bool isConnectNotifyId(uint32_t msgId);

    /**
     * \brief   Returns true if message ID is either request, or response, or attribute notification.
     **/
    inline bool isExecutableId( uint32_t msgId );

    /**
     * \brief   Returns true if message ID is empty function
     **/
    inline bool isEmptyFunctionId(uint32_t msgId);

    class ParameterArray;

    //////////////////////////////////////////////////////////////////////////
    // StateArray class declaration
    //////////////////////////////////////////////////////////////////////////
    using StateArrayBase    = FixedArray<areg::DataState>;
    /**
     * \brief   StateArray class. Keeps data state information
     **/
    class AREG_API StateArray : private StateArrayBase
    {
        friend class ParameterArray;
    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets initial size of fixed array. The size of array cannot be changed dynamically.
         **/
        StateArray(uint32_t count);

        /**
         * \brief   Initialization constructor.
         *          Does not make hard-copy, only assigns given
         *          parameters
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
         * \brief   Returns element by the index for reading. The index must be valid.
         **/
        inline const areg::DataState& operator [] (uint32_t index) const;
        /**
         * \brief   Returns element by the index for reading. The index must be valid.
         **/
        inline areg::DataState& operator [] (uint32_t index);

        /**
         * \brief   Returns the number of elements in the array.
         */
        inline uint32_t getSize() const;
        /**
         * \brief   Resets states in array. All states will be set to areg::DataIsUnavailable
         **/
        inline void resetStates();

        /**
         * \brief   Returns true if array has parameters, i.e. the size is
         *          not zero.
         **/
        inline bool hasParams() const;

        /**
         * \brief   Sets the state of certain entry in array.
         * \param   whichIndex  The index of entry in array which state should be changed
         * \param   newState    The state to set
         **/
        inline void setState(int32_t whichIndex, areg::DataState newState);

        /**
         * \brief   All entries are set to the same given state
         * \param   newState    The state to set for all entries
         **/
        inline void setAllState(areg::DataState newState);

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
        /**
         * \brief   The name of service interface.
         **/
        const char *        idServiceName{ nullptr };

        /**
         * \brief   Service interface implementation version
         **/
        areg::Version       idVersion{ 0, 0, 0 };

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
    extern AREG_API const areg::Version EmptyServiceVersion /*(1, 0, 0)*/;
    /**
     * \brief   Returns the instance of empty local service interface.
     **/
    AREG_API areg::InterfaceData & getEmptyInterface();

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
     * \brief   Map of parameters. Keeps information about parameter
     *          states for every response. The parameter state is accessed
     *          by specifying response ID and index of parameters in
     *          response function.
     *
     * @\example    Parameter Array
     *
     *          For example, if there is a response function prototype
     *          responseAREG(const bool &param1, const int32_t &param2)
     *          the 'param1' state can be accessed by index pair
     *          param1State = states[areg::respIndex(UPD_ID_responseAREG)][0];
     **/
    class AREG_API ParameterArray
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Initialization constructor. Initialize Parameter
         *          State Array from given service interface data structure,
         *          which contains map table of parameter counts in every response.
         * \param   ifData  The service interface data structure containing
         *                  parameter map table
         **/
        ParameterArray(const areg::InterfaceData& ifData);


        /**
         * \brief   Initialization constructor. Initialize Parameter
         *          State Array from given parameter count map,
         *          which contains map table of parameter counts in every response.
         * \param   paramCountMap   The map table of number of parameters in every response
         * \param   count           Number of entries in map table
         **/
        ParameterArray(const uint32_t* paramCountMap, int32_t count);

        /**
         * \brief   Moves data from given source.
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
         * \brief   Moves data from given source.
         * \param   src     The source of data to move.
         **/
        ParameterArray & operator = ( ParameterArray && src ) noexcept;

        /**
         * \brief   Access state by given index of array allowed for writing.
         *          The index is calculated by formula
         *          ('response ID' - areg::FuncIdRange::ResponseFirstId) or use
         *          areg::respIndex() macro
         **/
        inline areg::StateArray & operator [] (uint32_t index);
        /**
         * \brief   Access read-only state by given index of array.
         *          The index is calculated by formula
         *          ('response ID' - areg::FuncIdRange::ResponseFirstId) or use
         *          areg::respIndex() macro
         **/
        inline const areg::StateArray & operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Returns the state of parameter of certain response entry
         *          and certain parameter. The parameter entry is calculated
         *          row = areg::respIndex('response ID').
         * \param   row     The index of response call. Should be areg::respIndex('response ID')
         * \param   col     The index of parameter entry in response call. 
         *                  First parameter has index zero
         * \return  The state of parameter in call.
         **/
        inline areg::DataState getAt(uint32_t row, uint32_t col) const;

        /**
         * \brief   Sets state of parameter of certain response.
         * \param   row         The index of response entry, which can be calculated
         *                      by macro areg::respIndex('response ID')
         * \param   col         The index of parameter in response call. 
         *                      First parameter has index zero
         * \param   newValue    The state to set for parameter
         **/
        inline void setAt(uint32_t row, uint32_t col, areg::DataState newValue);

        /**
         * \brief   Returns true if specified response index has parameters.
         *          The response index is calculated by macro areg::respIndex('response ID')
         **/
        inline bool hasParameters(uint32_t whichRespIndex) const;
        /**
         * \brief   Returns true if valid response index. For example,
         *          the response ID RESPONSE_ID_NONE does not have valid
         *          response index. The index of response is calculated
         *          by macro areg::respIndex('response ID')
         * \param   whichRespIndex  The index of response, should be calculated by
         *                          macro areg::respIndex('response ID')
         * \return  Returns true if given index is valid
         **/
        inline bool isValidParamIndex(uint32_t whichRespIndex) const;

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Resets all states of parameters
         **/
        inline void resetAllStates();

        /**
         * \brief   Sets all parameter states of specified entry.
         *          The index should be calculated by areg::respIndex('response ID')
         * \param   whichParam  The index of entry, should be calculated by areg::respIndex('response ID')
         * \param   newState    The state to set
         **/
        inline void setParamState(uint32_t whichParam, areg::DataState newState);

        /**
         * \brief   Resets all states of parameter of specified entry index.
         *          The index should be calculated by areg::respIndex('response ID')
         * \param   whichParam  The index of entry, should be calculated by areg::respIndex('response ID')
         **/
        void resetParamState(uint32_t whichParam);

    //////////////////////////////////////////////////////////////////////////
    // Hidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Constructs the parameter array object.
         * \param   params  Parameter count table
         * \param   count   Number of entries in table
         **/
        void construct(const uint32_t* params, int32_t count);

        /**
         * \brief   Counts size in bytes required for states
         **/
        uint32_t countParamSpace(const uint32_t* params, int32_t count);

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
        areg::StateArray **    mParamList;

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
     * \brief   ProxyData class. 
     *          Contains info of Data required by Proxy object
     **/
    class AREG_API ProxyData
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Initialize data from given service interface data object.
         * \param   ifData  Service Interface data object.
         **/
        ProxyData(const areg::InterfaceData & ifData);
        /**
         * \brief   Destructor
         **/
        ~ProxyData() = default;

    public:

        /**
         * \brief   Resets states of Proxy Data
         **/
        void resetStates();

        /**
         * \brief	Set state for specified request ID (range in areg::FuncIdRange)
         *          First, it will figure our whether it is attribute or response request ID.
         *          Then it will convert to index value and set appropriate entry state.
         *          If the request ID is areg::ATTRIBUTE_SI_VERSION, it will only set
         *          state of component implementation version.
         * \param	msgId	    The function ID in range areg::FuncIdRange.
         *                      Can be either attribute or response range ID.
         *                      Otherwise it is ignored.
         * \param	newState	New state to set. If requested ID is response range,
         *                      it will set state for all parameters of that response, 
         *                      if any exist.
         **/
        void setDataState(uint32_t msgId, areg::DataState newState);

        /**
         * \brief   Returns data state of given message (attribute or response) ID.
         **/
        areg::DataState getDataState(uint32_t msgId) const;

        /**
         * \brief   Returns the response message ID of given request message ID.
         *          If message is wrong and/or is not in request to response map,
         *          it will return areg::INVALID_MESSAGE_ID
         **/
        uint32_t getResponseId(uint32_t requestId) const;

        /**
         * \brief   Returns data state of given attribute ID
         **/
        inline areg::DataState getAttributeState(uint32_t msgId) const;

        /**
         * \brief   Returns parameter state of given response ID
         **/
        inline areg::DataState getParamState(uint32_t msgId) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Implementation version
         **/
        areg::DataState           mImplVersion;   // implementation version

        /**
         * \brief   Service Interface data
         **/
        const areg::InterfaceData &   mIfData;        // fixed service interface data

        /**
         * \brief   Table of attribute states
         **/
        areg::AttrState                mAttrState;     // state of attributes

        /**
         * \brief   Table of response parameter state
         **/
        areg::ParamState               mParamState;    // state of parameters in response call.

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        ProxyData() = delete;
        AREG_NOCOPY_NOMOVE( ProxyData );
    };


    /**
     * \brief   areg::ConnectedInstance
     *          Service connected instance of application.
     **/
    struct ConnectedInstance
    {
        //!< The type of the application
        areg::MessageSource   ciSource    { areg::MessageSource::SourceUndefined };
        //!< The bit-set of connected instance
        areg::InstanceBitness ciBitness   { areg::InstanceBitness::BitnessUnknown };
        //!< The cookie of the connected instance.
        ITEM_ID                     ciCookie    { areg::COOKIE_UNKNOWN };
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
    using MapInstances = OrderedMap<ITEM_ID, areg::ConnectedInstance>;
}

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
// namespace areg inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool areg::isServiceConnected(areg::ServiceConnectionState connectionStatus)
{
    return (connectionStatus == areg::ServiceConnectionState::Connected);
}

inline bool areg::isServiceConnectionPending( areg::ServiceConnectionState connectionStatus )
{
    return (connectionStatus == areg::ServiceConnectionState::Pending);
}

inline bool areg::isServiceRejected( areg::ServiceConnectionState connectionStatus )
{
    return (connectionStatus == areg::ServiceConnectionState::Rejected);
}

inline bool areg::isServiceConnectionLost( areg::ServiceConnectionState connectionStatus )
{
    return (connectionStatus == areg::ServiceConnectionState::ConnectionLost);
}

inline bool areg::isServiceDisconnected( areg::ServiceConnectionState connectionStatus )
{
    return (connectionStatus != areg::ServiceConnectionState::Connected);
}

inline areg::ServiceConnectionState areg::serviceConnection( areg::DisconnectReason reason )
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

inline bool areg::isRequestId(uint32_t msgId)
{
    return ((msgId & static_cast<uint32_t>(areg::ServiceCallType::RequestFunction)) != 0);
}

inline bool areg::isResponseId(uint32_t msgId)
{
    return ((msgId & static_cast<uint32_t>(areg::ServiceCallType::ResponseFunction)) != 0);
}

inline bool areg::isAttributeId(uint32_t msgId)
{
    return ((msgId & static_cast<uint32_t>(areg::ServiceCallType::AttributeUpdate)) != 0);
}

inline bool areg::isServiceRegistryId( uint32_t msgId )
{
    return ((msgId & static_cast<uint32_t>(areg::ServiceCallType::ServiceRegisteration)) != 0);
}

inline bool areg::isEmptyFunctionId(uint32_t msgId)
{
    return (msgId == static_cast<uint32_t>(areg::FuncIdRange::EmptyFunctionId));
}

inline bool areg::isVersionNotifyId( uint32_t msgId )
{
    return (msgId == static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderVersion));
}

inline bool areg::isConnectNotifyId( uint32_t msgId )
{
    return (msgId == static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection));
}

inline bool areg::isExecutableId(uint32_t msgId)
{
    return ( (msgId  & static_cast<uint32_t>(areg::ServiceCallType::RequestFunction)     ) != 0 ||
             (msgId  & static_cast<uint32_t>(areg::ServiceCallType::ResponseFunction)    ) != 0 ||
             (msgId  & static_cast<uint32_t>(areg::ServiceCallType::AttributeUpdate)   ) != 0 ||
             (msgId == static_cast<uint32_t>(areg::ServiceCallType::NoFunction)  ) );
}


inline areg::MessageDataType areg::getMessageDataType( uint32_t msgId )
{
    if ( areg::isRequestId(msgId) )
        return areg::MessageDataType::RequestData;
    else if (areg::isResponseId(msgId))
        return areg::MessageDataType::ResponseData;
    else if (areg::isAttributeId(msgId))
        return areg::MessageDataType::AttributeData;
    else if (areg::isServiceRegistryId(msgId))
        return areg::MessageDataType::ServiceData;
    else
        return areg::MessageDataType::UndefinedData;
}

//////////////////////////////////////////////////////////////////////////
// class areg::StateArray inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const areg::DataState& areg::StateArray::operator [] (uint32_t index) const
{
    return StateArrayBase::operator[](index);
}

inline areg::DataState& areg::StateArray::operator [] (uint32_t index)
{
    return StateArrayBase::operator[](index);
}

inline uint32_t areg::StateArray::getSize() const
{
    return StateArrayBase::getSize();
}

inline void areg::StateArray::resetStates()
{
    setAllState(areg::DataState::DataIsUnavailable);
}

inline bool areg::StateArray::hasParams() const
{
    return (isEmpty() == false);
}

inline void areg::StateArray::setState(int32_t whichIndex, areg::DataState newState)
{
    mValueList[whichIndex] = newState;
}

inline void areg::StateArray::setAllState(areg::DataState newState)
{
    for ( uint32_t i = 0; i < this->mElemCount; ++ i )
        mValueList[i] = newState;
}

//////////////////////////////////////////////////////////////////////////
// class areg::ParameterArray inline function implementation
//////////////////////////////////////////////////////////////////////////
inline areg::StateArray& areg::ParameterArray::operator [] ( uint32_t index )
{
    ASSERT(isValidParamIndex(index));
    return *(mParamList[index]);
}

inline const areg::StateArray& areg::ParameterArray::operator [] ( uint32_t index ) const
{
    ASSERT(isValidParamIndex(index));
    return *(mParamList[index]);
}

inline areg::DataState areg::ParameterArray::getAt( uint32_t row, uint32_t col ) const
{
    ASSERT(isValidParamIndex(row) && mParamList[row]->isValidIndex(static_cast<uint32_t>(col)));
    return mParamList[row]->getAt(static_cast<uint32_t>(col));
}

inline void areg::ParameterArray::setAt( uint32_t row, uint32_t col, areg::DataState newValue )
{
    ASSERT(isValidParamIndex(row) && mParamList[row]->isValidIndex(static_cast<uint32_t>(col)));
    mParamList[row]->setAt(static_cast<uint32_t>(col), newValue);
}

inline bool areg::ParameterArray::hasParameters( uint32_t whichRespIndex ) const
{
    ASSERT(isValidParamIndex(whichRespIndex));
    return mParamList[whichRespIndex]->hasParams();
}

inline void areg::ParameterArray::resetAllStates()
{
    for (int col = 0; col < mElemCount; ++col)
    {
        mParamList[col]->resetStates();
    }
}

inline bool areg::ParameterArray::isValidParamIndex(uint32_t whichRespIndex) const
{
    return ((static_cast<int32_t>(whichRespIndex) >= 0) && (static_cast<int32_t>(whichRespIndex) < mElemCount));
}

inline void areg::ParameterArray::setParamState(uint32_t whichParam, areg::DataState newState)
{
    ASSERT(isValidParamIndex(whichParam));
    mParamList[whichParam]->setAllState(newState);
}

//////////////////////////////////////////////////////////////////////////
// class areg::ProxyData inline function implementation
//////////////////////////////////////////////////////////////////////////

inline areg::DataState areg::ProxyData::getAttributeState( uint32_t msgId ) const
{
    return areg::isVersionNotifyId(msgId) ? mImplVersion : mAttrState[areg::attrIndex(msgId)];
}

inline areg::DataState areg::ProxyData::getParamState( uint32_t msgId ) const
{
    const areg::StateArray& param = mParamState[areg::respIndex(msgId)];
    return (param.hasParams() ? param[0u] : areg::DataState::DataIsUnavailable);
}

//////////////////////////////////////////////////////////////////////////
// class areg::ConnectedInstance serialization
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Serializes the instance structure to the stream.
 * \param   stream  The streaming object to serialize.
 * \param   output  The single structure of instance to serialize.
 **/
inline OutStream& operator << (OutStream& stream, const areg::ConnectedInstance & output)
{
    stream << output.ciSource << output.ciBitness << output.ciCookie << output.ciTimestamp << output.ciInstance << output.ciLocation;
    return stream;
}

/**
 * \brief   De-serializes the instance structure from the stream.
 * \param   stream  The streaming object that contains the information of the connected instance.
 * \param   input  The single structure of instance to initialize.
 **/
inline const InStream& operator >> (const InStream& stream, areg::ConnectedInstance & input)
{
    stream >> input.ciSource >> input.ciBitness >> input.ciCookie >> input.ciTimestamp >> input.ciInstance >> input.ciLocation;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// class NEService enumerations string conversion
//////////////////////////////////////////////////////////////////////////

inline const char* areg::getString(areg::ResultType resultType)
{
    switch (resultType)
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

inline const char* areg::getString(areg::DataState dataState)
{
    switch (dataState)
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

inline const char* areg::getString( areg::RequestType resultType )
{
    switch (resultType)
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

inline const char* areg::getString( areg::MessageDataType dataType )
{
    switch (dataType)
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

inline const char* areg::getString( areg::ServiceConnectionState serviceConnection )
{
    switch (serviceConnection)
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

inline const char * areg::getString( areg::DisconnectReason reason )
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

inline const char * areg::getString( areg::RegistrationAction svcRequestType )
{
    switch ( svcRequestType )
    {
    case areg::RegistrationAction::RegisterClient:
        return "areg::RegistrationAction::RegisterClient";
    case areg::RegistrationAction::UnregisterClient:
        return "areg::RegistrationAction::UnregisterClient";
    case areg::RegistrationAction::RegisterStub:
        return "areg::RegisterStub";
    case areg::RegistrationAction::UnregisterStub:
        return "areg::RegistrationAction::UnregisterStub";
    default:
        return "ERR: Unexpected areg::RegistrationAction value!!!";
    }
}

inline const char * areg::getString( areg::ServiceType srvcType )
{
    switch ( srvcType )
    {
    case areg::ServiceType::Local:
        return "areg::ServiceType::Local";
    case areg::ServiceType::Public:
        return "areg::ServiceType::Public";
    case areg::ServiceType::Any:
        return "areg::ServiceType::Any";
    case areg::ServiceType::Invalid:
        return "areg::ServiceType::Invalid";
    default:
        return "ERR: Unexpected areg::RegistrationAction value!!!";
    }
}

const char* areg::getString(areg::InstanceBitness bitness)
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


const char * areg::getString(areg::MessageSource msgSource)
{
    switch (msgSource)
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


inline const char * areg::getString( areg::FuncIdRange funcId )
{
    switch ( funcId )
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
            if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(areg::FuncIdRange::RequestFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(areg::FuncIdRange::RequestLastId)) )
                return "Request ID range";
            else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(areg::FuncIdRange::ResponseFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(areg::FuncIdRange::ResponseLastId)) )
                return "Response ID range";
            else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(areg::FuncIdRange::AttributeFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(areg::FuncIdRange::AttributeLastId)) )
                return "Attribute ID range";
            else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(areg::FuncIdRange::RequestRegisterService)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(areg::FuncIdRange::ServiceLastId)) )
                return "Service registration ID";
            else
                return "ERR: Unexpected ID";
        }
    }
}

#endif  // AREG_COMPONENT_SERVICEDEFS_HPP
