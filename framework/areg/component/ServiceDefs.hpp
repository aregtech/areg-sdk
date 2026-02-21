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
AREG_NAMESPACE_BEGIN

    /**
     * \brief   Result types.
     *          Used when sending response service event
     *          and needs to define validation of execution.
     **/
    typedef enum class ResultType : uint16_t
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
        , ServiceInvalid     = 32897 /*0x8081*/  //!< service invalid (check cookie).        Bits: 1000 0000 1000 0001
        , ServiceRejected    = 32899 /*0x8083*/  //!< service rejected (unsupported).        Bits: 1000 0000 1000 0011

    } ResultType;

    /**
     * \brief   Returns string value of areg::ResultType type
     **/
    inline const char* getString(areg::ResultType resultType);

    /**
     * \brief   Data types
     *          Used getting data
     **/
    typedef enum class E_DataStateType  : uint16_t
    {
          DataIsUndefined       =     0 /*0x0000*/  //!< undefined type, not used
        , DataIsOK              = 16384 /*0x4000*/  //!< data valid
        , DataIsInvalid         = 16385 /*0x4001*/  //!< data is invalid
        , DataIsUnavailable     = 16387 /*0x4003*/  //!< data is unavailable, no such data
        , DataUnexpectedError   = 16389 /*0x4005*/  //!< other errors

    } eDataStateType;
    /**
     * \brief   Returns string value of areg::eDataStateType type
     **/
    inline const char* getString(areg::eDataStateType dataState);

    /**
     * \brief   Type of request.
     *          Used sending request event by proxy
     **/
    typedef enum class E_RequestType : uint16_t
    {
          Unprocessed       =     0 /*0x0000*/  //!< request is unprocessed
        , StartNotify       =  8193 /*0x2001*/  //!< request start notify on attribute update
        , StopNotify        =  8194 /*0x2002*/  //!< request stop notify on attribute update
        , RemoveAllNotify   =  8198 /*0x2006*/  //!< request remove all notifications
        , CallFunction      = 16385 /*0x4001*/  //!< request function call
        , ServiceConnection = 16387 /*0x4003*/  //!< request connection status update
        , ClientConnection  = 16389 /*0x4005*/  //!< request client connection status update
        , LoadComponent     = 16391 /*0x4007*/  //!< request load component
    } eRequestType;
    /**
     * \brief   Returns string value of areg::eRequestType type
     **/
    inline const char* getString( areg::eRequestType resultType );

    /**
     * \brief   Message Data types
     *          Used to identify type of event data, which
     *          used either in service request or in service response
     *          event objects
     **/
    typedef enum class E_MessageDataType : uint16_t
    {
          UndefinedDataType = 0x0000u   //!< data type is undefined
        , RequestDataType   = 0x1000u   //!< data type of request event
        , ResponseDataType  = 0x2000u   //!< data type response event
        , AttributeDataType = 0x4000u   //!< data type of attribute event
        , ServiceDataType   = 0x8000u   //!< data type service call

    } eMessageDataType;
    /**
     * \brief   Returns string value of areg::eMessageDataType type
     **/
    inline const char* getString( areg::eMessageDataType dataType );

    /**
     * \brief   From passed message ID finds data type
     **/
    inline areg::eMessageDataType getMessageDataType( unsigned int msgId );

    /**
     * \brief   areg::eServiceConnection
     *          Service Connections. Used in service calls
     **/
    typedef enum class E_ServiceConnection : uint16_t
    {
          ServiceConnectionUnknown  =    0  /*0x0000*/  //!< Connection is unknown.             Bit set: 0000 0000 0000 0000
        , ServiceConnected          =    1  /*0x0001*/  //!< Service connected, ready to serve. Bit set: 0000 0000 0000 0001
        , ServiceDisconnected       =  256  /*0x0100*/  //!< Service disconnected.              Bit set: 0000 0001 0000 0000
        , ServicePending            =  257  /*0x0101*/  //!< Service connection is pending.     Bit set: 0000 0001 0000 0001
        , ServiceConnectionLost     =  768  /*0x0300*/  //!< Service lost connection.           Bit set: 0000 0011 0000 0000
        , ServiceRejected           = 1280  /*0x0500*/  //!< Service connection rejected.       Bit set: 0000 0101 0000 0000
        , ServiceFailed             = 2304  /*0x0900*/  //!< Service connection failed.         Bit set: 0000 1001 0000 0000
        , ServiceShutdown           = 4352  /*0x1100*/  //!< Service shut down, no connection.  Bit set: 0001 0001 0000 0000
    } eServiceConnection;

    /**
     * \brief   Returns string value of areg::eDataType type
     **/
    inline const char* getString( areg::eServiceConnection serviceConnection );

    /**
     * \brief   Returns true if the service connection status is connected.
     **/
    inline bool isServiceConnected( areg::eServiceConnection connectionStatus );

    /**
     * \brief   Returns true if the service connection status is connected.
     **/
    inline bool isServiceConnectionPending( areg::eServiceConnection connectionStatus );

    /**
     * \brief   Returns true if the service connection status is rejected.
     **/
    inline bool isServiceRejected( areg::eServiceConnection connectionStatus );

    /**
     * \brief   Returns true if the service connection status is connection lost.
     **/
    inline bool isServiceConnectionLost( areg::eServiceConnection connectionStatus );

    /**
     * \brief   Returns true if the status is not connected.
     **/
    inline bool isServiceDisconnected( areg::eServiceConnection connectionStatus );

    /**
     * \brief   areg::eDisconnectReason
     *          The service provider and service consumer disconnect reason.
     *          Valid only when service is not connected. Otherwise, it should be ignored.
     **/
    typedef enum class E_DisconnectReason : uint16_t
    {
          ReasonUndefined               = 0     //!< Undefined disconnect reason.
        , ReasonServiceDisconnected     = 1     //!< The service is disconnected.
        , ReasonServiceLost             = 2     //!< The service connection is lost.
        , ReasonServiceRejected         = 4     //!< The service rejected connection.
        , ReasonProviderDisconnected    = 8     //!< The service provider disconnected.
        , ReasonProviderLost            = 16    //!< The connection with service provider is lost.
        , ReasonProviderRejected        = 32    //!< The service provider rejected the service consumer.
        , ReasonConsumerDisconnected    = 64    //!< The service consumer is disconnected.
        , ReasonConsumerLost            = 128   //!< The connection with service consumer is lost.
        , ReasonConsumerNotSupported    = 256   //!< The service consumer is rejected because is not supported (wrong version, for example).
        , ReasonSystemShutdown          = 512   //!< The system is shutting down.
        , ReasonClientConnectionLost    = 1024  //!< The system lost connection with the client. General reason.
        , ReasonClientConnectionClosed  = 2048  //!< The client requested to disconnect. General reason.
    } eDisconnectReason;

    inline const char * getString( areg::eDisconnectReason reason );

    inline areg::eServiceConnection serviceConnection( areg::eDisconnectReason reason );

    /**
     * \brief   areg::eServiceRequestType
     *          The service request types.
     **/
    typedef enum class E_ServiceRequestType : uint16_t
    {
          RegisterClient    = 0x0010    //!< Client requests to register.           Bit set: 0001 0000
        , UnregisterClient  = 0x0011    //!< Client requests to unregister.         Bit set: 0001 0001
        , RegisterStub      = 0x0020    //!< Server requests to register.           Bit set: 0010 0000
        , UnregisterStub    = 0x0021    //!< Server requests to unregister.         Bit set: 0010 0001
    } eServiceRequestType;
    
    /**
     * \brief   Returns string value of areg::eServiceRequestType type
     **/
    inline const char * getString( areg::eServiceRequestType svcRequestType );

    /**
     * \brief   areg::eServiceType
     *          Service Type. Either local or Remote.
     **/
    typedef enum class E_ServiceType : uint16_t
    {
          ServiceInvalid    = 0x0000    //!< Invalid Service      Bit set: 0000 0000 0000 0000
        , ServiceLocal      = 0x0040    //!< Local Service.       Bit set: 0000 0000 0100 0000
        , ServicePublic     = 0x0080    //!< External Service.    Bit set: 0000 0000 1000 0000
        , ServiceAny        = 0x00C0    //!< Any service.         Bit set: 0000 0000 1100 0000
    } eServiceType;

    /**
     * \brief   Returns string value of areg::eServiceType type
     **/
    inline const char * getString( areg::eServiceType srvcType );

    /**
     * \brief   areg::eMessageSource
     *          The source of the communication message
     **/
    typedef enum class E_MessageSource : uint16_t
    {
          MessageSourceUndefined    = 0     //!< The source of the message is undefined.
        , MessageSourceClient       = 1     //!< The source of the message is connected client application.
        , MessageSourceService      = 2     //!< The source of the message is the system service.
        , MessageSourceObserver     = 4     //!< The source of the message is any observer, including viewer.
        , MessageSourceTest         = 8     //!< The source of the message is a testing application.
        , MessageSourceSimulation   = 16    //!< The source of the message is a simulation application.
    } eMessageSource;

    /**
     * \brief   Returns string value of areg::eServiceType type
     **/
    inline const char * getString(areg::eMessageSource msgSource);

    /**
     * \brief   areg::eInstanceBitness
     *          The bitness of the executable instance.
     **/
    typedef enum class E_InstanceBitness: uint16_t
    {
          BitnessUnknown    = 0     //!< Unknown bitness
        , Bitness32         = 32    //!< 32-bit system
        , Bitness64         = 64    //!< 64-bit system
    } eInstanceBitness;

    /**
     * \brief   Returns string value of areg::eServiceType type
     **/
    inline const char* getString(areg::eInstanceBitness bitness);

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
    constexpr ITEM_ID   COOKIE_UNKNOWN              { static_cast<ITEM_ID>(areg::eCookies::CookieInvalid) };
    /**
     * \brief   areg::COOKIE_LOCAL
     *          The indication of local service.
     **/
    constexpr ITEM_ID   COOKIE_LOCAL                { static_cast<ITEM_ID>(areg::eCookies::CookieLocal) };
    /**
     * \brief   areg::COOKIE_ROUTER
     *          Indicates message router cookie
     **/
    constexpr ITEM_ID   COOKIE_ROUTER               { static_cast<ITEM_ID>(areg::eCookies::CookieRouter) };
    /**
     * \brief   areg::COOKIE_LOGGER
     *          Indicates log collector cookie
     **/
    constexpr ITEM_ID   COOKIE_LOGGER               { static_cast<ITEM_ID>(areg::eCookies::CookieLogger) };
    /**
     * \brief   areg::COOKIE_ANY
     *          Indicates any valid cookie
     **/
    constexpr ITEM_ID   COOKIE_ANY                  { static_cast<ITEM_ID>(areg::eCookies::CookieAny) };
    /**
     * \brief   areg::TARGET_UNKNOWN
     *          The unknown target ID
     **/
    constexpr ITEM_ID   TARGET_UNKNOWN              { static_cast<ITEM_ID>(areg::eCookies::CookieInvalid) };
    /**
     * \brief   areg::TARGET_LOCAL
     *          The local target ID
     **/
    constexpr ITEM_ID   TARGET_LOCAL                { static_cast<ITEM_ID>(areg::eCookies::CookieLocal) };
    /**
     * \brief   areg::TARGET_ALL
     *          The undefined (all) target ID
     **/
    constexpr ITEM_ID   TARGET_ALL                  { static_cast<ITEM_ID>(areg::COOKIE_ANY) };
    /**
     * \brief   areg::SOURCE_UNKNOWN
     *          The unknown source ID
     **/
    constexpr ITEM_ID   SOURCE_UNKNOWN              { static_cast<ITEM_ID>(areg::eCookies::CookieInvalid) };
    /**
     * \brief   areg::SOURCE_UNKNOWN
     *          The unknown source ID.
     **/
    constexpr ITEM_ID   SOURCE_LOCAL                { static_cast<ITEM_ID>(areg::eCookies::CookieLocal) };
    /**
     * \brief   areg::COOKIE_REMOTE_SERVICE
     *          The ID of first valid remote cookie.
     **/
    constexpr ITEM_ID   COOKIE_REMOTE_SERVICE       { static_cast<ITEM_ID>(areg::eCookies::CookieFirstRemote) };

    /**
     * \brief   areg::eServiceCalls
     *          Specifies the service call type
     **/
    typedef enum class E_ServiceCall  : uint16_t
    {
          ServiceCallNoFunction = static_cast<uint16_t>(eMessageDataType::UndefinedDataType) //!< No function call
        , ServiceCallRequest    = static_cast<uint16_t>(eMessageDataType::RequestDataType  ) //!< Call of service request function
        , ServiceCallResponse   = static_cast<uint16_t>(eMessageDataType::ResponseDataType ) //!< Call of service response function
        , ServiceCallAttribute  = static_cast<uint16_t>(eMessageDataType::AttributeDataType) //!< Call of service attribute update function
        , ServiceCallRegistery  = static_cast<uint16_t>(eMessageDataType::ServiceDataType  ) //!< Call of service registration
    } eServiceCalls;

    /**
     * \brief   Predefined range of function message IDs
     **/
    constexpr unsigned int  FUNC_RANGE          { static_cast<unsigned int>(4095) };  /*0x0FFF*/

    /**
     * \brief   The first ID of valid service interface function call
     **/
    constexpr unsigned int  SERVICE_FUNCTION    { static_cast<uint32_t>(eServiceCalls::ServiceCallRequest) };    /*0x1000*/

    /**
     * \brief   areg::REQUEST_ID_FIRST
     *          The first ID in request call.
     **/
    constexpr unsigned int  REQUEST_ID_FIRST    { static_cast<uint32_t>(eServiceCalls::ServiceCallRequest) };
    /**
     * \brief   areg::REQUEST_ID_LAST
     *          The last ID in request call.
     **/
    constexpr unsigned int  REQUEST_ID_LAST     { REQUEST_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   areg::RESPONSE_ID_FIRST
     *          The first ID in response call.
     **/
    constexpr unsigned int  RESPONSE_ID_FIRST   { static_cast<uint32_t>(eServiceCalls::ServiceCallResponse) };
    /**
     * \brief   areg::RESPONSE_ID_LAST
     *          The last ID in response call.
     **/
    constexpr unsigned int  RESPONSE_ID_LAST    { RESPONSE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   areg::ATTRIBUTE_ID_FIRST
     *          The first ID in attribute call.
     **/
    constexpr unsigned int  ATTRIBUTE_ID_FIRST  { static_cast<uint32_t>(eServiceCalls::ServiceCallAttribute) };
    /**
     * \brief   areg::ATTRIBUTE_ID_LAST
     *          The last ID in attribute call.
     **/
    constexpr unsigned int  ATTRIBUTE_ID_LAST   { ATTRIBUTE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   areg::SERVICE_ID_FIRST
     *          The last ID in service call.
     **/
    constexpr unsigned int  SERVICE_ID_FIRST    { static_cast<uint32_t>(areg::eServiceCalls::ServiceCallRegistery) };
    /**
     * \brief   areg::SERVICE_ID_LAST
     *          The last ID in service call.
     **/
    constexpr unsigned int  SERVICE_ID_LAST     { SERVICE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   areg::RESPONSE_ID_NONE
     *          Constant no response. Used to indicate that the request has no response.
     **/
    constexpr unsigned int  RESPONSE_ID_NONE    { static_cast<uint32_t>(eServiceCalls::ServiceCallNoFunction) };

    /**
     * \brief   areg::INVALID_MESSAGE_ID
     *          The invalid message ID
     **/
    constexpr unsigned int  INVALID_MESSAGE_ID  { static_cast<unsigned int>(~0) };    /*0xFFFFFFFF*/

    /**
     * \brief   Predefined range of function calls
     **/
    typedef enum class E_FuncIdRange  : uint32_t
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

    } eFuncIdRange;

    /**
     * \brief   Returns string value of areg::eFuncIdRange type
     **/
    inline const char * getString( areg::eFuncIdRange funcId );

    /**
     * \brief   Converts request message ID to array index. Returns INVALID_INDEX if empty.
     **/
    inline constexpr unsigned int reqIndex(unsigned int msgId)
    {
        return (msgId != static_cast<unsigned int>(eFuncIdRange::EmptyFunctionId))
                ? msgId - static_cast<unsigned int>(eFuncIdRange::RequestFirstId)
                : static_cast<unsigned int>(areg::INVALID_INDEX);
    }

    /**
     * \brief   Converts response message ID to array index. Returns INVALID_INDEX if empty.
     **/
    inline constexpr unsigned int respIndex(unsigned int msgId)
    {
        return (msgId != static_cast<unsigned int>(eFuncIdRange::EmptyFunctionId))
                ? msgId - static_cast<unsigned int>(eFuncIdRange::ResponseFirstId)
                : static_cast<unsigned int>(areg::INVALID_INDEX);
    }

    /**
     * \brief   Converts attribute message ID to array index. Returns INVALID_INDEX if empty.
     **/
    inline constexpr unsigned int attrIndex(unsigned int msgId)
    {
        return (msgId != static_cast<unsigned int>(eFuncIdRange::EmptyFunctionId))
                ? msgId - static_cast<unsigned int>(eFuncIdRange::AttributeFirstId)
                : static_cast<unsigned int>(areg::INVALID_INDEX);
    }

    /**
     * \brief   Returns true if message ID is in range of attribute call.
     **/
    inline bool isAttributeId(unsigned int msgId);

    /**
     * \brief   Returns true if message ID is in range of response call
     **/
    inline bool isResponseId(unsigned int msgId);

    /**
     * \brief   Returns true if message ID is in range of request call
     **/
    inline bool isRequestId(unsigned int msgId);

    /**
     * \brief   Returns true if message ID is a registration call
     **/
    inline bool isServiceRegistryId( unsigned int msgId );

    /**
     * \brief   Returns true if message ID is version notification
     **/
    inline bool isVersionNotifyId(unsigned int msgId);

    /**
     * \brief   Returns true if message ID is service connect
     **/
    inline bool isConnectNotifyId(unsigned int msgId);

    /**
     * \brief   Returns true if message ID is either request, or response, or attribute notification.
     **/
    inline bool isExecutableId( unsigned int msgId );

    /**
     * \brief   Returns true if message ID is empty function
     **/
    inline bool isEmptyFunctionId(unsigned int msgId);

    class ParameterArray;

    //////////////////////////////////////////////////////////////////////////
    // StateArray class declaration
    //////////////////////////////////////////////////////////////////////////
    using StateArrayBase    = FixedArray<areg::eDataStateType>;
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
        StateArray(unsigned char* thisBffer, int elemCount);

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
        inline const areg::eDataStateType& operator [] (unsigned int index) const;
        /**
         * \brief   Returns element by the index for reading. The index must be valid.
         **/
        inline areg::eDataStateType& operator [] (unsigned int index);

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
        inline void setState(int whichIndex, areg::eDataStateType newState);

        /**
         * \brief   All entries are set to the same given state
         * \param   newState    The state to set for all entries
         **/
        inline void setAllState(areg::eDataStateType newState);

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
    // S_InterfaceData structure declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Interface data structure. Contains information of service
     *          interface required by Proxy object. Should be initialized
     *          for every service interface.
     **/
    typedef struct AREG_API S_InterfaceData
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
        eServiceType        idServiceType{ eServiceType::ServiceInvalid };

        /**
         * \brief   Number of requests in service interface
         **/
        unsigned int        idRequestCount{ 0u };

        /**
         * \brief   Number of responses in service interface, including broadcasts
         **/
        unsigned int        idResponseCount{ 0u };

        /**
         * \brief   Number of attributes in service interface
         **/
        unsigned int        idAttributeCount{ 0u };

        /**
         * \brief   The list of requests. It is nullptr if the list is empty.
         **/
        const unsigned int* idRequestList{ nullptr };

        /**
         * \brief   The list of responses. It is nullptr if the list is empty.
         **/
        const unsigned int* idResponseList{ nullptr };

        /**
         * \brief   The list of attributes. It is nullptr if the list is empty.
         **/
        const unsigned int* idAttributeList{ nullptr };

        /**
         * \brief   Request to Response map. All requests are accessed by index 
         *          and every request indexed is calculated by calculating 
         *          ('request ID' - areg::eFuncIdRange::RequestFirstId)
         *          Every request should have appropriate response value. If request does not
         *          have response, it should have value areg::RESPONSE_ID_NONE
         *          The size of this map should be equal to idRequestCount.
         **/
        const unsigned int* idRequestToResponseMap{ nullptr };

        /**
         * \brief   Map of parameter count in every response. Every response index
         *          is calculated by formula ('response ID' - areg::eFuncIdRange::ResponseFirstId)
         *          The size of this map should be equal to idResponseCount.
         **/
        const unsigned int* idResponseParamCountMap{ nullptr };

    } SInterfaceData;

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
     * \brief   Returns the instance of empty local service interface.
     **/
    AREG_API areg::SInterfaceData & getEmptyInterface();

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
     *          responseAREG(const bool &param1, const int &param2)
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
        ParameterArray(const areg::SInterfaceData& ifData);


        /**
         * \brief   Initialization constructor. Initialize Parameter
         *          State Array from given parameter count map,
         *          which contains map table of parameter counts in every response.
         * \param   paramCountMap   The map table of number of parameters in every response
         * \param   count           Number of entries in map table
         **/
        ParameterArray(const unsigned int* paramCountMap, int count);

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
         *          ('response ID' - areg::eFuncIdRange::ResponseFirstId) or use
         *          areg::respIndex() macro
         **/
        inline areg::StateArray & operator [] (unsigned int index);
        /**
         * \brief   Access read-only state by given index of array.
         *          The index is calculated by formula
         *          ('response ID' - areg::eFuncIdRange::ResponseFirstId) or use
         *          areg::respIndex() macro
         **/
        inline const areg::StateArray & operator [] (unsigned int index) const;

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
        inline areg::eDataStateType getAt(unsigned int row, unsigned int col) const;

        /**
         * \brief   Sets state of parameter of certain response.
         * \param   row         The index of response entry, which can be calculated
         *                      by macro areg::respIndex('response ID')
         * \param   col         The index of parameter in response call. 
         *                      First parameter has index zero
         * \param   newValue    The state to set for parameter
         **/
        inline void setAt(unsigned int row, unsigned int col, areg::eDataStateType newValue);

        /**
         * \brief   Returns true if specified response index has parameters.
         *          The response index is calculated by macro areg::respIndex('response ID')
         **/
        inline bool hasParameters(unsigned int whichRespIndex) const;
        /**
         * \brief   Returns true if valid response index. For example,
         *          the response ID RESPONSE_ID_NONE does not have valid
         *          response index. The index of response is calculated
         *          by macro areg::respIndex('response ID')
         * \param   whichRespIndex  The index of response, should be calculated by
         *                          macro areg::respIndex('response ID')
         * \return  Returns true if given index is valid
         **/
        inline bool isValidParamIndex(unsigned int whichRespIndex) const;

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
        inline void setParamState(unsigned int whichParam, areg::eDataStateType newState);

        /**
         * \brief   Resets all states of parameter of specified entry index.
         *          The index should be calculated by areg::respIndex('response ID')
         * \param   whichParam  The index of entry, should be calculated by areg::respIndex('response ID')
         **/
        void resetParamState(unsigned int whichParam);

    //////////////////////////////////////////////////////////////////////////
    // Hidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Constructs the parameter array object.
         * \param   params  Parameter count table
         * \param   count   Number of entries in table
         **/
        void construct(const unsigned int* params, int count);

        /**
         * \brief   Counts size in bytes required for states
         **/
        unsigned int countParamSpace(const unsigned int* params, int count);

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    private:

        /**
         * \brief   Number of entries in parameter array.
         *          Should be equal to number of response in service interface
         **/
        int                         mElemCount;

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
        ProxyData(const areg::SInterfaceData & ifData);
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
         * \brief	Set state for specified request ID (range in areg::eFuncIdRange)
         *          First, it will figure our whether it is attribute or response request ID.
         *          Then it will convert to index value and set appropriate entry state.
         *          If the request ID is areg::ATTRIBUTE_SI_VERSION, it will only set
         *          state of component implementation version.
         * \param	msgId	    The function ID in range areg::eFuncIdRange.
         *                      Can be either attribute or response range ID.
         *                      Otherwise it is ignored.
         * \param	newState	New state to set. If requested ID is response range,
         *                      it will set state for all parameters of that response, 
         *                      if any exist.
         **/
        void setDataState(unsigned int msgId, areg::eDataStateType newState);

        /**
         * \brief   Returns data state of given message (attribute or response) ID.
         **/
        areg::eDataStateType getDataState(unsigned int msgId) const;

        /**
         * \brief   Returns the response message ID of given request message ID.
         *          If message is wrong and/or is not in request to response map,
         *          it will return areg::INVALID_MESSAGE_ID
         **/
        unsigned int getResponseId(unsigned int requestId) const;

        /**
         * \brief   Returns data state of given attribute ID
         **/
        inline areg::eDataStateType getAttributeState(unsigned int msgId) const;

        /**
         * \brief   Returns parameter state of given response ID
         **/
        inline areg::eDataStateType getParamState(unsigned int msgId) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Implementation version
         **/
        areg::eDataStateType           mImplVersion;   // implementation version

        /**
         * \brief   Service Interface data
         **/
        const areg::SInterfaceData &   mIfData;        // fixed service interface data

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
     * \brief   areg::sServiceConnectedInstance
     *          Service connected instance of application.
     **/
    struct sServiceConnectedInstance
    {
        //!< The type of the application
        areg::eMessageSource   ciSource    { areg::eMessageSource::MessageSourceUndefined };
        //!< The bit-set of connected instance
        areg::eInstanceBitness ciBitness   { areg::eInstanceBitness::BitnessUnknown };
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
    using MapInstances = OrderedMap<ITEM_ID, areg::sServiceConnectedInstance>;
AREG_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////
// Global namespace NEService inline function implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_STREAMABLE(areg::ResultType)
AREG_IMPLEMENT_STREAMABLE(areg::eDataStateType)
AREG_IMPLEMENT_STREAMABLE(areg::eRequestType)
AREG_IMPLEMENT_STREAMABLE(areg::eMessageDataType)
AREG_IMPLEMENT_STREAMABLE(areg::eServiceConnection)
AREG_IMPLEMENT_STREAMABLE(areg::eDisconnectReason)
AREG_IMPLEMENT_STREAMABLE(areg::eServiceRequestType)
AREG_IMPLEMENT_STREAMABLE(areg::eServiceType)
AREG_IMPLEMENT_STREAMABLE(areg::eInstanceBitness)
AREG_IMPLEMENT_STREAMABLE(areg::eMessageSource)
AREG_IMPLEMENT_STREAMABLE(areg::eFuncIdRange)

//////////////////////////////////////////////////////////////////////////
// namespace NEService inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool areg::isServiceConnected(areg::eServiceConnection connectionStatus)
{
    return (connectionStatus == areg::eServiceConnection::ServiceConnected);
}

inline bool areg::isServiceConnectionPending( areg::eServiceConnection connectionStatus )
{
    return (connectionStatus == areg::eServiceConnection::ServicePending);
}

inline bool areg::isServiceRejected( areg::eServiceConnection connectionStatus )
{
    return (connectionStatus == areg::eServiceConnection::ServiceRejected);
}

inline bool areg::isServiceConnectionLost( areg::eServiceConnection connectionStatus )
{
    return (connectionStatus == areg::eServiceConnection::ServiceConnectionLost);
}

inline bool areg::isServiceDisconnected( areg::eServiceConnection connectionStatus )
{
    return (connectionStatus != areg::eServiceConnection::ServiceConnected);
}

inline areg::eServiceConnection areg::serviceConnection( areg::eDisconnectReason reason )
{
    switch ( reason )
    {
    case areg::eDisconnectReason::ReasonUndefined:
        return areg::eServiceConnection::ServiceConnectionUnknown;
    
    case areg::eDisconnectReason::ReasonServiceDisconnected:
    case areg::eDisconnectReason::ReasonServiceLost:
    case areg::eDisconnectReason::ReasonProviderLost:
    case areg::eDisconnectReason::ReasonConsumerLost:
    case areg::eDisconnectReason::ReasonClientConnectionLost:
        return areg::eServiceConnection::ServiceConnectionLost;

    case areg::eDisconnectReason::ReasonServiceRejected:
    case areg::eDisconnectReason::ReasonProviderRejected:
    case areg::eDisconnectReason::ReasonConsumerNotSupported:
        return areg::eServiceConnection::ServiceRejected;

    case areg::eDisconnectReason::ReasonConsumerDisconnected:
    case areg::eDisconnectReason::ReasonProviderDisconnected:
    case areg::eDisconnectReason::ReasonClientConnectionClosed:
        return areg::eServiceConnection::ServiceDisconnected;

    case areg::eDisconnectReason::ReasonSystemShutdown:
        return areg::eServiceConnection::ServiceShutdown;

    default:
        return areg::eServiceConnection::ServiceConnected;
    }
}

inline bool areg::isRequestId(unsigned int msgId)
{
    return ((msgId & static_cast<unsigned int>(areg::eServiceCalls::ServiceCallRequest)) != 0);
}

inline bool areg::isResponseId(unsigned int msgId)
{
    return ((msgId & static_cast<unsigned int>(areg::eServiceCalls::ServiceCallResponse)) != 0);
}

inline bool areg::isAttributeId(unsigned int msgId)
{
    return ((msgId & static_cast<unsigned int>(areg::eServiceCalls::ServiceCallAttribute)) != 0);
}

inline bool areg::isServiceRegistryId( unsigned int msgId )
{
    return ((msgId & static_cast<unsigned int>(areg::eServiceCalls::ServiceCallRegistery)) != 0);
}

inline bool areg::isEmptyFunctionId(unsigned int msgId)
{
    return (msgId == static_cast<unsigned int>(areg::eFuncIdRange::EmptyFunctionId));
}

inline bool areg::isVersionNotifyId( unsigned int msgId )
{
    return (msgId == static_cast<unsigned int>(areg::eFuncIdRange::ResponseServiceProviderVersion));
}

inline bool areg::isConnectNotifyId( unsigned int msgId )
{
    return (msgId == static_cast<unsigned int>(areg::eFuncIdRange::ResponseServiceProviderConnection));
}

inline bool areg::isExecutableId(unsigned int msgId)
{
    return ( (msgId  & static_cast<unsigned int>(areg::eServiceCalls::ServiceCallRequest)     ) != 0 ||
             (msgId  & static_cast<unsigned int>(areg::eServiceCalls::ServiceCallResponse)    ) != 0 ||
             (msgId  & static_cast<unsigned int>(areg::eServiceCalls::ServiceCallAttribute)   ) != 0 ||
             (msgId == static_cast<unsigned int>(areg::eServiceCalls::ServiceCallNoFunction)  ) );
}


inline areg::eMessageDataType areg::getMessageDataType( unsigned int msgId )
{
    if ( areg::isRequestId(msgId) )
        return areg::eMessageDataType::RequestDataType;
    else if (areg::isResponseId(msgId))
        return areg::eMessageDataType::ResponseDataType;
    else if (areg::isAttributeId(msgId))
        return areg::eMessageDataType::AttributeDataType;
    else if (areg::isServiceRegistryId(msgId))
        return areg::eMessageDataType::ServiceDataType;
    else
        return areg::eMessageDataType::UndefinedDataType;
}

//////////////////////////////////////////////////////////////////////////
// class areg::StateArray inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const areg::eDataStateType& areg::StateArray::operator [] (unsigned int index) const
{
    return StateArrayBase::operator[](index);
}

inline areg::eDataStateType& areg::StateArray::operator [] (unsigned int index)
{
    return StateArrayBase::operator[](index);
}

inline uint32_t areg::StateArray::getSize() const
{
    return StateArrayBase::getSize();
}

inline void areg::StateArray::resetStates()
{
    setAllState(areg::eDataStateType::DataIsUnavailable);
}

inline bool areg::StateArray::hasParams() const
{
    return (isEmpty() == false);
}

inline void areg::StateArray::setState(int whichIndex, areg::eDataStateType newState)
{
    mValueList[whichIndex] = newState;
}

inline void areg::StateArray::setAllState(areg::eDataStateType newState)
{
    for ( uint32_t i = 0; i < this->mElemCount; ++ i )
        mValueList[i] = newState;
}

//////////////////////////////////////////////////////////////////////////
// class areg::ParameterArray inline function implementation
//////////////////////////////////////////////////////////////////////////
inline areg::StateArray& areg::ParameterArray::operator [] ( unsigned int index )
{
    ASSERT(isValidParamIndex(index));
    return *(mParamList[index]);
}

inline const areg::StateArray& areg::ParameterArray::operator [] ( unsigned int index ) const
{
    ASSERT(isValidParamIndex(index));
    return *(mParamList[index]);
}

inline areg::eDataStateType areg::ParameterArray::getAt( unsigned int row, unsigned int col ) const
{
    ASSERT(isValidParamIndex(row) && mParamList[row]->isValidIndex(static_cast<uint32_t>(col)));
    return mParamList[row]->getAt(static_cast<uint32_t>(col));
}

inline void areg::ParameterArray::setAt( unsigned int row, unsigned int col, areg::eDataStateType newValue )
{
    ASSERT(isValidParamIndex(row) && mParamList[row]->isValidIndex(static_cast<uint32_t>(col)));
    mParamList[row]->setAt(static_cast<uint32_t>(col), newValue);
}

inline bool areg::ParameterArray::hasParameters( unsigned int whichRespIndex ) const
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

inline bool areg::ParameterArray::isValidParamIndex(unsigned int whichRespIndex) const
{
    return ((static_cast<int>(whichRespIndex) >= 0) && (static_cast<int>(whichRespIndex) < mElemCount));
}

inline void areg::ParameterArray::setParamState(unsigned int whichParam, areg::eDataStateType newState)
{
    ASSERT(isValidParamIndex(whichParam));
    mParamList[whichParam]->setAllState(newState);
}

//////////////////////////////////////////////////////////////////////////
// class areg::ProxyData inline function implementation
//////////////////////////////////////////////////////////////////////////

inline areg::eDataStateType areg::ProxyData::getAttributeState( unsigned int msgId ) const
{
    return areg::isVersionNotifyId(msgId) ? mImplVersion : mAttrState[areg::attrIndex(msgId)];
}

inline areg::eDataStateType areg::ProxyData::getParamState( unsigned int msgId ) const
{
    const areg::StateArray& param = mParamState[areg::respIndex(msgId)];
    return (param.hasParams() ? param[0u] : areg::eDataStateType::DataIsUnavailable);
}

//////////////////////////////////////////////////////////////////////////
// class areg::sServiceConnectedInstance serialization
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Serializes the instance structure to the stream.
 * \param   stream  The streaming object to serialize.
 * \param   output  The single structure of instance to serialize.
 **/
inline OutStream& operator << (OutStream& stream, const areg::sServiceConnectedInstance & output)
{
    stream << output.ciSource << output.ciBitness << output.ciCookie << output.ciTimestamp << output.ciInstance << output.ciLocation;
    return stream;
}

/**
 * \brief   De-serializes the instance structure from the stream.
 * \param   stream  The streaming object that contains the information of the connected instance.
 * \param   input  The single structure of instance to initialize.
 **/
inline const InStream& operator >> (const InStream& stream, areg::sServiceConnectedInstance & input)
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
    case    areg::ResultType::ServiceInvalid:
        return "areg::ResultType::ServiceInvalid";
    case    areg::ResultType::ServiceRejected:
        return "areg::ResultType::ServiceRejected";

    default:
        ASSERT(false);
        return "ERR: Undefined areg::ResultType value!";
    }
}

inline const char* areg::getString(areg::eDataStateType dataState)
{
    switch (dataState)
    {
    case    areg::eDataStateType::DataIsUndefined:
        return "areg::eDataStateType::DataIsUndefined";

    case    areg::eDataStateType::DataIsOK:
        return "areg::eDataStateType::DataIsOK";

    case    areg::eDataStateType::DataIsInvalid:
        return "areg::eDataStateType::DataIsInvalid";

    case    areg::eDataStateType::DataIsUnavailable:
        return "areg::eDataStateType::DataIsUnavailable";

    case    areg::eDataStateType::DataUnexpectedError:
        return "areg::eDataStateType::DataUnexpectedError";

    default:
        ASSERT(false);
        return "ERR: Undefined areg::eDataStateType value!";
    }
}

inline const char* areg::getString( areg::eRequestType resultType )
{
    switch (resultType)
    {
    case    areg::eRequestType::Unprocessed:
        return "areg::eRequestType::Unprocessed";
    case    areg::eRequestType::StartNotify:
        return "areg::eRequestType::StartNotify";
    case    areg::eRequestType::StopNotify:
        return "areg::eRequestType::StopNotify";        
    case    areg::eRequestType::RemoveAllNotify:
        return "areg::eRequestType::RemoveAllNotify";
    case    areg::eRequestType::CallFunction:
        return "areg::eRequestType::CallFunction";
    case    areg::eRequestType::ServiceConnection:
        return "areg::eRequestType::ServiceConnection";
    case    areg::eRequestType::ClientConnection:
        return "areg::eRequestType::ClientConnection";
    case    areg::eRequestType::LoadComponent:
        return "areg::eRequestType::LoadComponent";
    default:
        ASSERT(false);
        return "ERR: Undefined areg::eRequestType value!";
    }
}

inline const char* areg::getString( areg::eMessageDataType dataType )
{
    switch (dataType)
    {
    case areg::eMessageDataType::UndefinedDataType:
        return "areg::eMessageDataType::UndefinedDataType";
    case areg::eMessageDataType::RequestDataType:
        return "areg::eMessageDataType::RequestDataType";
    case areg::eMessageDataType::ResponseDataType:
        return "areg::eMessageDataType::ResponseDataType";
    case areg::eMessageDataType::AttributeDataType:
        return "areg::eMessageDataType::AttributeDataType";
    case areg::eMessageDataType::ServiceDataType:
        return "areg::eMessageDataType::ServiceDataType";

    default:
        ASSERT(false);
        return "ERR: Undefined areg::eMessageDataType value!";
    }
}

inline const char* areg::getString( areg::eServiceConnection serviceConnection )
{
    switch (serviceConnection)
    {
    case areg::eServiceConnection::ServiceConnectionUnknown:
        return "areg::eServiceConnection::ServiceConnectionUnknown";
    case areg::eServiceConnection::ServiceConnected:
        return "areg::eServiceConnection::ServiceConnected";
    case areg::eServiceConnection::ServicePending:
        return "areg::eServiceConnection::ServicePending";
    case areg::eServiceConnection::ServiceDisconnected:
        return "areg::eServiceConnection::ServiceDisconnected";
    case areg::eServiceConnection::ServiceConnectionLost:
        return "areg::eServiceConnection::ServiceConnectionLost";
    case areg::eServiceConnection::ServiceRejected:
        return "areg::eServiceConnection::ServiceRejected";
    case areg::eServiceConnection::ServiceFailed:
        return "areg::eServiceConnection::ServiceFailed";
    case areg::eServiceConnection::ServiceShutdown:
        return "areg::eServiceConnection::ServiceShutdown";
    default:
        ASSERT(false);
        return "ERR: Undefined areg::eServiceConnection value!";
    }
}

inline const char * areg::getString( areg::eDisconnectReason reason )
{
    switch ( reason )
    {
    case areg::eDisconnectReason::ReasonUndefined:
        return "areg::eDisconnectReason::ReasonUndefined";
    case areg::eDisconnectReason::ReasonServiceDisconnected:
        return "areg::eDisconnectReason::ReasonServiceDisconnected";
    case areg::eDisconnectReason::ReasonServiceLost:
        return "areg::eDisconnectReason::ReasonServiceLost";
    case areg::eDisconnectReason::ReasonServiceRejected:
        return "areg::eDisconnectReason::ReasonServiceRejected";
    case areg::eDisconnectReason::ReasonProviderDisconnected:
        return "areg::eDisconnectReason::ReasonProviderDisconnected";
    case areg::eDisconnectReason::ReasonProviderLost:
        return "areg::eDisconnectReason::ReasonProviderLost";
    case areg::eDisconnectReason::ReasonProviderRejected:
        return "areg::eDisconnectReason::ReasonProviderRejected";
    case areg::eDisconnectReason::ReasonConsumerDisconnected:
        return "areg::eDisconnectReason::ReasonConsumerDisconnected";
    case areg::eDisconnectReason::ReasonConsumerLost:
        return "areg::eDisconnectReason::ReasonConsumerLost";
    case areg::eDisconnectReason::ReasonConsumerNotSupported:
        return "areg::eDisconnectReason::ReasonConsumerNotSupported";
    case areg::eDisconnectReason::ReasonSystemShutdown:
        return "areg::eDisconnectReason::ReasonSystemShutdown";
    case areg::eDisconnectReason::ReasonClientConnectionLost:
        return "areg::eDisconnectReason::ReasonClientConnectionLost";
    case areg::eDisconnectReason::ReasonClientConnectionClosed:
        return "areg::eDisconnectReason::ReasonClientConnectionClosed";
    default:
        ASSERT( false );
        return "ERR: Undefined areg::eDisconnectReason value!";
    }
}

inline const char * areg::getString( areg::eServiceRequestType svcRequestType )
{
    switch ( svcRequestType )
    {
    case areg::eServiceRequestType::RegisterClient:
        return "areg::eServiceRequestType::RegisterClient";
    case areg::eServiceRequestType::UnregisterClient:
        return "areg::eServiceRequestType::UnregisterClient";
    case areg::eServiceRequestType::RegisterStub:
        return "areg::RegisterStub";
    case areg::eServiceRequestType::UnregisterStub:
        return "areg::eServiceRequestType::UnregisterStub";
    default:
        return "ERR: Unexpected areg::eServiceRequestType value!!!";
    }
}

inline const char * areg::getString( areg::eServiceType srvcType )
{
    switch ( srvcType )
    {
    case areg::eServiceType::ServiceLocal:
        return "areg::eServiceType::ServiceLocal";
    case areg::eServiceType::ServicePublic:
        return "areg::eServiceType::ServicePublic";
    case areg::eServiceType::ServiceAny:
        return "areg::eServiceType::ServiceAny";
    case areg::eServiceType::ServiceInvalid:
        return "areg::eServiceType::ServiceInvalid";
    default:
        return "ERR: Unexpected areg::eServiceRequestType value!!!";
    }
}

const char* areg::getString(areg::eInstanceBitness bitness)
{
    switch (bitness)
    {
    case areg::eInstanceBitness::Bitness32:
        return "areg::eInstanceBitness::Bitness32";
    case areg::eInstanceBitness::Bitness64:
        return "areg::eInstanceBitness::Bitness64";
    case areg::eInstanceBitness::BitnessUnknown:
        return "areg::eInstanceBitness::BitnessUnknown";
    default:
        return "ERR: Unexpected areg::eInstanceBitness value!!!";
    }
}


const char * areg::getString(areg::eMessageSource msgSource)
{
    switch (msgSource)
    {
    case areg::eMessageSource::MessageSourceUndefined:
        return "areg::eMessageSource::MessageSourceUndefined";
    case areg::eMessageSource::MessageSourceClient:
        return "areg::eMessageSource::MessageSourceClient";
    case areg::eMessageSource::MessageSourceService:
        return "areg::eMessageSource::MessageSourceService";
    case areg::eMessageSource::MessageSourceObserver:
        return "areg::eMessageSource::MessageSourceObserver";
    case areg::eMessageSource::MessageSourceTest:
        return "areg::eMessageSource::MessageSourceTest";
    case areg::eMessageSource::MessageSourceSimulation:
        return "areg::eMessageSource::MessageSourceSimulation";
    default:
        return "ERR: Unexpected areg::eMessageSource value!!!";
    }
}


inline const char * areg::getString( areg::eFuncIdRange funcId )
{
    switch ( funcId )
    {
    case areg::eFuncIdRange::EmptyFunctionId:
        return "areg::eFuncIdRange::EmptyFunctionId";
    case areg::eFuncIdRange::ComponentCleanup:
        return "areg::eFuncIdRange::ComponentCleanup";
    case areg::eFuncIdRange::RequestRegisterService:
        return "areg::eFuncIdRange::RequestRegisterService";
    case areg::eFuncIdRange::RequestServiceProviderVersion:
        return "areg::eFuncIdRange::RequestServiceProviderVersion";
    case areg::eFuncIdRange::ResponseServiceProviderVersion:
        return "areg::eFuncIdRange::ResponseServiceProviderVersion";
    case areg::eFuncIdRange::RequestServiceProviderConnection:
        return "areg::eFuncIdRange::RequestServiceProviderConnection";
    case areg::eFuncIdRange::ResponseServiceProviderConnection:
        return "areg::eFuncIdRange::ResponseServiceProviderConnection";
    case areg::eFuncIdRange::SystemServiceConnect:
        return "areg::eFuncIdRange::SystemServiceConnect";
    case areg::eFuncIdRange::SystemServiceDisconnect:
        return "areg::eFuncIdRange::SystemServiceDisconnect";
    case areg::eFuncIdRange::SystemServiceNotifyConnection:
        return "areg::eFuncIdRange::SystemServiceNotifyConnection";
    case areg::eFuncIdRange::SystemServiceQueryInstances:
        return "areg::eFuncIdRange::SystemServiceQueryInstances";
    case areg::eFuncIdRange::SystemServiceNotifyInstances:
        return "areg::eFuncIdRange::SystemServiceNotifyInstances";
    case areg::eFuncIdRange::SystemServiceRequestRegister:
        return "areg::eFuncIdRange::SystemServiceRequestRegister";
    case areg::eFuncIdRange::SystemServiceNotifyRegister:
        return "areg::eFuncIdRange::SystemServiceNotifyRegister";
    case areg::eFuncIdRange::ServiceLogRegisterScopes:
        return "areg::eFuncIdRange::ServiceLogRegisterScopes";
    case areg::eFuncIdRange::ServiceLogUpdateScopes:
        return "areg::eFuncIdRange::ServiceLogUpdateScopes";
    case areg::eFuncIdRange::ServiceLogQueryScopes:
        return "areg::eFuncIdRange::ServiceLogQueryScopes";
    case areg::eFuncIdRange::ServiceLogScopesUpdated:
        return "areg::eFuncIdRange::ServiceLogScopesUpdated";
    case areg::eFuncIdRange::ServiceSaveLogConfiguration:
        return "areg::eFuncIdRange::ServiceSaveLogConfiguration";
    case areg::eFuncIdRange::ServiceLogConfigurationSaved:
        return "areg::eFuncIdRange::ServiceLogConfigurationSaved";
    case areg::eFuncIdRange::ServiceLogMessage:
        return "areg::eFuncIdRange::ServiceLogMessage";
    case areg::eFuncIdRange::RequestFirstId:
        return "areg::eFuncIdRange::RequestFirstId";
    case areg::eFuncIdRange::ResponseFirstId:
        return "areg::eFuncIdRange::ResponseFirstId";
    case areg::eFuncIdRange::AttributeFirstId:
        return "areg::eFuncIdRange::AttributeFirstId";
    case areg::eFuncIdRange::RequestLastId:
        return "areg::eFuncIdRange::RequestLastId";
    case areg::eFuncIdRange::ResponseLastId:
        return "areg::eFuncIdRange::ResponseLastId";
    case areg::eFuncIdRange::AttributeLastId:
        return "areg::eFuncIdRange::AttributeLastId";
    case areg::eFuncIdRange::ServiceLastId:
        return "areg::eFuncIdRange::ServiceLastId";

    default:
        {
            if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(areg::eFuncIdRange::RequestFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(areg::eFuncIdRange::RequestLastId)) )
                return "Request ID range";
            else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(areg::eFuncIdRange::ResponseFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(areg::eFuncIdRange::ResponseLastId)) )
                return "Response ID range";
            else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(areg::eFuncIdRange::AttributeFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(areg::eFuncIdRange::AttributeLastId)) )
                return "Attribute ID range";
            else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(areg::eFuncIdRange::RequestRegisterService)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(areg::eFuncIdRange::ServiceLastId)) )
                return "Service registration ID";
            else
                return "ERR: Unexpected ID";
        }
    }
}

#endif  // AREG_COMPONENT_SERVICEDEFS_HPP
