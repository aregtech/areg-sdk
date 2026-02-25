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
     * \brief   Returns string value of ResultType type
     **/
    inline const char* getString(ResultType resultType);

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
     * \brief   Returns string value of DataState type
     **/
    inline const char* getString(DataState dataState);

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
     * \brief   Returns string value of RequestType type
     **/
    inline const char* getString( RequestType resultType );

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
     * \brief   Returns string value of MessageDataType type
     **/
    inline const char* getString( MessageDataType dataType );

    /**
     * \brief   From passed message ID finds data type
     **/
    inline MessageDataType getMessageDataType( uint32_t msgId );

    /**
     * \brief   ServiceConnectionState
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
     * \brief   Returns string value of ServiceConnectionState type
     **/
    inline const char* getString( ServiceConnectionState serviceConnection );

    /**
     * \brief   Returns true if the service connection status is connected.
     **/
    inline bool isServiceConnected( ServiceConnectionState connectionStatus );

    /**
     * \brief   Returns true if the service connection status is connected.
     **/
    inline bool isServiceConnectionPending( ServiceConnectionState connectionStatus );

    /**
     * \brief   Returns true if the service connection status is rejected.
     **/
    inline bool isServiceRejected( ServiceConnectionState connectionStatus );

    /**
     * \brief   Returns true if the service connection status is connection lost.
     **/
    inline bool isServiceConnectionLost( ServiceConnectionState connectionStatus );

    /**
     * \brief   Returns true if the status is not connected.
     **/
    inline bool isServiceDisconnected( ServiceConnectionState connectionStatus );

    /**
     * \brief   DisconnectReason
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

    inline const char * getString( DisconnectReason reason );

    inline ServiceConnectionState serviceConnection( DisconnectReason reason );

    /**
     * \brief   RegistrationAction
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
     * \brief   Returns string value of RegistrationAction type
     **/
    inline const char * getString( RegistrationAction svcRequestType );

    /**
     * \brief   ServiceType
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
     * \brief   Returns string value of ServiceType type
     **/
    inline const char * getString( ServiceType srvcType );

    /**
     * \brief   MessageSource
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
     * \brief   Returns string value of ServiceType type
     **/
    inline const char * getString(MessageSource msgSource);

    /**
     * \brief   InstanceBitness
     *          The bitness of the executable instance.
     **/
    enum class InstanceBitness  : uint16_t
    {
          BitnessUnknown    = 0     //!< Unknown bitness
        , Bitness32         = 32    //!< 32-bit system
        , Bitness64         = 64    //!< 64-bit system
    };

    /**
     * \brief   Returns string value of ServiceType type
     **/
    inline const char* getString(InstanceBitness bitness);

    /**
     * \brief   SEQUENCE_NUMBER_NOTIFY
     *          Sequence number predefining notification message ID
     **/
    constexpr SequenceNumber    SEQUENCE_NUMBER_NOTIFY  { static_cast<SequenceNumber>(0) };    /*0x00000000*/
    /**
     * \brief   SEQUENCE_NUMBER_ANY
     *          Any sequence number, used in messages. "Any sequence number" used to find any listener object with same message ID.
     **/
    constexpr SequenceNumber    SEQUENCE_NUMBER_ANY     { static_cast<SequenceNumber>(~0) };    /*0xFFFFFFFF*/

    /**
     * \brief   COOKIE_UNKNOWN
     *          Unknown cookie
     **/
    constexpr ITEM_ID   COOKIE_UNKNOWN              { static_cast<ITEM_ID>(Cookie::Invalid) };
    /**
     * \brief   COOKIE_LOCAL
     *          The indication of local service.
     **/
    constexpr ITEM_ID   COOKIE_LOCAL                { static_cast<ITEM_ID>(Cookie::Local) };
    /**
     * \brief   COOKIE_ROUTER
     *          Indicates message router cookie
     **/
    constexpr ITEM_ID   COOKIE_ROUTER               { static_cast<ITEM_ID>(Cookie::Router) };
    /**
     * \brief   COOKIE_LOGGER
     *          Indicates log collector cookie
     **/
    constexpr ITEM_ID   COOKIE_LOGGER               { static_cast<ITEM_ID>(Cookie::Logger) };
    /**
     * \brief   COOKIE_ANY
     *          Indicates any valid cookie
     **/
    constexpr ITEM_ID   COOKIE_ANY                  { static_cast<ITEM_ID>(Cookie::Any) };
    /**
     * \brief   TARGET_UNKNOWN
     *          The unknown target ID
     **/
    constexpr ITEM_ID   TARGET_UNKNOWN              { static_cast<ITEM_ID>(Cookie::Invalid) };
    /**
     * \brief   TARGET_LOCAL
     *          The local target ID
     **/
    constexpr ITEM_ID   TARGET_LOCAL                { static_cast<ITEM_ID>(Cookie::Local) };
    /**
     * \brief   TARGET_ALL
     *          The undefined (all) target ID
     **/
    constexpr ITEM_ID   TARGET_ALL                  { static_cast<ITEM_ID>(COOKIE_ANY) };
    /**
     * \brief   SOURCE_UNKNOWN
     *          The unknown source ID
     **/
    constexpr ITEM_ID   SOURCE_UNKNOWN              { static_cast<ITEM_ID>(Cookie::Invalid) };
    /**
     * \brief   SOURCE_UNKNOWN
     *          The unknown source ID.
     **/
    constexpr ITEM_ID   SOURCE_LOCAL                { static_cast<ITEM_ID>(Cookie::Local) };
    /**
     * \brief   COOKIE_REMOTE_SERVICE
     *          The ID of first valid remote cookie.
     **/
    constexpr ITEM_ID   COOKIE_REMOTE_SERVICE       { static_cast<ITEM_ID>(Cookie::FirstRemote) };

    /**
     * \brief   ServiceCallType
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
     * \brief   REQUEST_ID_FIRST
     *          The first ID in request call.
     **/
    constexpr uint32_t  REQUEST_ID_FIRST    { static_cast<uint32_t>(ServiceCallType::RequestFunction) };
    /**
     * \brief   REQUEST_ID_LAST
     *          The last ID in request call.
     **/
    constexpr uint32_t  REQUEST_ID_LAST     { REQUEST_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   RESPONSE_ID_FIRST
     *          The first ID in response call.
     **/
    constexpr uint32_t  RESPONSE_ID_FIRST   { static_cast<uint32_t>(ServiceCallType::ResponseFunction) };
    /**
     * \brief   RESPONSE_ID_LAST
     *          The last ID in response call.
     **/
    constexpr uint32_t  RESPONSE_ID_LAST    { RESPONSE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   ATTRIBUTE_ID_FIRST
     *          The first ID in attribute call.
     **/
    constexpr uint32_t  ATTRIBUTE_ID_FIRST  { static_cast<uint32_t>(ServiceCallType::AttributeUpdate) };
    /**
     * \brief   ATTRIBUTE_ID_LAST
     *          The last ID in attribute call.
     **/
    constexpr uint32_t  ATTRIBUTE_ID_LAST   { ATTRIBUTE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   SERVICE_ID_FIRST
     *          The last ID in service call.
     **/
    constexpr uint32_t  SERVICE_ID_FIRST    { static_cast<uint32_t>(ServiceCallType::ServiceRegisteration) };
    /**
     * \brief   SERVICE_ID_LAST
     *          The last ID in service call.
     **/
    constexpr uint32_t  SERVICE_ID_LAST     { SERVICE_ID_FIRST + FUNC_RANGE };

    /**
     * \brief   RESPONSE_ID_NONE
     *          Constant no response. Used to indicate that the request has no response.
     **/
    constexpr uint32_t  RESPONSE_ID_NONE    { static_cast<uint32_t>(ServiceCallType::NoFunction) };

    /**
     * \brief   INVALID_MESSAGE_ID
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
     * \brief   Returns string value of FuncIdRange type
     **/
    inline const char * getString( FuncIdRange funcId );

    /**
     * \brief   Converts request message ID to array index. Returns INVALID_INDEX if empty.
     **/
    inline constexpr uint32_t reqIndex(uint32_t msgId)
    {
        return (msgId != static_cast<uint32_t>(FuncIdRange::EmptyFunctionId))
                ? msgId - static_cast<uint32_t>(FuncIdRange::RequestFirstId)
                : static_cast<uint32_t>(INVALID_INDEX);
    }

    /**
     * \brief   Converts response message ID to array index. Returns INVALID_INDEX if empty.
     **/
    inline constexpr uint32_t respIndex(uint32_t msgId)
    {
        return (msgId != static_cast<uint32_t>(FuncIdRange::EmptyFunctionId))
                ? msgId - static_cast<uint32_t>(FuncIdRange::ResponseFirstId)
                : static_cast<uint32_t>(INVALID_INDEX);
    }

    /**
     * \brief   Converts attribute message ID to array index. Returns INVALID_INDEX if empty.
     **/
    inline constexpr uint32_t attrIndex(uint32_t msgId)
    {
        return (msgId != static_cast<uint32_t>(FuncIdRange::EmptyFunctionId))
                ? msgId - static_cast<uint32_t>(FuncIdRange::AttributeFirstId)
                : static_cast<uint32_t>(INVALID_INDEX);
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
    using StateArrayBase    = areg::FixedArray<DataState>;
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
        inline const DataState& operator [] (uint32_t index) const;
        /**
         * \brief   Returns element by the index for reading. The index must be valid.
         **/
        inline DataState& operator [] (uint32_t index);

        /**
         * \brief   Returns the number of elements in the array.
         */
        inline uint32_t getSize() const;
        /**
         * \brief   Resets states in array. All states will be set to DataIsUnavailable
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
        inline void setState(int32_t whichIndex, DataState newState);

        /**
         * \brief   All entries are set to the same given state
         * \param   newState    The state to set for all entries
         **/
        inline void setAllState(DataState newState);

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
        Version       idVersion{ 0, 0, 0 };

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
         *          ('request ID' - FuncIdRange::RequestFirstId)
         *          Every request should have appropriate response value. If request does not
         *          have response, it should have value RESPONSE_ID_NONE
         *          The size of this map should be equal to idRequestCount.
         **/
        const uint32_t* idRequestToResponseMap{ nullptr };

        /**
         * \brief   Map of parameter count in every response. Every response index
         *          is calculated by formula ('response ID' - FuncIdRange::ResponseFirstId)
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
     * \brief   Returns the instance of empty local service interface.
     **/
    AREG_API InterfaceData & getEmptyInterface();

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
     *          param1State = states[respIndex(UPD_ID_responseAREG)][0];
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
        ParameterArray(const InterfaceData& ifData);


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
         *          ('response ID' - FuncIdRange::ResponseFirstId) or use
         *          respIndex() macro
         **/
        inline StateArray & operator [] (uint32_t index);
        /**
         * \brief   Access read-only state by given index of array.
         *          The index is calculated by formula
         *          ('response ID' - FuncIdRange::ResponseFirstId) or use
         *          respIndex() macro
         **/
        inline const StateArray & operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Returns the state of parameter of certain response entry
         *          and certain parameter. The parameter entry is calculated
         *          row = respIndex('response ID').
         * \param   row     The index of response call. Should be respIndex('response ID')
         * \param   col     The index of parameter entry in response call. 
         *                  First parameter has index zero
         * \return  The state of parameter in call.
         **/
        inline DataState getAt(uint32_t row, uint32_t col) const;

        /**
         * \brief   Sets state of parameter of certain response.
         * \param   row         The index of response entry, which can be calculated
         *                      by macro respIndex('response ID')
         * \param   col         The index of parameter in response call. 
         *                      First parameter has index zero
         * \param   newValue    The state to set for parameter
         **/
        inline void setAt(uint32_t row, uint32_t col, DataState newValue);

        /**
         * \brief   Returns true if specified response index has parameters.
         *          The response index is calculated by macro respIndex('response ID')
         **/
        inline bool hasParameters(uint32_t whichRespIndex) const;
        /**
         * \brief   Returns true if valid response index. For example,
         *          the response ID RESPONSE_ID_NONE does not have valid
         *          response index. The index of response is calculated
         *          by macro respIndex('response ID')
         * \param   whichRespIndex  The index of response, should be calculated by
         *                          macro respIndex('response ID')
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
         *          The index should be calculated by respIndex('response ID')
         * \param   whichParam  The index of entry, should be calculated by respIndex('response ID')
         * \param   newState    The state to set
         **/
        inline void setParamState(uint32_t whichParam, DataState newState);

        /**
         * \brief   Resets all states of parameter of specified entry index.
         *          The index should be calculated by respIndex('response ID')
         * \param   whichParam  The index of entry, should be calculated by respIndex('response ID')
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
        StateArray **    mParamList;

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
    using ParamState    = ParameterArray;
    /**
     * \brief   Type of state array
     **/
    using AttrState     = StateArray;

    //////////////////////////////////////////////////////////////////////////
    // ProxyData class declaration
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
        ProxyData(const InterfaceData & ifData);
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
         * \brief	Set state for specified request ID (range in FuncIdRange)
         *          First, it will figure our whether it is attribute or response request ID.
         *          Then it will convert to index value and set appropriate entry state.
         *          If the request ID is ATTRIBUTE_SI_VERSION, it will only set
         *          state of component implementation version.
         * \param	msgId	    The function ID in range FuncIdRange.
         *                      Can be either attribute or response range ID.
         *                      Otherwise it is ignored.
         * \param	newState	New state to set. If requested ID is response range,
         *                      it will set state for all parameters of that response, 
         *                      if any exist.
         **/
        void setDataState(uint32_t msgId, DataState newState);

        /**
         * \brief   Returns data state of given message (attribute or response) ID.
         **/
        DataState getDataState(uint32_t msgId) const;

        /**
         * \brief   Returns the response message ID of given request message ID.
         *          If message is wrong and/or is not in request to response map,
         *          it will return INVALID_MESSAGE_ID
         **/
        uint32_t getResponseId(uint32_t requestId) const;

        /**
         * \brief   Returns data state of given attribute ID
         **/
        inline DataState getAttributeState(uint32_t msgId) const;

        /**
         * \brief   Returns parameter state of given response ID
         **/
        inline DataState getParamState(uint32_t msgId) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Implementation version
         **/
        DataState           mImplVersion;   // implementation version

        /**
         * \brief   Service Interface data
         **/
        const InterfaceData &   mIfData;        // fixed service interface data

        /**
         * \brief   Table of attribute states
         **/
        AttrState                mAttrState;     // state of attributes

        /**
         * \brief   Table of response parameter state
         **/
        ParamState               mParamState;    // state of parameters in response call.

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        ProxyData() = delete;
        AREG_NOCOPY_NOMOVE( ProxyData );
    };


    /**
     * \brief   ConnectedInstance
     *          Service connected instance of application.
     **/
    struct ConnectedInstance
    {
        //!< The type of the application
        MessageSource   ciSource    { MessageSource::SourceUndefined };
        //!< The bit-set of connected instance
        InstanceBitness ciBitness   { InstanceBitness::BitnessUnknown };
        //!< The cookie of the connected instance.
        ITEM_ID                     ciCookie    { COOKIE_UNKNOWN };
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
    using MapInstances = OrderedMap<ITEM_ID, ConnectedInstance>;

    //////////////////////////////////////////////////////////////////////////
    // Global namespace areg inline function implementation
    //////////////////////////////////////////////////////////////////////////
    AREG_IMPLEMENT_STREAMABLE(ResultType)
    AREG_IMPLEMENT_STREAMABLE(DataState)
    AREG_IMPLEMENT_STREAMABLE(RequestType)
    AREG_IMPLEMENT_STREAMABLE(MessageDataType)
    AREG_IMPLEMENT_STREAMABLE(ServiceConnectionState)
    AREG_IMPLEMENT_STREAMABLE(DisconnectReason)
    AREG_IMPLEMENT_STREAMABLE(RegistrationAction)
    AREG_IMPLEMENT_STREAMABLE(ServiceType)
    AREG_IMPLEMENT_STREAMABLE(InstanceBitness)
    AREG_IMPLEMENT_STREAMABLE(MessageSource)
    AREG_IMPLEMENT_STREAMABLE(FuncIdRange)

    //////////////////////////////////////////////////////////////////////////
    // namespace areg inline function implementation
    //////////////////////////////////////////////////////////////////////////

    inline bool isServiceConnected(ServiceConnectionState connectionStatus)
    {
        return (connectionStatus == ServiceConnectionState::Connected);
    }

    inline bool isServiceConnectionPending( ServiceConnectionState connectionStatus )
    {
        return (connectionStatus == ServiceConnectionState::Pending);
    }

    inline bool isServiceRejected( ServiceConnectionState connectionStatus )
    {
        return (connectionStatus == ServiceConnectionState::Rejected);
    }

    inline bool isServiceConnectionLost( ServiceConnectionState connectionStatus )
    {
        return (connectionStatus == ServiceConnectionState::ConnectionLost);
    }

    inline bool isServiceDisconnected( ServiceConnectionState connectionStatus )
    {
        return (connectionStatus != ServiceConnectionState::Connected);
    }

    inline ServiceConnectionState serviceConnection( DisconnectReason reason )
    {
        switch ( reason )
        {
        case DisconnectReason::UndefinedReason:
            return ServiceConnectionState::Unknown;
        
        case DisconnectReason::ServiceDisconnected:
        case DisconnectReason::ServiceLost:
        case DisconnectReason::ProviderLost:
        case DisconnectReason::ConsumerLost:
        case DisconnectReason::ClientConnectionLost:
            return ServiceConnectionState::ConnectionLost;

        case DisconnectReason::ServiceRejected:
        case DisconnectReason::ProviderRejected:
        case DisconnectReason::ConsumerNotSupported:
            return ServiceConnectionState::Rejected;

        case DisconnectReason::ConsumerDisconnected:
        case DisconnectReason::ProviderDisconnected:
        case DisconnectReason::ClientConnectionClosed:
            return ServiceConnectionState::Disconnected;

        case DisconnectReason::SystemShutdown:
            return ServiceConnectionState::Shutdown;

        default:
            return ServiceConnectionState::Connected;
        }
    }

    inline bool isRequestId(uint32_t msgId)
    {
        return ((msgId & static_cast<uint32_t>(ServiceCallType::RequestFunction)) != 0);
    }

    inline bool isResponseId(uint32_t msgId)
    {
        return ((msgId & static_cast<uint32_t>(ServiceCallType::ResponseFunction)) != 0);
    }

    inline bool isAttributeId(uint32_t msgId)
    {
        return ((msgId & static_cast<uint32_t>(ServiceCallType::AttributeUpdate)) != 0);
    }

    inline bool isServiceRegistryId( uint32_t msgId )
    {
        return ((msgId & static_cast<uint32_t>(ServiceCallType::ServiceRegisteration)) != 0);
    }

    inline bool isEmptyFunctionId(uint32_t msgId)
    {
        return (msgId == static_cast<uint32_t>(FuncIdRange::EmptyFunctionId));
    }

    inline bool isVersionNotifyId( uint32_t msgId )
    {
        return (msgId == static_cast<uint32_t>(FuncIdRange::ResponseServiceProviderVersion));
    }

    inline bool isConnectNotifyId( uint32_t msgId )
    {
        return (msgId == static_cast<uint32_t>(FuncIdRange::ResponseServiceProviderConnection));
    }

    inline bool isExecutableId(uint32_t msgId)
    {
        return ( (msgId  & static_cast<uint32_t>(ServiceCallType::RequestFunction)     ) != 0 ||
                (msgId  & static_cast<uint32_t>(ServiceCallType::ResponseFunction)    ) != 0 ||
                (msgId  & static_cast<uint32_t>(ServiceCallType::AttributeUpdate)   ) != 0 ||
                (msgId == static_cast<uint32_t>(ServiceCallType::NoFunction)  ) );
    }


    inline MessageDataType getMessageDataType( uint32_t msgId )
    {
        if ( isRequestId(msgId) )
            return MessageDataType::RequestData;
        else if (isResponseId(msgId))
            return MessageDataType::ResponseData;
        else if (isAttributeId(msgId))
            return MessageDataType::AttributeData;
        else if (isServiceRegistryId(msgId))
            return MessageDataType::ServiceData;
        else
            return MessageDataType::UndefinedData;
    }

    //////////////////////////////////////////////////////////////////////////
    // class StateArray inline function implementation
    //////////////////////////////////////////////////////////////////////////

    inline const DataState& StateArray::operator [] (uint32_t index) const
    {
        return StateArrayBase::operator[](index);
    }

    inline DataState& StateArray::operator [] (uint32_t index)
    {
        return StateArrayBase::operator[](index);
    }

    inline uint32_t StateArray::getSize() const
    {
        return StateArrayBase::getSize();
    }

    inline void StateArray::resetStates()
    {
        setAllState(DataState::DataIsUnavailable);
    }

    inline bool StateArray::hasParams() const
    {
        return (isEmpty() == false);
    }

    inline void StateArray::setState(int32_t whichIndex, DataState newState)
    {
        mValueList[whichIndex] = newState;
    }

    inline void StateArray::setAllState(DataState newState)
    {
        for ( uint32_t i = 0; i < this->mElemCount; ++ i )
            mValueList[i] = newState;
    }

    //////////////////////////////////////////////////////////////////////////
    // class ParameterArray inline function implementation
    //////////////////////////////////////////////////////////////////////////
    inline StateArray& ParameterArray::operator [] ( uint32_t index )
    {
        ASSERT(isValidParamIndex(index));
        return *(mParamList[index]);
    }

    inline const StateArray& ParameterArray::operator [] ( uint32_t index ) const
    {
        ASSERT(isValidParamIndex(index));
        return *(mParamList[index]);
    }

    inline DataState ParameterArray::getAt( uint32_t row, uint32_t col ) const
    {
        ASSERT(isValidParamIndex(row) && mParamList[row]->isValidIndex(static_cast<uint32_t>(col)));
        return mParamList[row]->getAt(static_cast<uint32_t>(col));
    }

    inline void ParameterArray::setAt( uint32_t row, uint32_t col, DataState newValue )
    {
        ASSERT(isValidParamIndex(row) && mParamList[row]->isValidIndex(static_cast<uint32_t>(col)));
        mParamList[row]->setAt(static_cast<uint32_t>(col), newValue);
    }

    inline bool ParameterArray::hasParameters( uint32_t whichRespIndex ) const
    {
        ASSERT(isValidParamIndex(whichRespIndex));
        return mParamList[whichRespIndex]->hasParams();
    }

    inline void ParameterArray::resetAllStates()
    {
        for (int col = 0; col < mElemCount; ++col)
        {
            mParamList[col]->resetStates();
        }
    }

    inline bool ParameterArray::isValidParamIndex(uint32_t whichRespIndex) const
    {
        return ((static_cast<int32_t>(whichRespIndex) >= 0) && (static_cast<int32_t>(whichRespIndex) < mElemCount));
    }

    inline void ParameterArray::setParamState(uint32_t whichParam, DataState newState)
    {
        ASSERT(isValidParamIndex(whichParam));
        mParamList[whichParam]->setAllState(newState);
    }

    //////////////////////////////////////////////////////////////////////////
    // class ProxyData inline function implementation
    //////////////////////////////////////////////////////////////////////////

    inline DataState ProxyData::getAttributeState( uint32_t msgId ) const
    {
        return isVersionNotifyId(msgId) ? mImplVersion : mAttrState[attrIndex(msgId)];
    }

    inline DataState ProxyData::getParamState( uint32_t msgId ) const
    {
        const StateArray& param = mParamState[respIndex(msgId)];
        return (param.hasParams() ? param[0u] : DataState::DataIsUnavailable);
    }

    //////////////////////////////////////////////////////////////////////////
    // class ConnectedInstance serialization
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Serializes the instance structure to the stream.
     * \param   stream  The streaming object to serialize.
     * \param   output  The single structure of instance to serialize.
     **/
    inline OutStream& operator << (OutStream& stream, const ConnectedInstance & output)
    {
        stream << output.ciSource << output.ciBitness << output.ciCookie << output.ciTimestamp << output.ciInstance << output.ciLocation;
        return stream;
    }

    /**
     * \brief   De-serializes the instance structure from the stream.
     * \param   stream  The streaming object that contains the information of the connected instance.
     * \param   input  The single structure of instance to initialize.
     **/
    inline const InStream& operator >> (const InStream& stream, ConnectedInstance & input)
    {
        stream >> input.ciSource >> input.ciBitness >> input.ciCookie >> input.ciTimestamp >> input.ciInstance >> input.ciLocation;
        return stream;
    }

    //////////////////////////////////////////////////////////////////////////
    // class NEService enumerations string conversion
    //////////////////////////////////////////////////////////////////////////

    inline const char* getString(ResultType resultType)
    {
        switch (resultType)
        {
        case    ResultType::Undefined:
            return "ResultType::Undefined";

        case    ResultType::Error:
            return "ResultType::Error";
        case ResultType::Undelivered:
            return "ResultType::Undelivered";
        case    ResultType::NotProcessed:
            return "ResultType::NotProcessed";
        case ResultType::Processed:
            return "ResultType::Processed";

        case ResultType::MessageUndelivered:
            return "ResultType::MessageUndelivered";

        case    ResultType::RequestOK:
            return "ResultType::RequestOK";
        case    ResultType::RequestInvalid:
            return "ResultType::RequestInvalid";
        case    ResultType::RequestError:
            return "ResultType::RequestError";
        case    ResultType::RequestBusy:
            return "ResultType::RequestBusy";
        case    ResultType::RequestCanceled:
            return "ResultType::RequestCanceled";

        case    ResultType::DataOK:
            return "ResultType::DataOK";
        case    ResultType::DataInvalid:
            return "ResultType::DataInvalid";

        case    ResultType::ServiceOK:
            return "ResultType::ServiceOK";
        case    ResultType::ServiceUnavailable:
            return "ResultType::ServiceUnavailable";
        case    ResultType::Invalid:
            return "ResultType::Invalid";
        case    ResultType::ServiceRejected:
            return "ResultType::ServiceRejected";

        default:
            ASSERT(false);
            return "ERR: Undefined ResultType value!";
        }
    }

    inline const char* getString(DataState dataState)
    {
        switch (dataState)
        {
        case    DataState::DataIsUndefined:
            return "DataState::DataIsUndefined";

        case    DataState::DataIsOK:
            return "DataState::DataIsOK";

        case    DataState::DataIsInvalid:
            return "DataState::DataIsInvalid";

        case    DataState::DataIsUnavailable:
            return "DataState::DataIsUnavailable";

        case    DataState::DataUnexpectedError:
            return "DataState::DataUnexpectedError";

        default:
            ASSERT(false);
            return "ERR: Undefined DataState value!";
        }
    }

    inline const char* getString( RequestType resultType )
    {
        switch (resultType)
        {
        case    RequestType::Unprocessed:
            return "RequestType::Unprocessed";
        case    RequestType::StartNotify:
            return "RequestType::StartNotify";
        case    RequestType::StopNotify:
            return "RequestType::StopNotify";        
        case    RequestType::RemoveAllNotify:
            return "RequestType::RemoveAllNotify";
        case    RequestType::CallFunction:
            return "RequestType::CallFunction";
        case    RequestType::ServiceConnection:
            return "RequestType::ServiceConnection";
        case    RequestType::ClientConnection:
            return "RequestType::ClientConnection";
        case    RequestType::LoadComponent:
            return "RequestType::LoadComponent";
        default:
            ASSERT(false);
            return "ERR: Undefined RequestType value!";
        }
    }

    inline const char* getString( MessageDataType dataType )
    {
        switch (dataType)
        {
        case MessageDataType::UndefinedData:
            return "MessageDataType::UndefinedData";
        case MessageDataType::RequestData:
            return "MessageDataType::RequestData";
        case MessageDataType::ResponseData:
            return "MessageDataType::ResponseData";
        case MessageDataType::AttributeData:
            return "MessageDataType::AttributeData";
        case MessageDataType::ServiceData:
            return "MessageDataType::ServiceData";

        default:
            ASSERT(false);
            return "ERR: Undefined MessageDataType value!";
        }
    }

    inline const char* getString( ServiceConnectionState serviceConnection )
    {
        switch (serviceConnection)
        {
        case ServiceConnectionState::Unknown:
            return "ServiceConnectionState::Unknown";
        case ServiceConnectionState::Connected:
            return "ServiceConnectionState::Connected";
        case ServiceConnectionState::Pending:
            return "ServiceConnectionState::Pending";
        case ServiceConnectionState::Disconnected:
            return "ServiceConnectionState::Disconnected";
        case ServiceConnectionState::ConnectionLost:
            return "ServiceConnectionState::ConnectionLost";
        case ServiceConnectionState::Rejected:
            return "ServiceConnectionState::Rejected";
        case ServiceConnectionState::Failed:
            return "ServiceConnectionState::Failed";
        case ServiceConnectionState::Shutdown:
            return "ServiceConnectionState::Shutdown";
        default:
            ASSERT(false);
            return "ERR: Undefined ServiceConnectionState value!";
        }
    }

    inline const char * getString( DisconnectReason reason )
    {
        switch ( reason )
        {
        case DisconnectReason::UndefinedReason:
            return "DisconnectReason::UndefinedReason";
        case DisconnectReason::ServiceDisconnected:
            return "DisconnectReason::ServiceDisconnected";
        case DisconnectReason::ServiceLost:
            return "DisconnectReason::ServiceLost";
        case DisconnectReason::ServiceRejected:
            return "DisconnectReason::ServiceRejected";
        case DisconnectReason::ProviderDisconnected:
            return "DisconnectReason::ProviderDisconnected";
        case DisconnectReason::ProviderLost:
            return "DisconnectReason::ProviderLost";
        case DisconnectReason::ProviderRejected:
            return "DisconnectReason::ProviderRejected";
        case DisconnectReason::ConsumerDisconnected:
            return "DisconnectReason::ConsumerDisconnected";
        case DisconnectReason::ConsumerLost:
            return "DisconnectReason::ConsumerLost";
        case DisconnectReason::ConsumerNotSupported:
            return "DisconnectReason::ConsumerNotSupported";
        case DisconnectReason::SystemShutdown:
            return "DisconnectReason::SystemShutdown";
        case DisconnectReason::ClientConnectionLost:
            return "DisconnectReason::ClientConnectionLost";
        case DisconnectReason::ClientConnectionClosed:
            return "DisconnectReason::ClientConnectionClosed";
        default:
            ASSERT( false );
            return "ERR: Undefined DisconnectReason value!";
        }
    }

    inline const char * getString( RegistrationAction svcRequestType )
    {
        switch ( svcRequestType )
        {
        case RegistrationAction::RegisterClient:
            return "RegistrationAction::RegisterClient";
        case RegistrationAction::UnregisterClient:
            return "RegistrationAction::UnregisterClient";
        case RegistrationAction::RegisterStub:
            return "RegisterStub";
        case RegistrationAction::UnregisterStub:
            return "RegistrationAction::UnregisterStub";
        default:
            return "ERR: Unexpected RegistrationAction value!!!";
        }
    }

    inline const char * getString( ServiceType srvcType )
    {
        switch ( srvcType )
        {
        case ServiceType::Local:
            return "ServiceType::Local";
        case ServiceType::Public:
            return "ServiceType::Public";
        case ServiceType::Any:
            return "ServiceType::Any";
        case ServiceType::Invalid:
            return "ServiceType::Invalid";
        default:
            return "ERR: Unexpected RegistrationAction value!!!";
        }
    }

    const char* getString(InstanceBitness bitness)
    {
        switch (bitness)
        {
        case InstanceBitness::Bitness32:
            return "InstanceBitness::Bitness32";
        case InstanceBitness::Bitness64:
            return "InstanceBitness::Bitness64";
        case InstanceBitness::BitnessUnknown:
            return "InstanceBitness::BitnessUnknown";
        default:
            return "ERR: Unexpected InstanceBitness value!!!";
        }
    }


    const char * getString(MessageSource msgSource)
    {
        switch (msgSource)
        {
        case MessageSource::SourceUndefined:
            return "MessageSource::SourceUndefined";
        case MessageSource::SourceClient:
            return "MessageSource::SourceClient";
        case MessageSource::SourceService:
            return "MessageSource::SourceService";
        case MessageSource::SourceObserver:
            return "MessageSource::SourceObserver";
        case MessageSource::SourceTest:
            return "MessageSource::SourceTest";
        case MessageSource::SourceSimulation:
            return "MessageSource::SourceSimulation";
        default:
            return "ERR: Unexpected MessageSource value!!!";
        }
    }


    inline const char * getString( FuncIdRange funcId )
    {
        switch ( funcId )
        {
        case FuncIdRange::EmptyFunctionId:
            return "FuncIdRange::EmptyFunctionId";
        case FuncIdRange::ComponentCleanup:
            return "FuncIdRange::ComponentCleanup";
        case FuncIdRange::RequestRegisterService:
            return "FuncIdRange::RequestRegisterService";
        case FuncIdRange::RequestServiceProviderVersion:
            return "FuncIdRange::RequestServiceProviderVersion";
        case FuncIdRange::ResponseServiceProviderVersion:
            return "FuncIdRange::ResponseServiceProviderVersion";
        case FuncIdRange::RequestServiceProviderConnection:
            return "FuncIdRange::RequestServiceProviderConnection";
        case FuncIdRange::ResponseServiceProviderConnection:
            return "FuncIdRange::ResponseServiceProviderConnection";
        case FuncIdRange::SystemServiceConnect:
            return "FuncIdRange::SystemServiceConnect";
        case FuncIdRange::SystemServiceDisconnect:
            return "FuncIdRange::SystemServiceDisconnect";
        case FuncIdRange::SystemServiceNotifyConnection:
            return "FuncIdRange::SystemServiceNotifyConnection";
        case FuncIdRange::SystemServiceQueryInstances:
            return "FuncIdRange::SystemServiceQueryInstances";
        case FuncIdRange::SystemServiceNotifyInstances:
            return "FuncIdRange::SystemServiceNotifyInstances";
        case FuncIdRange::SystemServiceRequestRegister:
            return "FuncIdRange::SystemServiceRequestRegister";
        case FuncIdRange::SystemServiceNotifyRegister:
            return "FuncIdRange::SystemServiceNotifyRegister";
        case FuncIdRange::ServiceLogRegisterScopes:
            return "FuncIdRange::ServiceLogRegisterScopes";
        case FuncIdRange::ServiceLogUpdateScopes:
            return "FuncIdRange::ServiceLogUpdateScopes";
        case FuncIdRange::ServiceLogQueryScopes:
            return "FuncIdRange::ServiceLogQueryScopes";
        case FuncIdRange::ServiceLogScopesUpdated:
            return "FuncIdRange::ServiceLogScopesUpdated";
        case FuncIdRange::ServiceSaveLogConfiguration:
            return "FuncIdRange::ServiceSaveLogConfiguration";
        case FuncIdRange::ServiceLogConfigurationSaved:
            return "FuncIdRange::ServiceLogConfigurationSaved";
        case FuncIdRange::ServiceLogMessage:
            return "FuncIdRange::ServiceLogMessage";
        case FuncIdRange::RequestFirstId:
            return "FuncIdRange::RequestFirstId";
        case FuncIdRange::ResponseFirstId:
            return "FuncIdRange::ResponseFirstId";
        case FuncIdRange::AttributeFirstId:
            return "FuncIdRange::AttributeFirstId";
        case FuncIdRange::RequestLastId:
            return "FuncIdRange::RequestLastId";
        case FuncIdRange::ResponseLastId:
            return "FuncIdRange::ResponseLastId";
        case FuncIdRange::AttributeLastId:
            return "FuncIdRange::AttributeLastId";
        case FuncIdRange::ServiceLastId:
            return "FuncIdRange::ServiceLastId";

        default:
            {
                if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(FuncIdRange::RequestFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(FuncIdRange::RequestLastId)) )
                    return "Request ID range";
                else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(FuncIdRange::ResponseFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(FuncIdRange::ResponseLastId)) )
                    return "Response ID range";
                else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(FuncIdRange::AttributeFirstId)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(FuncIdRange::AttributeLastId)) )
                    return "Attribute ID range";
                else if ( (static_cast<uint32_t>(funcId) > static_cast<uint32_t>(FuncIdRange::RequestRegisterService)) && (static_cast<uint32_t>(funcId) < static_cast<uint32_t>(FuncIdRange::ServiceLastId)) )
                    return "Service registration ID";
                else
                    return "ERR: Unexpected ID";
            }
        }
    }

} // namespace areg
#endif  // AREG_COMPONENT_SERVICEDEFS_HPP
