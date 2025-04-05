#ifndef AREG_COMPONENT_NESERVICE_HPP
#define AREG_COMPONENT_NESERVICE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/NEService.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, NEService namespace contains
 *              collection of classes, structures and types defining
 *              Service Interface.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/TEMap.hpp"
#include "areg/base/NECommon.hpp"
#include "areg/base/String.hpp"
#include "areg/base/TEFixedArray.hpp"
#include "areg/base/Version.hpp"

/************************************************************************
 * predefined macro
 ************************************************************************/

/**
 * \brief   Converts request message id to index
 **/
#define GET_REQ_INDEX(msgId)           static_cast<unsigned int>(static_cast<unsigned int>(msgId) != static_cast<unsigned int>(NEService::eFuncIdRange::EmptyFunctionId) ? static_cast<unsigned int>(msgId) - static_cast<unsigned int>(NEService::eFuncIdRange::RequestFirstId) : static_cast<unsigned int>(NECommon::INVALID_INDEX))

/**
 * \brief   Converts response message id to index
 **/
#define GET_RESP_INDEX(msgId)          static_cast<unsigned int>(static_cast<unsigned int>(msgId) != static_cast<unsigned int>(NEService::eFuncIdRange::EmptyFunctionId) ? static_cast<unsigned int>(msgId) - static_cast<unsigned int>(NEService::eFuncIdRange::ResponseFirstId) : static_cast<unsigned int>(NECommon::INVALID_INDEX))

/**
 * \brief   Converts attribute message id to index
 **/
#define GET_ATTR_INDEX(msgId)          static_cast<unsigned int>(static_cast<unsigned int>(msgId) != static_cast<unsigned int>(NEService::eFuncIdRange::EmptyFunctionId) ? static_cast<unsigned int>(msgId) - static_cast<unsigned int>(NEService::eFuncIdRange::AttributeFirstId) : static_cast<unsigned int>(NECommon::INVALID_INDEX))

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
    typedef enum class E_ResultType : uint16_t
    {
        /* not used */
          Undefined         =     0 /*0x0000*/  //!< undefined result. not used             Bits: 0000 0000 0000 0000
        /* additional bits */
        , Error             =     1 /*0x0001*/  //!< indicate error. contains error bit     Bits: 0000 0000 0000 0001
        , Undelivered       =    32 /*0x0020*/  //!< message was not delivered              Bits: 0000 0000 0010 0000
        , NotProcessed      =    64 /*0x0040*/  //!< call did not reach target              Bits: 0000 0000 0100 0000
        , Processed         =   128 /*0x0080*/  //!< call reached target                    Bits: 0000 0000 1000 0000

         /* Message did not reach target, used in remote messaging */
        , MessageUndelivered=  4129 /*0x1021*/  //!< request failed to reach target.        Bits: 0001 0000 0010 0001

        /* request calls result */
        , RequestOK         =  8320 /*0x2080*/  //!< indicates success of request call.     Bits: 0010 0000 1000 0000
        , RequestInvalid    =  8257 /*0x2041*/  //!< indicates failure of request call.     Bits: 0010 0000 0100 0001
        , RequestError      =  8321 /*0x2081*/  //!< indicates request execution failure.   Bits: 0010 0000 1000 0001
        , RequestBusy       =  8323 /*0x2083*/  //!< request cannot execute, it is busy.    Bits: 0010 0000 1000 0011
        , RequestCanceled   =  8325 /*0x2085*/  //!< request is canceled and not executed.  Bits: 0010 0000 1000 0101

        /* data update result */
        , DataOK            = 16512 /*0x4080*/  //!< indicates data validation.             Bits: 0100 0000 1000 0000
        , DataInvalid       = 16449 /*0x4041*/  //!< indicates data invalid.                Bits: 0100 0000 0100 0001

        /* service call result */
        , ServiceOK         = 32896 /*0x8080*/  //!< service call processed.                Bits: 1000 0000 1000 0000
        , ServiceUnavailable= 32833 /*0x8041*/  //!< service is unavailable.                Bits: 1000 0000 0100 0001
        , ServiceInvalid    = 32897 /*0x8081*/  //!< service invalid (check cookie).        Bits: 1000 0000 1000 0001
        , ServiceRejected   = 32899 /*0x8083*/  //!< service rejected (unsupported).        Bits: 1000 0000 1000 0011

    } eResultType;
    /**
     * \brief   Returns string value of NEService::eResultType type
     **/
    inline const char* getString(NEService::eResultType resultType);

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
     * \brief   Returns string value of NEService::eDataStateType type
     **/
    inline const char* getString(NEService::eDataStateType dataState);

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
     * \brief   Returns string value of NEService::eRequestType type
     **/
    inline const char* getString( NEService::eRequestType resultType );

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
     * \brief   Returns string value of NEService::eMessageDataType type
     **/
    inline const char* getString( NEService::eMessageDataType dataType );

    /**
     * \brief   From passed message ID finds data type
     **/
    inline NEService::eMessageDataType getMessageDataType( unsigned int msgId );

    /**
     * \brief   NEService::eServiceConnection
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
     * \brief   Returns string value of NEService::eDataType type
     **/
    inline const char* getString( NEService::eServiceConnection serviceConnection );

    /**
     * \brief   Returns true if the service connection status is connected.
     **/
    inline bool isServiceConnected( NEService::eServiceConnection connectionStatus );

    /**
     * \brief   Returns true if the service connection status is connected.
     **/
    inline bool isServiceConnectionPending( NEService::eServiceConnection connectionStatus );

    /**
     * \brief   Returns true if the service connection status is rejected.
     **/
    inline bool isServiceRejected( NEService::eServiceConnection connectionStatus );

    /**
     * \brief   Returns true if the service connection status is connection lost.
     **/
    inline bool isServiceConnectionLost( NEService::eServiceConnection connectionStatus );

    /**
     * \brief   Returns true if the status is not connected.
     **/
    inline bool isServiceDisconnected( NEService::eServiceConnection connectionStatus );

    /**
     * \brief   NEService::eDisconnectReason
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

    inline const char * getString( NEService::eDisconnectReason reason );

    inline NEService::eServiceConnection serviceConnection( NEService::eDisconnectReason reason );

    /**
     * \brief   NEService::eServiceRequestType
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
     * \brief   Returns string value of NEService::eServiceRequestType type
     **/
    inline const char * getString( NEService::eServiceRequestType svcRequestType );

    /**
     * \brief   NEService::eServiceType
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
     * \brief   Returns string value of NEService::eServiceType type
     **/
    inline const char * getString( NEService::eServiceType srvcType );

    /**
     * \brief   NEService::eMessageSource
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
     * \brief   Returns string value of NEService::eServiceType type
     **/
    inline const char * getString(NEService::eMessageSource msgSource);

    /**
     * \brief   NEService::eInstanceBitness
     *          The bitness of the executable instance.
     **/
    typedef enum class E_InstanceBitness: uint16_t
    {
          BitnessUnknown    = 0     //!< Unknown bitness
        , Bitness32         = 32    //!< 32-bit system
        , Bitness64         = 64    //!< 64-bit system
    } eInstanceBitness;

    /**
     * \brief   Returns string value of NEService::eServiceType type
     **/
    inline const char* getString(NEService::eInstanceBitness bitness);

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
    constexpr ITEM_ID   COOKIE_UNKNOWN              { static_cast<ITEM_ID>(NECommon::eCookies::CookieInvalid) };
    /**
     * \brief   NEService::COOKIE_LOCAL
     *          The indication of local service.
     **/
    constexpr ITEM_ID   COOKIE_LOCAL                { static_cast<ITEM_ID>(NECommon::eCookies::CookieLocal) };
    /**
     * \brief   NEService::COOKIE_ROUTER
     *          Indicates message router cookie
     **/
    constexpr ITEM_ID   COOKIE_ROUTER               { static_cast<ITEM_ID>(NECommon::eCookies::CookieRouter) };
    /**
     * \brief   NEService::COOKIE_LOGGER
     *          Indicates log collector cookie
     **/
    constexpr ITEM_ID   COOKIE_LOGGER               { static_cast<ITEM_ID>(NECommon::eCookies::CookieLogger) };
    /**
     * \brief   NEService::COOKIE_ANY
     *          Indicates any valid cookie
     **/
    constexpr ITEM_ID   COOKIE_ANY                  { static_cast<ITEM_ID>(NECommon::eCookies::CookieAny) };
    /**
     * \brief   NEService::TARGET_UNKNOWN
     *          The unknown target ID
     **/
    constexpr ITEM_ID   TARGET_UNKNOWN              { static_cast<ITEM_ID>(NECommon::eCookies::CookieInvalid) };
    /**
     * \brief   NEService::TARGET_LOCAL
     *          The local target ID
     **/
    constexpr ITEM_ID   TARGET_LOCAL                { static_cast<ITEM_ID>(NECommon::eCookies::CookieLocal) };
    /**
     * \brief   NEService::TARGET_ALL
     *          The undefined (all) target ID
     **/
    constexpr ITEM_ID   TARGET_ALL                  { static_cast<ITEM_ID>(NECommon::eCookies::CookieInvalid) };
    /**
     * \brief   NEService::SOURCE_UNKNOWN
     *          The unknown source ID
     **/
    constexpr ITEM_ID   SOURCE_UNKNOWN              { static_cast<ITEM_ID>(NECommon::eCookies::CookieInvalid) };
    /**
     * \brief   NEService::SOURCE_UNKNOWN
     *          The unknown source ID.
     **/
    constexpr ITEM_ID   SOURCE_LOCAL                { static_cast<ITEM_ID>(NECommon::eCookies::CookieLocal) };
    /**
     * \brief   NEService::COOKIE_REMOTE_SERVICE
     *          The ID of first valid remote cookie.
     **/
    constexpr ITEM_ID   COOKIE_REMOTE_SERVICE       { static_cast<ITEM_ID>(NECommon::eCookies::CookieFirstRemote) };

    /**
     * \brief   NEService::eServiceCalls
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
     * \brief   NEService::REQUEST_ID_FIRST
     *          The first ID in request call.
     **/
    constexpr unsigned int  REQUEST_ID_FIRST    { static_cast<uint32_t>(eServiceCalls::ServiceCallRequest) };
    /**
     * \brief   NEService::REQUEST_ID_LAST
     *          The last ID in request call.
     **/
    constexpr unsigned int  REQUEST_ID_LAST     { REQUEST_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   NEService::RESPONSE_ID_FIRST
     *          The first ID in response call.
     **/
    constexpr unsigned int  RESPONSE_ID_FIRST   { static_cast<uint32_t>(eServiceCalls::ServiceCallResponse) };
    /**
     * \brief   NEService::RESPONSE_ID_LAST
     *          The last ID in response call.
     **/
    constexpr unsigned int  RESPONSE_ID_LAST    { RESPONSE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   NEService::ATTRIBUTE_ID_FIRST
     *          The first ID in attribute call.
     **/
    constexpr unsigned int  ATTRIBUTE_ID_FIRST  { static_cast<uint32_t>(eServiceCalls::ServiceCallAttribute) };
    /**
     * \brief   NEService::ATTRIBUTE_ID_LAST
     *          The last ID in attribute call.
     **/
    constexpr unsigned int  ATTRIBUTE_ID_LAST   { ATTRIBUTE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   NEService::SERVICE_ID_FIRST
     *          The last ID in service call.
     **/
    constexpr unsigned int  SERVICE_ID_FIRST    { static_cast<uint32_t>(NEService::eServiceCalls::ServiceCallRegistery) };
    /**
     * \brief   NEService::SERVICE_ID_LAST
     *          The last ID in service call.
     **/
    constexpr unsigned int  SERVICE_ID_LAST     { SERVICE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   NEService::RESPONSE_ID_NONE
     *          Constant no response. Used to indicate that the request has no response.
     **/
    constexpr unsigned int  RESPONSE_ID_NONE    { static_cast<uint32_t>(eServiceCalls::ServiceCallNoFunction) };

    /**
     * \brief   NEService::INVALID_MESSAGE_ID
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
     * \brief   Returns string value of NEService::eFuncIdRange type
     **/
    inline const char * getString( NEService::eFuncIdRange funcId );

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
    using StateArrayBase    = TEFixedArray<NEService::eDataStateType>;
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
        ~StateArray( void );

    //////////////////////////////////////////////////////////////////////////
    // Operators, attributes and operations
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Returns element by the index for reading. The index must be valid.
         **/
        inline const NEService::eDataStateType& operator [] (unsigned int index) const;
        /**
         * \brief   Returns element by the index for reading. The index must be valid.
         **/
        inline NEService::eDataStateType& operator [] (unsigned int index);

        /**
         * \brief   Returns the number of elements in the array.
         */
        inline uint32_t getSize(void) const;
        /**
         * \brief   Resets states in array. All states will be set to NEService::DataIsUnavailable
         **/
        inline void resetStates( void );

        /**
         * \brief   Returns true if array has parameters, i.e. the size is
         *          not zero.
         **/
        inline bool hasParams( void ) const;

        /**
         * \brief   Sets the state of certain entry in array.
         * \param   whichIndex  The index of entry in array which state should be changed
         * \param   newState    The state to set
         **/
        inline void setState(int whichIndex, NEService::eDataStateType newState);

        /**
         * \brief   All entries are set to the same given state
         * \param   newState    The state to set for all entries
         **/
        inline void setAllState(NEService::eDataStateType newState);

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
        StateArray(void) = delete;
        DECLARE_NOCOPY_NOMOVE(StateArray);
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
         *          ('request ID' - NEService::eFuncIdRange::RequestFirstId)
         *          Every request should have appropriate response value. If request does not
         *          have response, it should have value NEService::RESPONSE_ID_NONE
         *          The size of this map should be equal to idRequestCount.
         **/
        const unsigned int* idRequestToResponseMap{ nullptr };

        /**
         * \brief   Map of parameter count in every response. Every response index
         *          is calculated by formula ('response ID' - NEService::eFuncIdRange::ResponseFirstId)
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
    AREG_API NEService::SInterfaceData & getEmptyInterface( void );

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
     *          param1State = states[GET_RESP_INDEX(UPD_ID_responseAREG)][0];
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
        ParameterArray(const NEService::SInterfaceData& ifData);


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
        ~ParameterArray( void );

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
         *          ('response ID' - NEService::eFuncIdRange::ResponseFirstId) or use
         *          GET_RESP_INDEX() macro
         **/
        inline NEService::StateArray & operator [] (unsigned int index);
        /**
         * \brief   Access read-only state by given index of array.
         *          The index is calculated by formula
         *          ('response ID' - NEService::eFuncIdRange::ResponseFirstId) or use
         *          GET_RESP_INDEX() macro
         **/
        inline const NEService::StateArray & operator [] (unsigned int index) const;

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Returns the state of parameter of certain response entry
         *          and certain parameter. The parameter entry is calculated
         *          row = GET_RESP_INDEX('response ID').
         * \param   row     The index of response call. Should be GET_RESP_INDEX('response ID')
         * \param   col     The index of parameter entry in response call. 
         *                  First parameter has index zero
         * \return  The state of parameter in call.
         **/
        inline NEService::eDataStateType getAt(unsigned int row, unsigned int col) const;

        /**
         * \brief   Sets state of parameter of certain response.
         * \param   row         The index of response entry, which can be calculated
         *                      by macro GET_RESP_INDEX('response ID')
         * \param   col         The index of parameter in response call. 
         *                      First parameter has index zero
         * \param   newValue    The state to set for parameter
         **/
        inline void setAt(unsigned int row, unsigned int col, NEService::eDataStateType newValue);

        /**
         * \brief   Returns true if specified response index has parameters.
         *          The response index is calculated by macro GET_RESP_INDEX('response ID')
         **/
        inline bool hasParameters(unsigned int whichRespIndex) const;
        /**
         * \brief   Returns true if valid response index. For example,
         *          the response ID RESPONSE_ID_NONE does not have valid
         *          response index. The index of response is calculated
         *          by macro GET_RESP_INDEX('response ID')
         * \param   whichRespIndex  The index of response, should be calculated by
         *                          macro GET_RESP_INDEX('response ID')
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
        inline void resetAllStates( void );

        /**
         * \brief   Sets all parameter states of specified entry.
         *          The index should be calculated by GET_RESP_INDEX('response ID')
         * \param   whichParam  The index of entry, should be calculated by GET_RESP_INDEX('response ID')
         * \param   newState    The state to set
         **/
        inline void setParamState(unsigned int whichParam, NEService::eDataStateType newState);

        /**
         * \brief   Resets all states of parameter of specified entry index.
         *          The index should be calculated by GET_RESP_INDEX('response ID')
         * \param   whichParam  The index of entry, should be calculated by GET_RESP_INDEX('response ID')
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
        NEService::StateArray **    mParamList;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        ParameterArray( void ) = delete;
        DECLARE_NOCOPY( ParameterArray );
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
        ProxyData(const NEService::SInterfaceData & ifData);
        /**
         * \brief   Destructor
         **/
        ~ProxyData( void ) = default;

    public:

        /**
         * \brief   Resets states of Proxy Data
         **/
        void resetStates( void );

        /**
         * \brief	Set state for specified request ID (range in NEService::eFuncIdRange)
         *          First, it will figure our whether it is attribute or response request ID.
         *          Then it will convert to index value and set appropriate entry state.
         *          If the request ID is NEService::ATTRIBUTE_SI_VERSION, it will only set
         *          state of component implementation version.
         * \param	msgId	    The function ID in range NEService::eFuncIdRange.
         *                      Can be either attribute or response range ID.
         *                      Otherwise it is ignored.
         * \param	newState	New state to set. If requested ID is response range,
         *                      it will set state for all parameters of that response, 
         *                      if any exist.
         **/
        void setDataState(unsigned int msgId, NEService::eDataStateType newState);

        /**
         * \brief   Returns data state of given message (attribute or response) ID.
         **/
        NEService::eDataStateType getDataState(unsigned int msgId) const;

        /**
         * \brief   Returns the response message ID of given request message ID.
         *          If message is wrong and/or is not in request to response map,
         *          it will return NEService::INVALID_MESSAGE_ID
         **/
        unsigned int getResponseId(unsigned int requestId) const;

        /**
         * \brief   Returns data state of given attribute ID
         **/
        inline NEService::eDataStateType getAttributeState(unsigned int msgId) const;

        /**
         * \brief   Returns parameter state of given response ID
         **/
        inline NEService::eDataStateType getParamState(unsigned int msgId) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Implementation version
         **/
        NEService::eDataStateType           mImplVersion;   // implementation version

        /**
         * \brief   Service Interface data
         **/
        const NEService::SInterfaceData &   mIfData;        // fixed service interface data

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
        ProxyData( void ) = delete;
        DECLARE_NOCOPY_NOMOVE( ProxyData );
    };


    /**
     * \brief   NEService::sServiceConnectedInstance
     *          Service connected instance of application.
     **/
    struct sServiceConnectedInstance
    {
        //!< The type of the application
        NEService::eMessageSource   ciSource    { NEService::eMessageSource::MessageSourceUndefined };
        //!< The bit-set of connected instance
        NEService::eInstanceBitness ciBitness   { NEService::eInstanceBitness::BitnessUnknown };
        //!< The cookie of the connected instance.
        ITEM_ID                     ciCookie    { NEService::COOKIE_UNKNOWN };
        //!< The connection timestamp
        TIME64                      ciTimestamp { 0 };
        //!< The name of the application
        String                      ciInstance  { "" };
        //!< The optional file location
        String                      ciLocation  { "" };
    };

    /**
     * \brief   The map of key-value connected instances, where the key is an instance ID and the value is connected instance information.
     **/
    using MapInstances = TEMap<ITEM_ID, NEService::sServiceConnectedInstance>;
}

//////////////////////////////////////////////////////////////////////////
// Global namespace NEService inline function implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_STREAMABLE(NEService::eResultType)
IMPLEMENT_STREAMABLE(NEService::eDataStateType)
IMPLEMENT_STREAMABLE(NEService::eRequestType)
IMPLEMENT_STREAMABLE(NEService::eMessageDataType)
IMPLEMENT_STREAMABLE(NEService::eServiceConnection)
IMPLEMENT_STREAMABLE(NEService::eDisconnectReason)
IMPLEMENT_STREAMABLE(NEService::eServiceRequestType)
IMPLEMENT_STREAMABLE(NEService::eServiceType)
IMPLEMENT_STREAMABLE(NEService::eInstanceBitness)
IMPLEMENT_STREAMABLE(NEService::eMessageSource)
IMPLEMENT_STREAMABLE(NEService::eFuncIdRange)

//////////////////////////////////////////////////////////////////////////
// namespace NEService inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool NEService::isServiceConnected(NEService::eServiceConnection connectionStatus)
{
    return (connectionStatus == NEService::eServiceConnection::ServiceConnected);
}

inline bool NEService::isServiceConnectionPending( NEService::eServiceConnection connectionStatus )
{
    return (connectionStatus == NEService::eServiceConnection::ServicePending);
}

inline bool NEService::isServiceRejected( NEService::eServiceConnection connectionStatus )
{
    return (connectionStatus == NEService::eServiceConnection::ServiceRejected);
}

inline bool NEService::isServiceConnectionLost( NEService::eServiceConnection connectionStatus )
{
    return (connectionStatus == NEService::eServiceConnection::ServiceConnectionLost);
}

inline bool NEService::isServiceDisconnected( NEService::eServiceConnection connectionStatus )
{
    return (connectionStatus != NEService::eServiceConnection::ServiceConnected);
}

inline NEService::eServiceConnection NEService::serviceConnection( NEService::eDisconnectReason reason )
{
    switch ( reason )
    {
    case NEService::eDisconnectReason::ReasonUndefined:
        return NEService::eServiceConnection::ServiceConnectionUnknown;
    
    case NEService::eDisconnectReason::ReasonServiceDisconnected:
    case NEService::eDisconnectReason::ReasonServiceLost:
    case NEService::eDisconnectReason::ReasonProviderLost:
    case NEService::eDisconnectReason::ReasonConsumerLost:
    case NEService::eDisconnectReason::ReasonClientConnectionLost:
        return NEService::eServiceConnection::ServiceConnectionLost;

    case NEService::eDisconnectReason::ReasonServiceRejected:
    case NEService::eDisconnectReason::ReasonProviderRejected:
    case NEService::eDisconnectReason::ReasonConsumerNotSupported:
        return NEService::eServiceConnection::ServiceRejected;

    case NEService::eDisconnectReason::ReasonConsumerDisconnected:
    case NEService::eDisconnectReason::ReasonProviderDisconnected:
    case NEService::eDisconnectReason::ReasonClientConnectionClosed:
        return NEService::eServiceConnection::ServiceDisconnected;

    case NEService::eDisconnectReason::ReasonSystemShutdown:
        return NEService::eServiceConnection::ServiceShutdown;

    default:
        return NEService::eServiceConnection::ServiceConnected;
    }
}

inline bool NEService::isRequestId(unsigned int msgId)
{
    return ((msgId & static_cast<unsigned int>(NEService::eServiceCalls::ServiceCallRequest)) != 0);
}

inline bool NEService::isResponseId(unsigned int msgId)
{
    return ((msgId & static_cast<unsigned int>(NEService::eServiceCalls::ServiceCallResponse)) != 0);
}

inline bool NEService::isAttributeId(unsigned int msgId)
{
    return ((msgId & static_cast<unsigned int>(NEService::eServiceCalls::ServiceCallAttribute)) != 0);
}

inline bool NEService::isServiceRegistryId( unsigned int msgId )
{
    return ((msgId & static_cast<unsigned int>(NEService::eServiceCalls::ServiceCallRegistery)) != 0);
}

inline bool NEService::isEmptyFunctionId(unsigned int msgId)
{
    return (msgId == static_cast<unsigned int>(NEService::eFuncIdRange::EmptyFunctionId));
}

inline bool NEService::isVersionNotifyId( unsigned int msgId )
{
    return (msgId == static_cast<unsigned int>(NEService::eFuncIdRange::ResponseServiceProviderVersion));
}

inline bool NEService::isConnectNotifyId( unsigned int msgId )
{
    return (msgId == static_cast<unsigned int>(NEService::eFuncIdRange::ResponseServiceProviderConnection));
}

inline bool NEService::isExecutableId(unsigned int msgId)
{
    return ( (msgId  & static_cast<unsigned int>(NEService::eServiceCalls::ServiceCallRequest)     ) != 0 ||
             (msgId  & static_cast<unsigned int>(NEService::eServiceCalls::ServiceCallResponse)    ) != 0 ||
             (msgId  & static_cast<unsigned int>(NEService::eServiceCalls::ServiceCallAttribute)   ) != 0 ||
             (msgId == static_cast<unsigned int>(NEService::eServiceCalls::ServiceCallNoFunction)  ) );
}


inline NEService::eMessageDataType NEService::getMessageDataType( unsigned int msgId )
{
    if ( NEService::isRequestId(msgId) )
        return NEService::eMessageDataType::RequestDataType;
    else if (NEService::isResponseId(msgId))
        return NEService::eMessageDataType::ResponseDataType;
    else if (NEService::isAttributeId(msgId))
        return NEService::eMessageDataType::AttributeDataType;
    else if (NEService::isServiceRegistryId(msgId))
        return NEService::eMessageDataType::ServiceDataType;
    else
        return NEService::eMessageDataType::UndefinedDataType;
}

//////////////////////////////////////////////////////////////////////////
// class NEService::StateArray inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const NEService::eDataStateType& NEService::StateArray::operator [] (unsigned int index) const
{
    return StateArrayBase::operator[](index);
}

inline NEService::eDataStateType& NEService::StateArray::operator [] (unsigned int index)
{
    return StateArrayBase::operator[](index);
}

inline uint32_t NEService::StateArray::getSize(void) const
{
    return StateArrayBase::getSize();
}

inline void NEService::StateArray::resetStates( void )
{
    setAllState(NEService::eDataStateType::DataIsUnavailable);
}

inline bool NEService::StateArray::hasParams( void ) const
{
    return (isEmpty() == false);
}

inline void NEService::StateArray::setState(int whichIndex, NEService::eDataStateType newState)
{
    mValueList[whichIndex] = newState;
}

inline void NEService::StateArray::setAllState(NEService::eDataStateType newState)
{
    for ( uint32_t i = 0; i < this->mElemCount; ++ i )
        mValueList[i] = newState;
}

//////////////////////////////////////////////////////////////////////////
// class NEService::ParameterArray inline function implementation
//////////////////////////////////////////////////////////////////////////
inline NEService::StateArray& NEService::ParameterArray::operator [] ( unsigned int index )
{
    ASSERT(isValidParamIndex(index));
    return *(mParamList[index]);
}

inline const NEService::StateArray& NEService::ParameterArray::operator [] ( unsigned int index ) const
{
    ASSERT(isValidParamIndex(index));
    return *(mParamList[index]);
}

inline NEService::eDataStateType NEService::ParameterArray::getAt( unsigned int row, unsigned int col ) const
{
    ASSERT(isValidParamIndex(row) && mParamList[row]->isValidIndex(static_cast<uint32_t>(col)));
    return mParamList[row]->getAt(static_cast<uint32_t>(col));
}

inline void NEService::ParameterArray::setAt( unsigned int row, unsigned int col, NEService::eDataStateType newValue )
{
    ASSERT(isValidParamIndex(row) && mParamList[row]->isValidIndex(static_cast<uint32_t>(col)));
    mParamList[row]->setAt(static_cast<uint32_t>(col), newValue);
}

inline bool NEService::ParameterArray::hasParameters( unsigned int whichRespIndex ) const
{
    ASSERT(isValidParamIndex(whichRespIndex));
    return mParamList[whichRespIndex]->hasParams();
}

inline void NEService::ParameterArray::resetAllStates( void )
{
    for (int col = 0; col < mElemCount; ++col)
    {
        mParamList[col]->resetStates();
    }
}

inline bool NEService::ParameterArray::isValidParamIndex(unsigned int whichRespIndex) const
{
    return ((static_cast<int>(whichRespIndex) >= 0) && (static_cast<int>(whichRespIndex) < mElemCount));
}

inline void NEService::ParameterArray::setParamState(unsigned int whichParam, NEService::eDataStateType newState)
{
    ASSERT(isValidParamIndex(whichParam));
    mParamList[whichParam]->setAllState(newState);
}

//////////////////////////////////////////////////////////////////////////
// class NEService::ProxyData inline function implementation
//////////////////////////////////////////////////////////////////////////

inline NEService::eDataStateType NEService::ProxyData::getAttributeState( unsigned int msgId ) const
{
    return NEService::isVersionNotifyId(msgId) ? mImplVersion : mAttrState[GET_ATTR_INDEX(msgId)];
}

inline NEService::eDataStateType NEService::ProxyData::getParamState( unsigned int msgId ) const
{
    const NEService::StateArray& param = mParamState[GET_RESP_INDEX(msgId)];
    return (param.hasParams() ? param[0u] : NEService::eDataStateType::DataIsUnavailable);
}

//////////////////////////////////////////////////////////////////////////
// class NEService::sServiceConnectedInstance serialization
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Serializes the instance structure to the stream.
 * \param   stream  The streaming object to serialize.
 * \param   output  The single structure of instance to serialize.
 **/
inline IEOutStream& operator << (IEOutStream& stream, const NEService::sServiceConnectedInstance & output)
{
    stream << output.ciSource << output.ciBitness << output.ciCookie << output.ciTimestamp << output.ciInstance << output.ciLocation;
    return stream;
}

/**
 * \brief   De-serializes the instance structure from the stream.
 * \param   stream  The streaming object that contains the information of the connected instance.
 * \param   input  The single structure of instance to initialize.
 **/
inline const IEInStream& operator >> (const IEInStream& stream, NEService::sServiceConnectedInstance & input)
{
    stream >> input.ciSource >> input.ciBitness >> input.ciCookie >> input.ciTimestamp >> input.ciInstance >> input.ciLocation;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// class NEService enumerations string conversion
//////////////////////////////////////////////////////////////////////////

inline const char* NEService::getString(NEService::eResultType resultType)
{
    switch (resultType)
    {
    case    NEService::eResultType::Undefined:
        return "NEService::eResultType::Undefined";

    case    NEService::eResultType::Error:
        return "NEService::eResultType::Error";
    case NEService::eResultType::Undelivered:
        return "NEService::eResultType::Undelivered";
    case    NEService::eResultType::NotProcessed:
        return "NEService::eResultType::NotProcessed";
    case NEService::eResultType::Processed:
        return "NEService::eResultType::Processed";

    case NEService::eResultType::MessageUndelivered:
        return "NEService::eResultType::MessageUndelivered";

    case    NEService::eResultType::RequestOK:
        return "NEService::eResultType::RequestOK";
    case    NEService::eResultType::RequestInvalid:
        return "NEService::eResultType::RequestInvalid";
    case    NEService::eResultType::RequestError:
        return "NEService::eResultType::RequestError";
    case    NEService::eResultType::RequestBusy:
        return "NEService::eResultType::RequestBusy";
    case    NEService::eResultType::RequestCanceled:
        return "NEService::eResultType::RequestCanceled";

    case    NEService::eResultType::DataOK:
        return "NEService::eResultType::DataOK";
    case    NEService::eResultType::DataInvalid:
        return "NEService::eResultType::DataInvalid";

    case    NEService::eResultType::ServiceOK:
        return "NEService::eResultType::ServiceOK";
    case    NEService::eResultType::ServiceUnavailable:
        return "NEService::eResultType::ServiceUnavailable";
    case    NEService::eResultType::ServiceInvalid:
        return "NEService::eResultType::ServiceInvalid";
    case    NEService::eResultType::ServiceRejected:
        return "NEService::eResultType::ServiceRejected";

    default:
        ASSERT(false);
        return "ERR: Undefined NEService::eResultType value!";
    }
}

inline const char* NEService::getString(NEService::eDataStateType dataState)
{
    switch (dataState)
    {
    case    NEService::eDataStateType::DataIsUndefined:
        return "NEService::eDataStateType::DataIsUndefined";

    case    NEService::eDataStateType::DataIsOK:
        return "NEService::eDataStateType::DataIsOK";

    case    NEService::eDataStateType::DataIsInvalid:
        return "NEService::eDataStateType::DataIsInvalid";

    case    NEService::eDataStateType::DataIsUnavailable:
        return "NEService::eDataStateType::DataIsUnavailable";

    case    NEService::eDataStateType::DataUnexpectedError:
        return "NEService::eDataStateType::DataUnexpectedError";

    default:
        ASSERT(false);
        return "ERR: Undefined NEService::eDataStateType value!";
    }
}

inline const char* NEService::getString( NEService::eRequestType resultType )
{
    switch (resultType)
    {
    case    NEService::eRequestType::Unprocessed:
        return "NEService::eRequestType::Unprocessed";
    case    NEService::eRequestType::StartNotify:
        return "NEService::eRequestType::StartNotify";
    case    NEService::eRequestType::StopNotify:
        return "NEService::eRequestType::StopNotify";        
    case    NEService::eRequestType::RemoveAllNotify:
        return "NEService::eRequestType::RemoveAllNotify";
    case    NEService::eRequestType::CallFunction:
        return "NEService::eRequestType::CallFunction";
    case    NEService::eRequestType::ServiceConnection:
        return "NEService::eRequestType::ServiceConnection";
    case    NEService::eRequestType::ClientConnection:
        return "NEService::eRequestType::ClientConnection";
    case    NEService::eRequestType::LoadComponent:
        return "NEService::eRequestType::LoadComponent";
    default:
        ASSERT(false);
        return "ERR: Undefined NEService::eRequestType value!";
    }
}

inline const char* NEService::getString( NEService::eMessageDataType dataType )
{
    switch (dataType)
    {
    case NEService::eMessageDataType::UndefinedDataType:
        return "NEService::eMessageDataType::UndefinedDataType";
    case NEService::eMessageDataType::RequestDataType:
        return "NEService::eMessageDataType::RequestDataType";
    case NEService::eMessageDataType::ResponseDataType:
        return "NEService::eMessageDataType::ResponseDataType";
    case NEService::eMessageDataType::AttributeDataType:
        return "NEService::eMessageDataType::AttributeDataType";
    case NEService::eMessageDataType::ServiceDataType:
        return "NEService::eMessageDataType::ServiceDataType";

    default:
        ASSERT(false);
        return "ERR: Undefined NEService::eMessageDataType value!";
    }
}

inline const char* NEService::getString( NEService::eServiceConnection serviceConnection )
{
    switch (serviceConnection)
    {
    case NEService::eServiceConnection::ServiceConnectionUnknown:
        return "NEService::eServiceConnection::ServiceConnectionUnknown";
    case NEService::eServiceConnection::ServiceConnected:
        return "NEService::eServiceConnection::ServiceConnected";
    case NEService::eServiceConnection::ServicePending:
        return "NEService::eServiceConnection::ServicePending";
    case NEService::eServiceConnection::ServiceDisconnected:
        return "NEService::eServiceConnection::ServiceDisconnected";
    case NEService::eServiceConnection::ServiceConnectionLost:
        return "NEService::eServiceConnection::ServiceConnectionLost";
    case NEService::eServiceConnection::ServiceRejected:
        return "NEService::eServiceConnection::ServiceRejected";
    case NEService::eServiceConnection::ServiceFailed:
        return "NEService::eServiceConnection::ServiceFailed";
    case NEService::eServiceConnection::ServiceShutdown:
        return "NEService::eServiceConnection::ServiceShutdown";
    default:
        ASSERT(false);
        return "ERR: Undefined NEService::eServiceConnection value!";
    }
}

inline const char * NEService::getString( NEService::eDisconnectReason reason )
{
    switch ( reason )
    {
    case NEService::eDisconnectReason::ReasonUndefined:
        return "NEService::eDisconnectReason::ReasonUndefined";
    case NEService::eDisconnectReason::ReasonServiceDisconnected:
        return "NEService::eDisconnectReason::ReasonServiceDisconnected";
    case NEService::eDisconnectReason::ReasonServiceLost:
        return "NEService::eDisconnectReason::ReasonServiceLost";
    case NEService::eDisconnectReason::ReasonServiceRejected:
        return "NEService::eDisconnectReason::ReasonServiceRejected";
    case NEService::eDisconnectReason::ReasonProviderDisconnected:
        return "NEService::eDisconnectReason::ReasonProviderDisconnected";
    case NEService::eDisconnectReason::ReasonProviderLost:
        return "NEService::eDisconnectReason::ReasonProviderLost";
    case NEService::eDisconnectReason::ReasonProviderRejected:
        return "NEService::eDisconnectReason::ReasonProviderRejected";
    case NEService::eDisconnectReason::ReasonConsumerDisconnected:
        return "NEService::eDisconnectReason::ReasonConsumerDisconnected";
    case NEService::eDisconnectReason::ReasonConsumerLost:
        return "NEService::eDisconnectReason::ReasonConsumerLost";
    case NEService::eDisconnectReason::ReasonConsumerNotSupported:
        return "NEService::eDisconnectReason::ReasonConsumerNotSupported";
    case NEService::eDisconnectReason::ReasonSystemShutdown:
        return "NEService::eDisconnectReason::ReasonSystemShutdown";
    case NEService::eDisconnectReason::ReasonClientConnectionLost:
        return "NEService::eDisconnectReason::ReasonClientConnectionLost";
    case NEService::eDisconnectReason::ReasonClientConnectionClosed:
        return "NEService::eDisconnectReason::ReasonClientConnectionClosed";
    default:
        ASSERT( false );
        return "ERR: Undefined NEService::eDisconnectReason value!";
    }
}

inline const char * NEService::getString( NEService::eServiceRequestType svcRequestType )
{
    switch ( svcRequestType )
    {
    case NEService::eServiceRequestType::RegisterClient:
        return "NEService::eServiceRequestType::RegisterClient";
    case NEService::eServiceRequestType::UnregisterClient:
        return "NEService::eServiceRequestType::UnregisterClient";
    case NEService::eServiceRequestType::RegisterStub:
        return "NEService::RegisterStub";
    case NEService::eServiceRequestType::UnregisterStub:
        return "NEService::eServiceRequestType::UnregisterStub";
    default:
        return "ERR: Unexpected NEService::eServiceRequestType value!!!";
    }
}

inline const char * NEService::getString( NEService::eServiceType srvcType )
{
    switch ( srvcType )
    {
    case NEService::eServiceType::ServiceLocal:
        return "NEService::eServiceType::ServiceLocal";
    case NEService::eServiceType::ServicePublic:
        return "NEService::eServiceType::ServicePublic";
    case NEService::eServiceType::ServiceAny:
        return "NEService::eServiceType::ServiceAny";
    case NEService::eServiceType::ServiceInvalid:
        return "NEService::eServiceType::ServiceInvalid";
    default:
        return "ERR: Unexpected NEService::eServiceRequestType value!!!";
    }
}

const char* NEService::getString(NEService::eInstanceBitness bitness)
{
    switch (bitness)
    {
    case NEService::eInstanceBitness::Bitness32:
        return "NEService::eInstanceBitness::Bitness32";
    case NEService::eInstanceBitness::Bitness64:
        return "NEService::eInstanceBitness::Bitness64";
    case NEService::eInstanceBitness::BitnessUnknown:
        return "NEService::eInstanceBitness::BitnessUnknown";
    default:
        return "ERR: Unexpected NEService::eInstanceBitness value!!!";
    }
}


const char * NEService::getString(NEService::eMessageSource msgSource)
{
    switch (msgSource)
    {
    case NEService::eMessageSource::MessageSourceUndefined:
        return "NEService::eMessageSource::MessageSourceUndefined";
    case NEService::eMessageSource::MessageSourceClient:
        return "NEService::eMessageSource::MessageSourceClient";
    case NEService::eMessageSource::MessageSourceService:
        return "NEService::eMessageSource::MessageSourceService";
    case NEService::eMessageSource::MessageSourceObserver:
        return "NEService::eMessageSource::MessageSourceObserver";
    case NEService::eMessageSource::MessageSourceTest:
        return "NEService::eMessageSource::MessageSourceTest";
    case NEService::eMessageSource::MessageSourceSimulation:
        return "NEService::eMessageSource::MessageSourceSimulation";
    default:
        return "ERR: Unexpected NEService::eMessageSource value!!!";
    }
}


inline const char * NEService::getString( NEService::eFuncIdRange funcId )
{
    switch ( funcId )
    {
    case NEService::eFuncIdRange::EmptyFunctionId:
        return "NEService::eFuncIdRange::EmptyFunctionId";
    case NEService::eFuncIdRange::ComponentCleanup:
        return "NEService::eFuncIdRange::ComponentCleanup";
    case NEService::eFuncIdRange::RequestRegisterService:
        return "NEService::eFuncIdRange::RequestRegisterService";
    case NEService::eFuncIdRange::RequestServiceProviderVersion:
        return "NEService::eFuncIdRange::RequestServiceProviderVersion";
    case NEService::eFuncIdRange::ResponseServiceProviderVersion:
        return "NEService::eFuncIdRange::ResponseServiceProviderVersion";
    case NEService::eFuncIdRange::RequestServiceProviderConnection:
        return "NEService::eFuncIdRange::RequestServiceProviderConnection";
    case NEService::eFuncIdRange::ResponseServiceProviderConnection:
        return "NEService::eFuncIdRange::ResponseServiceProviderConnection";
    case NEService::eFuncIdRange::SystemServiceConnect:
        return "NEService::eFuncIdRange::SystemServiceConnect";
    case NEService::eFuncIdRange::SystemServiceDisconnect:
        return "NEService::eFuncIdRange::SystemServiceDisconnect";
    case NEService::eFuncIdRange::SystemServiceNotifyConnection:
        return "NEService::eFuncIdRange::SystemServiceNotifyConnection";
    case NEService::eFuncIdRange::SystemServiceQueryInstances:
        return "NEService::eFuncIdRange::SystemServiceQueryInstances";
    case NEService::eFuncIdRange::SystemServiceNotifyInstances:
        return "NEService::eFuncIdRange::SystemServiceNotifyInstances";
    case NEService::eFuncIdRange::SystemServiceRequestRegister:
        return "NEService::eFuncIdRange::SystemServiceRequestRegister";
    case NEService::eFuncIdRange::SystemServiceNotifyRegister:
        return "NEService::eFuncIdRange::SystemServiceNotifyRegister";
    case NEService::eFuncIdRange::ServiceLogRegisterScopes:
        return "NEService::eFuncIdRange::ServiceLogRegisterScopes";
    case NEService::eFuncIdRange::ServiceLogUpdateScopes:
        return "NEService::eFuncIdRange::ServiceLogUpdateScopes";
    case NEService::eFuncIdRange::ServiceLogQueryScopes:
        return "NEService::eFuncIdRange::ServiceLogQueryScopes";
    case NEService::eFuncIdRange::ServiceLogScopesUpdated:
        return "NEService::eFuncIdRange::ServiceLogScopesUpdated";
    case NEService::eFuncIdRange::ServiceSaveLogConfiguration:
        return "NEService::eFuncIdRange::ServiceSaveLogConfiguration";
    case NEService::eFuncIdRange::ServiceLogConfigurationSaved:
        return "NEService::eFuncIdRange::ServiceLogConfigurationSaved";
    case NEService::eFuncIdRange::ServiceLogMessage:
        return "NEService::eFuncIdRange::ServiceLogMessage";
    case NEService::eFuncIdRange::RequestFirstId:
        return "NEService::eFuncIdRange::RequestFirstId";
    case NEService::eFuncIdRange::ResponseFirstId:
        return "NEService::eFuncIdRange::ResponseFirstId";
    case NEService::eFuncIdRange::AttributeFirstId:
        return "NEService::eFuncIdRange::AttributeFirstId";
    case NEService::eFuncIdRange::RequestLastId:
        return "NEService::eFuncIdRange::RequestLastId";
    case NEService::eFuncIdRange::ResponseLastId:
        return "NEService::eFuncIdRange::ResponseLastId";
    case NEService::eFuncIdRange::AttributeLastId:
        return "NEService::eFuncIdRange::AttributeLastId";
    case NEService::eFuncIdRange::ServiceLastId:
        return "NEService::eFuncIdRange::ServiceLastId";

    default:
        {
            if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(NEService::eFuncIdRange::RequestFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(NEService::eFuncIdRange::RequestLastId)) )
                return "Request ID range";
            else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(NEService::eFuncIdRange::ResponseFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(NEService::eFuncIdRange::ResponseLastId)) )
                return "Response ID range";
            else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(NEService::eFuncIdRange::AttributeFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(NEService::eFuncIdRange::AttributeLastId)) )
                return "Attribute ID range";
            else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(NEService::eFuncIdRange::RequestRegisterService)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLastId)) )
                return "Service registration ID";
            else
                return "ERR: Unexpected ID";
        }
    }
}

#endif  // AREG_COMPONENT_NESERVICE_HPP
