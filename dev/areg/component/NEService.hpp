#ifndef AREG_COMPONENT_NESERVICE_HPP
#define AREG_COMPONENT_NESERVICE_HPP
/************************************************************************
 * \file        areg/component/NEService.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
#include "areg/base/TEFixedArray.hpp"
#include "areg/base/Version.hpp"
#include "areg/base/String.hpp"

/************************************************************************
 * predefined macro
 ************************************************************************/

/**
 * \brief   Converts request message id to index
 **/
#define GET_REQ_INDEX(msgId)           (static_cast<unsigned int>(msgId) != static_cast<unsigned int>(NEService:: EMPTY_FUNCTION_ID) ? static_cast<int>(static_cast<unsigned int>(msgId) - static_cast<unsigned int>(NEService::REQUEST_ID_FIRST)) : -1)

/**
 * \brief   Converts response message id to index
 **/
#define GET_RESP_INDEX(msgId)          (static_cast<unsigned int>(msgId) != static_cast<unsigned int>(NEService:: EMPTY_FUNCTION_ID) ? static_cast<int>(static_cast<unsigned int>(msgId) - static_cast<unsigned int>(NEService::RESPONSE_ID_FIRST)) : -1)

/**
 * \brief   Converts attribute message id to index
 **/
#define GET_ATTR_INDEX(msgId)          (static_cast<unsigned int>(msgId) != static_cast<unsigned int>(NEService:: EMPTY_FUNCTION_ID) ? static_cast<int>(static_cast<unsigned int>(msgId) - static_cast<unsigned int>(NEService::ATTRIBUTE_ID_FIRST)) : -1)


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
    typedef enum E_ResultType
    {
        /* not used */
          RESULT_UNDEFINED          =     0 /*0x0000*/      //!< undefined result. not used               Bits: 0000 0000 0000 0000
        /* additional bits */
        , RESULT_ERROR              =     1 /*0x0001*/      //!< indicate error. contains error bit       Bits: 0000 0000 0000 0001
        , RESULT_UNDELIVERED        =    32 /*0x0020*/      //!< message was not delivered                Bits: 0000 0000 0010 0000
        , RESULT_NOT_PROCESSED      =    64 /*0x0040*/      //!< call did not reach target                Bits: 0000 0000 0100 0000
        , RESULT_PROCESSED          =   128 /*0x0080*/      //!< call reached target                      Bits: 0000 0000 1000 0000

         /* Message did not reach target, used in remote messaging */
        , RESULT_MESSAGE_UNDELIVERED=  4129 /*0x1021*/      //!< request failed to reach target.          Bits: 0001 0000 0010 0001

        /* request calls result */
        , RESULT_OK                 =  8320 /*0x2080*/      //!< indicates success of request call.       Bits: 0010 0000 1000 0000
        , RESULT_INVALID            =  8257 /*0x2041*/      //!< indicates failure of request call.       Bits: 0010 0000 0100 0001
        , RESULT_REQUEST_ERROR      =  8321 /*0x2081*/      //!< indicates request execution failure.     Bits: 0010 0000 1000 0001
        , RESULT_REQUEST_BUSY       =  8323 /*0x2083*/      //!< request cannot execute, it is busy.      Bits: 0010 0000 1000 0011
        , RESULT_REQUEST_CANCELED   =  8325 /*0x2085*/      //!< request is canceled and not executed.    Bits: 0010 0000 1000 0101

        /* data update result */
        , RESULT_DATA_OK            = 16512 /*0x4080*/      //!< indicates data validation.               Bits: 0100 0000 1000 0000
        , RESULT_DATA_INVALID       = 16449 /*0x4041*/      //!< indicates data invalid.                  Bits: 0100 0000 0100 0001

        /* service call result */
        , RESULT_SERVICE_OK         = 32896 /*0x8080*/      //!< service call processed.                  Bits: 1000 0000 1000 0000
        , RESULT_SERVICE_UNAVAILABLE= 32833 /*0x8041*/      //!< service is unavailable.                  Bits: 1000 0000 0100 0001
        , RESULT_SERVICE_INVALID    = 32897 /*0x8081*/      //!< service invalid (check cookie).          Bits: 1000 0000 1000 0001
        , RESULT_SERVICE_REJECTED   = 32899 /*0x8083*/      //!< service rejected (unsupported).          Bits: 1000 0000 1000 0011

    } eResultType;
    /**
     * \brief   Returns string value of NEService::eResultType type
     **/
    inline const char* getString(NEService::eResultType resultType);

    /**
     * \brief   Data types
     *          Used getting data
     **/
    typedef enum E_DataStateType
    {
          DATA_UNDEFINED        =     0 /*0x0000*/    //!< undefined type, not used
        , DATA_OK               = 16384 /*0x4000*/    //!< data valid
        , DATA_INVALID          = 16385 /*0x4001*/    //!< data is invalid
        , DATA_UNAVAILABLE      = 16387 /*0x4003*/    //!< data is unavailable, no such data
        , DATA_UNEXPECTED_ERROR = 16389 /*0x4005*/    //!< other errors

    } eDataStateType;
    /**
     * \brief   Returns string value of NEService::eDataStateType type
     **/
    inline const char* getString(NEService::eDataStateType dataState);

    /**
     * \brief   Type of request.
     *          Used sending request event by proxy
     **/
    typedef enum E_RequestType
    {
          REQUEST_UNPROCESSED           =     0 /*0x0000*/    //!< request is unprocessed
        , REQUEST_START_NOTIFY          =  8193 /*0x2001*/    //!< request start notify on attribute update
        , REQUEST_STOP_NOTIFY           =  8194 /*0x2002*/    //!< request stop notify on attribute update
        , REQUEST_REMOVE_ALL_NOTIFY     =  8198 /*0x2006*/    //!< request remove all notifications
        , REQUEST_CALL                  = 16385 /*0x4001*/    //!< request function call
        , REQUEST_CONNECTION            = 16387 /*0x4003*/    //!< request connection status update
        , REQUEST_CLIENT_CONNECTION     = 16389 /*0x4005*/    //!< request client connection status update
        , REQUEST_LOAD_COMPONENT        = 16391 /*0x4007*/    //!< request load component
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
    typedef enum E_MessageDataType
    {
          UNDEFINED_DATA_TYPE       =     0 /*0x0000*/  //!< data type is undefined
        , REQUEST_DATA_TYPE         =  4096 /*0x1000*/  //!< data type of request event
        , RESPONSE_DATA_TYPE        =  8192 /*0x2000*/  //!< data type response event
        , ATTRIBUTE_DATA_TYPE       = 16384 /*0x3000*/  //!< data type of attribute event
        , SERVICE_DATA_TYPE         = 32768 /*0x8000*/  //!< data type service call

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
    typedef enum E_ServiceConnection
    {
          ServiceConnectionUnknown  =    0  /*0x0000*/  //!< Connection is unknown.               Bit set: 0000 0000
        , ServiceConnected          =    1  /*0x0001*/  //!< Service Connected, ready to serve.   Bit set: 0000 0001
        , ServicePending            =    3  /*0x0003*/  //!< Service Connection is pending.       Bit set: 0000 0011
        , ServiceDisconnected       =   16  /*0x0010*/  //!< Service disconnected.                Bit set: 0001 0000
        , ServiceRejected           =   48  /*0x0030*/  //!< Service Connection rejected.         Bit set: 0011 0000
        , ServiceFailed             =   80  /*0x0050*/  //!< Service Connection failed.           Bit set: 0101 0000
        , ServiceShutdown           =  144  /*0x0090*/  //!< Service shut down, no connection.    Bit set: 1001 0000
    } eServiceConnection;
    /**
     * \brief   Returns string value of NEService::eDataType type
     **/
    inline const char* getString( NEService::eServiceConnection serviceConnection );

    typedef enum E_ServiceRequestType
    {
          SERVICE_REQUEST_REGISTER_CLIENT       = 0x10  //!< Client requests to register for service.     Bit set:    0001 0000
        , SERVICE_REQUEST_UNREGISTER_CLIENT     = 0x11  //!< Client requests to unregister for service.   Bit set:    0001 0001
        , SERVICE_REQUEST_REGISTER_STUB         = 0x20  //!< Server requests to register for service.     Bit set:    0010 0000
        , SERVICE_REQUEST_UNREGISTER_STUB       = 0x21  //!< Server requests to unregister for service.   Bit set:    0010 0001
    } eServiceRequestType;
    
    /**
     * \brief   Returns string value of NEService::eServiceRequestType type
     **/
    inline const char * getString( NEService::eServiceRequestType svcRequestType );

    /**
     * \brief   NEService::eServiceType
     *          Service Type. Either local or Remote.
     **/
    typedef enum E_ServiceType
    {
          ServiceInvalid    = 0x00  //!< Invalid Service      Bit set: 0000 0000
        , ServiceLocal      = 0x40  //!< Local Service.       Bit set: 0100 0000
        , ServiceRemote     = 0x80  //!< External Service.    Bit set: 1000 0000
        , ServiceAny        = 0xC0  //!< Any service.         Bit set: 1100 0000
    } eServiceType;

    /**
     * \brief   Returns string value of NEService::eServiceType type
     **/
    inline const char * getString( NEService::eServiceType srvcType );

    /**
     * \brief   NEService::SEQUENCE_NUMBER_NOTIFY
     *          Sequence number predefining notification message ID
     **/
    const unsigned int  SEQUENCE_NUMBER_NOTIFY  = static_cast<unsigned int>(0);    /*0x00000000*/
    /**
     * \brief   NEService::SEQUENCE_NUMBER_ANY
     *          Any sequence number, used in messages. "Any sequence number" used to find any listener object with same message ID.
     **/
    const unsigned int  SEQUENCE_NUMBER_ANY     = static_cast<unsigned int>(~0);    /*0xFFFFFFFF*/

    /**
     * \brief   NEService::eCookie
     *          The list of reserved cookie values
     **/
    typedef enum E_Cookies
    {
          CookieInvalid     = 0     //!< Invalid cookie value
        , CookieLocal               //!< Valid cookie value of local services
        , CookieRouter              //!< Valid cookie value of Routing Service
        , CookieFirstValid  = 256   //!< First valid cookie of any other remote service
    } eCookies;

    /**
     * \brief   NEService::COOKIE_UNKNOWN
     *          Unknown cookie
     **/
    const ITEM_ID   COOKIE_UNKNOWN      = NEService::CookieInvalid;
    /**
     * \brief   NEService::COOKIE_LOCAL
     *          The indication of local service.
     **/
    const ITEM_ID   COOKIE_LOCAL        = NEService::CookieLocal;
    /**
     * \brief   NEService::COOKIE_ROUTER
     *          Indicates Router cookie
     **/
    const ITEM_ID   COOKIE_ROUTER       = NEService::CookieRouter;
    /**
     * \brief   NEService::TARGET_UNKNOWN
     *          The unknown target ID
     **/
    const ITEM_ID   TARGET_UNKNOWN      = NEService::CookieInvalid;
    /**
     * \brief   NEService::TARGET_LOCAL
     *          The local target ID
     **/
    const ITEM_ID   TARGET_LOCAL        = NEService::CookieLocal;
    /**
     * \brief   NEService::SOUR_UNKNOWN
     *          The unknown source ID
     **/
    const ITEM_ID   SOUR_UNKNOWN      = NEService::CookieInvalid;
    /**
     * \brief   NEService::SOUR_UNKNOWN
     *          The unknown source ID
     **/
    const ITEM_ID   SOUR_LOCAL        = NEService::CookieLocal;

    /**
     * \brief   NEService::eServiceCalls
     *          Specifies the service call type
     **/
    typedef enum E_ServiceCall
    {
          ServiceCallNoFunction     = 0x0000    //!< No function call
        , ServiceCallRequest        = 0x1000    //!< Call of service request function
        , ServiceCallResponse       = 0x2000    //!< Call of service response function
        , ServiceCallAttribute      = 0x4000    //!< Call of service attribute update function
        , ServiceCallRegister       = 0x8000    //!< Call of service registration
    } eServiceCalls;

    /**
     * \brief   Predefined range of function message IDs
     **/
    const unsigned int  FUNC_RANGE              = static_cast<unsigned int>(4095);  /*0x0FFF*/

    /**
     * \brief   The first ID of valid service interface function call
     **/
    const unsigned int  SERVICE_FUNCTION        = static_cast<unsigned int>(NEService::ServiceCallRequest);    /*0x1000*/

    /**
     * \brief   Predefined range of function calls
     **/
    typedef enum E_FuncIdRange
    {
        /**
         * \brief   Empty function ID
         **/        
          EMPTY_FUNCTION_ID     = static_cast<unsigned int>(NEService::ServiceCallNoFunction)   /* 0x00000000 */    //!< No function
        , COMPONENT_MAKE_CLEANUP= 0x00000100                                                                        //!< A call to remove pointer in a component thread context.

        /**
         * \brief   Request call range. Should start from REQUEST_ID_FIRST
         **/
        , REQUEST_ID_FIRST      = static_cast<unsigned int>(NEService::ServiceCallRequest)      /* 0x00001000 */    //!< Request call first ID
        , REQUEST_ID_LAST       = static_cast<unsigned int>(REQUEST_ID_FIRST)   + NEService::FUNC_RANGE             //!< Request call last ID

        /**
         * \brief   Response call range. Should start from RESPONSE_ID_FIRST
         **/
        , RESPONSE_ID_FIRST     = static_cast<unsigned int>(NEService::ServiceCallResponse)     /* 0x00002000 */    //!< Response call first ID
        , RESPONSE_ID_LAST      = static_cast<unsigned int>(RESPONSE_ID_FIRST)  + NEService::FUNC_RANGE             //!< Response call last ID
        
        /**
         * \brief   Attribute request call. Should start from ATTRIBUTE_ID_FIRST
         **/
        , ATTRIBUTE_ID_FIRST    = static_cast<unsigned int>(NEService::ServiceCallAttribute)    /* 0x00004000 */    //!< Attribute update notification call first ID
        , ATTRIBUTE_ID_LAST     = static_cast<unsigned int>(ATTRIBUTE_ID_FIRST) + NEService::FUNC_RANGE             //!< Attribute update notification call last ID

        , SI_REGISTER_ID_FIRST  = static_cast<unsigned int>(NEService::ServiceCallRegister)     /* 0x00008000 */    //!< Service registration first ID

        /**
         * \brief   Sent by client to Stub to get supported version information
         **/
        , SI_REQUEST_VERSION                                                                                        //!< Service Interface request version
        /**
         * \brief   Sent by Stub to notify clients implementation version
         **/
        , SI_NOTIFY_VERSION                                                                                         //!< Service Interface notify version
        /**
         * \brief   Sent by client or stub when requesting service connection (eServiceRequestType)
         **/
        , SI_SERVICE_CONNECTION_REQUEST                                                                             //!< Service connect request
        /**
         * \brief   Sent by service to targets to notify connection status update
         **/
        , SI_SERVICE_CONNECTION_NOTIFY                                                                              //!< Service connection notify request

        /**
         * \brief   Called to connect to Routing Service
         **/
        , SI_ROUTER_CONNECT                                                                                         //!< Routing Service connect
        /**
         * \brief   Called to disconnect Routing Service
         **/
        , SI_ROUTER_DISCONNECT                                                                                      //!< Routing Service disconnect
        /**
         * \brief   Sent by Routing Service to notify connection status
         **/
        , SI_ROUTER_NOTIFY                                                                                          //!< Routing Service notification
        /**
         * \brief   Called to register new service at Routing Service
         **/
        , SI_ROUTER_REGISTER                                                                                        //!< Routing Service registration
        /**
         * \brief   Called to query service registration at Routing Service
         **/
        , SI_ROUTER_QUERY                                                                                           //!< Routing Service query
        /**
         * \brief   Sent by Routing Service to notify the queried service registration availability
         **/
        , SI_ROUTER_REGISTER_NOTIFY                                                                                 //!< Routing Service registration notify

        , SI_REGISTER_ID_LAST   = static_cast<unsigned int>(SI_REGISTER_ID_FIRST)  + NEService::FUNC_RANGE          //!< Servicing call last ID

    } eFuncIdRange;

    /**
     * \brief   Returns string value of NEService::eFuncIdRange type
     **/
    inline const char * getString( NEService::eFuncIdRange funcId );

    /**
     * \brief   Constant no response. Should be used in Request to Response map for request
     *           calls, which have no response
     **/
    const unsigned int  RESPONSE_ID_NONE    = static_cast<unsigned int>(NEService::EMPTY_FUNCTION_ID);

    /**
     * \brief   NEService::INVALID_MESSAGE_ID
     *          The invalid message ID
     **/
    const unsigned int INVALID_MESSAGE_ID   = static_cast<unsigned int>(-1);    /*0xFFFFFFFF*/

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
    inline bool isRegistrationId( unsigned int msgId );

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
    typedef TEListImpl<NEService::eDataStateType>   ImplStateArray;
    /**
     * \brief   StateArray class. Keeps data state information
     **/
    class AREG_API StateArray : public TEFixedArray<NEService::eDataStateType, NEService::eDataStateType, ImplStateArray>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets initial size of fixed array. The size of array cannot be changed dynamically.
         **/
        StateArray(int size = 0);

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        StateArray(const StateArray & src);

        /**
         * \brief   Destructor
         **/
        ~StateArray( void );

    //////////////////////////////////////////////////////////////////////////
    // Attributes and operations
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Resets states in array. All states will be set to NEService::DATA_UNAVAILABLE
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
    // Hidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
        friend class ParameterArray;

        /**
         * \brief   Initialization constructor.
         *          Does not make hard-copy, only assigns given
         *          parameters
         **/
        StateArray(unsigned char* thisBffer, int elemCount);
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
         * \brief   The name of service (interface)
         **/
        const char*         idServiceName;              // service (interface) name

        /**
         * \brief   Implementation version
         **/
        Version           idVersion;                  // interface version

        /**
         * \brief   The service type. Either local or remote
         **/
        eServiceType       idServiceType;

        /**
         * \brief   Number of requests in service interface
         **/
        unsigned int        idRequestCount;             // number of requests

        /**
         * \brief   Number of responses in service interface
         **/
        unsigned int        idResponseCount;            // number of responses

        /**
         * \brief   Number of attributes in service interface
         **/
        unsigned int        idAttributeCount;           // number of attributes

        /**
         * \brief   The list of requests. It is NULL if the list is empty.
         **/
        const unsigned int* idRequestList;

        /**
         * \brief   The list of responses. It is NULL if the list is empty.
         **/
        const unsigned int* idResponseList;

        /**
         * \brief   The list of attributes. It is NULL if the list is empty.
         **/
        const unsigned int* idAttributeList;

        /**
         * \brief   Request to Response map. All requests are accessed by index 
         *          and every request indexed is calculated by calculating 
         *          ('request ID' - NEService::REQUEST_ID_FIRST)
         *          Every request should have appropriate response value. If request does not
         *          have response, it should have value NEService::RESPONSE_ID_NONE
         **/
        const unsigned int* idRequestToResponseMap;     // fixed map of requests and its responses. Must have same size as idRequestCount

        /**
         * \brief   Map of parameter count in every response. Every response index
         *          is calculated by formula ('response ID' - NEService::RESPONSE_ID_FIRST)
         *          The size of this map should be equal to idResponseCount
         **/
        const unsigned int* idResponseParamCountMap;    // map of parameter count in every response. Must have same size as idResponseCount

    } SInterfaceData;

    //////////////////////////////////////////////////////////////////////////
    // Empty service interface
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The name of empty service
     **/
    const char * const  EmptyServiceName    = "AREG_EmptyService_";
    /**
     * \brief   Empty service version
     **/
    extern AREG_API const Version EmptyServiceVersion /*(1, 0, 0)*/;
    /**
     * \brief   Returns the instance of empty local service interface.
     **/
    AREG_API NEService::SInterfaceData & getEmptyInterface( void );

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
         * \brief   Destructor
         **/
        ~ParameterArray( void );

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Access state by given index of array allowed for writing.
         *          The index is calculated by formula
         *          ('response ID' - NEService::RESPONSE_ID_FIRST) or use
         *          GET_RESP_INDEX() macro
         **/
        inline NEService::StateArray& operator [] (int index);
        /**
         * \brief   Access read-only state by given index of array.
         *          The index is calculated by formula
         *          ('response ID' - NEService::RESPONSE_ID_FIRST) or use
         *          GET_RESP_INDEX() macro
         **/
        inline const NEService::StateArray& operator [] (int index) const;

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
        inline NEService::eDataStateType getAt(int row, int col) const;

        /**
         * \brief   Sets state of parameter of certain response.
         * \param   row         The index of response entry, which can be calculated
         *                      by macro GET_RESP_INDEX('response ID')
         * \param   col         The index of parameter in response call. 
         *                      First parameter has index zero
         * \param   newValue    The state to set for parameter
         **/
        inline void setAt(int row, int col, NEService::eDataStateType newValue);

        /**
         * \brief   Returns true if specified response index has parameters.
         *          The response index is calculated by macro GET_RESP_INDEX('response ID')
         **/
        inline bool hasParameters(int whichRespIndex) const;
        /**
         * \brief   Returns true if valid response index. For example,
         *          the response ID RESPONSE_ID_NONE does not have valid
         *          response index. The index of response is calculated
         *          by macro GET_RESP_INDEX('response ID')
         * \param   whichRespIndex  The index of response, should be calculated by
         *                          macro GET_RESP_INDEX('response ID')
         * \return  Returns true if given index is valid
         **/
        inline bool isValidParamIndex(int whichRespIndex) const;

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
        inline void setParamState(int whichParam, NEService::eDataStateType newState);

        /**
         * \brief   Resets all states of parameter of specified entry index.
         *          The index should be calculated by GET_RESP_INDEX('response ID')
         * \param   whichParam  The index of entry, should be calculated by GET_RESP_INDEX('response ID')
         **/
        void resetParamState(int whichParam);

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
        NEService::StateArray**  mParamList;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        ParameterArray( void );
        ParameterArray(const NEService::ParameterArray & /*src*/ );
        const NEService::ParameterArray& operator = (const NEService::ParameterArray & /*src*/ );
    };

    /**
     * \brief   Type of parameter array
     **/
    typedef NEService::ParameterArray    ParamState;
    /**
     * \brief   Type of state array
     **/
    typedef NEService::StateArray        AttrState;

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
        ~ProxyData( void );

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

    };
}

//////////////////////////////////////////////////////////////////////////
// Global namespace NEService inline function implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_STREAMABLE(NEService::eResultType)
IMPLEMENT_STREAMABLE(NEService::eDataStateType)
IMPLEMENT_STREAMABLE(NEService::eRequestType)
IMPLEMENT_STREAMABLE(NEService::eMessageDataType)
IMPLEMENT_STREAMABLE(NEService::eServiceConnection)
IMPLEMENT_STREAMABLE(NEService::eServiceRequestType)
IMPLEMENT_STREAMABLE(NEService::eServiceType)
IMPLEMENT_STREAMABLE(NEService::eFuncIdRange)

//////////////////////////////////////////////////////////////////////////
// namespace NEService inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool NEService::isRequestId(unsigned int msgId)
{
    return (msgId & static_cast<unsigned int>(NEService::ServiceCallRequest)) != 0;
}

inline bool NEService::isResponseId(unsigned int msgId)
{
    return (msgId & static_cast<unsigned int>(NEService::ServiceCallResponse)) != 0;
}

inline bool NEService::isAttributeId(unsigned int msgId)
{
    return (msgId & static_cast<unsigned int>(NEService::ServiceCallAttribute)) != 0;
}

inline bool NEService::isRegistrationId( unsigned int msgId )
{
    return (msgId & static_cast<unsigned int>(NEService::ServiceCallRegister)) != 0;
}

inline bool NEService::isEmptyFunctionId(unsigned int msgId)
{
    return (msgId == static_cast<unsigned int>(NEService::EMPTY_FUNCTION_ID));
}

inline bool NEService::isVersionNotifyId( unsigned int msgId )
{
    return (msgId == static_cast<unsigned int>(NEService::SI_NOTIFY_VERSION));
}

inline bool NEService::isConnectNotifyId( unsigned int msgId )
{
    return (msgId == static_cast<unsigned int>(NEService::SI_SERVICE_CONNECTION_NOTIFY));
}

inline bool NEService::isExecutableId(unsigned int msgId)
{
    return ( (msgId & static_cast<unsigned int>(NEService::ServiceCallRequest))    != 0 ||
             (msgId & static_cast<unsigned int>(NEService::ServiceCallResponse))   != 0 ||
             (msgId & static_cast<unsigned int>(NEService::ServiceCallAttribute))  != 0 ||
             (msgId == static_cast<unsigned int>(NEService::ServiceCallNoFunction))    );
}


inline NEService::eMessageDataType NEService::getMessageDataType( unsigned int msgId )
{
    if ( NEService::isRequestId(msgId) )
        return NEService::REQUEST_DATA_TYPE;
    else if (NEService::isResponseId(msgId))
        return NEService::RESPONSE_DATA_TYPE;
    else if (NEService::isAttributeId(msgId))
        return NEService::ATTRIBUTE_DATA_TYPE;
    else if (NEService::isRegistrationId(msgId))
        return NEService::SERVICE_DATA_TYPE;
    else
        return NEService::UNDEFINED_DATA_TYPE;
}

//////////////////////////////////////////////////////////////////////////
// class NEService::StateArray inline function implementation
//////////////////////////////////////////////////////////////////////////
inline void NEService::StateArray::resetStates( void )
{
    setAllState(NEService::DATA_UNAVAILABLE);
}

inline bool NEService::StateArray::hasParams( void ) const
{
    return (isEmpty() == false);
}

inline void NEService::StateArray::setState(int whichIndex, NEService::eDataStateType newState)
{
    ASSERT(isValidIndex(whichIndex));
    mValueList[whichIndex] = newState;
}

inline void NEService::StateArray::setAllState(NEService::eDataStateType newState)
{
    for ( int i = 0; i < this->mElemCount; ++ i )
        mValueList[i] = newState;
}

//////////////////////////////////////////////////////////////////////////
// class NEService::ParameterArray inline function implementation
//////////////////////////////////////////////////////////////////////////
inline NEService::StateArray& NEService::ParameterArray::operator [] ( int index )
{
    ASSERT(isValidParamIndex(index));
    return *(mParamList[index]);
}

inline const NEService::StateArray& NEService::ParameterArray::operator [] ( int index ) const
{
    ASSERT(isValidParamIndex(index));
    return *(mParamList[index]);
}

inline NEService::eDataStateType NEService::ParameterArray::getAt( int row, int col ) const
{
    ASSERT(isValidParamIndex(row) && mParamList[row]->isValidIndex(col));
    return mParamList[row]->getAt(col);
}

inline void NEService::ParameterArray::setAt( int row, int col, NEService::eDataStateType newValue )
{
    ASSERT(isValidParamIndex(row) && mParamList[row]->isValidIndex(col));
    mParamList[row]->setAt(col, newValue);
}

inline bool NEService::ParameterArray::hasParameters( int whichRespIndex ) const
{
    ASSERT(isValidParamIndex(whichRespIndex));
    return mParamList[whichRespIndex]->hasParams();
}

inline void NEService::ParameterArray::resetAllStates( void )
{
    for (int col = 0; col < mElemCount; col ++)
        mParamList[col]->resetStates();
}

inline bool NEService::ParameterArray::isValidParamIndex(int whichRespIndex) const
{
    return ((whichRespIndex >= 0) && (whichRespIndex < mElemCount));
}

inline void NEService::ParameterArray::setParamState(int whichParam, NEService::eDataStateType newState)
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
    return (param.hasParams() ? param[0] : NEService::DATA_UNAVAILABLE);
}

//////////////////////////////////////////////////////////////////////////
// class NEService enumerations string conversion
//////////////////////////////////////////////////////////////////////////

inline const char* NEService::getString(NEService::eResultType resultType)
{
    switch (resultType)
    {
    case    NEService::RESULT_UNDEFINED:
        return "NEService::RESULT_UNDEFINED";

    case    NEService::RESULT_ERROR:
        return "NEService::RESULT_ERROR";
    case NEService::RESULT_UNDELIVERED:
        return "NEService::RESULT_UNDELIVERED";
    case    NEService::RESULT_NOT_PROCESSED:
        return "NEService::RESULT_NOT_PROCESSED";
    case NEService::RESULT_PROCESSED:
        return "NEService::RESULT_PROCESSED";

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        return "NEService::RESULT_MESSAGE_UNDELIVERED";

    case    NEService::RESULT_OK:
        return "NEService::RESULT_OK";
    case    NEService::RESULT_INVALID:
        return "NEService::RESULT_INVALID";
    case    NEService::RESULT_REQUEST_ERROR:
        return "NEService::RESULT_REQUEST_ERROR";
    case    NEService::RESULT_REQUEST_BUSY:
        return "NEService::RESULT_REQUEST_BUSY";
    case    NEService::RESULT_REQUEST_CANCELED:
        return "NEService::RESULT_REQUEST_CANCELED";

    case    NEService::RESULT_DATA_OK:
        return "NEService::RESULT_DATA_OK";
    case    NEService::RESULT_DATA_INVALID:
        return "NEService::RESULT_DATA_INVALID";

    case    NEService::RESULT_SERVICE_OK:
        return "NEService::RESULT_SERVICE_OK";
    case    NEService::RESULT_SERVICE_UNAVAILABLE:
        return "NEService::RESULT_SERVICE_UNAVAILABLE";
    case    NEService::RESULT_SERVICE_INVALID:
        return "NEService::RESULT_SERVICE_INVALID";
    case    NEService::RESULT_SERVICE_REJECTED:
        return "NEService::RESULT_SERVICE_REJECTED";

    default:
        ASSERT(false);
        return "ERR: Undefined NEService::eResultType value!";
    }
}

inline const char* NEService::getString(NEService::eDataStateType dataState)
{
    switch (dataState)
    {
    case    NEService::DATA_UNDEFINED:
        return "NEService::DATA_UNDEFINED";

    case    NEService::DATA_OK:
        return "NEService::DATA_OK";

    case    NEService::DATA_INVALID:
        return "NEService::DATA_INVALID";

    case    NEService::DATA_UNAVAILABLE:
        return "NEService::DATA_UNAVAILABLE";

    case    NEService::DATA_UNEXPECTED_ERROR:
        return "NEService::DATA_UNEXPECTED_ERROR";

    default:
        ASSERT(false);
        return "ERR: Undefined NEService::eDataStateType value!";
    }
}

inline const char* NEService::getString( NEService::eRequestType resultType )
{
    switch (resultType)
    {
    case    NEService::REQUEST_UNPROCESSED:
        return "NEService::REQUEST_UNPROCESSED";
    case    NEService::REQUEST_START_NOTIFY:
        return "NEService::REQUEST_START_NOTIFY";
    case    NEService::REQUEST_STOP_NOTIFY:
        return "NEService::REQUEST_STOP_NOTIFY";        
    case    NEService::REQUEST_REMOVE_ALL_NOTIFY:
        return "NEService::REQUEST_REMOVE_ALL_NOTIFY";
    case    NEService::REQUEST_CALL:
        return "NEService::REQUEST_CALL";
    case    NEService::REQUEST_CONNECTION:
        return "NEService::REQUEST_CONNECTION";
    case    NEService::REQUEST_CLIENT_CONNECTION:
        return "NEService::REQUEST_CLIENT_CONNECTION";
    case    NEService::REQUEST_LOAD_COMPONENT:
        return "NEService::REQUEST_LOAD_COMPONENT";
    default:
        ASSERT(false);
        return "ERR: Undefined NEService::eRequestType value!";
    }
}

inline const char* NEService::getString( NEService::eMessageDataType dataType )
{
    switch (dataType)
    {
    case    NEService::UNDEFINED_DATA_TYPE:
        return "NEService::UNDEFINED_DATA_TYPE";

    case    NEService::REQUEST_DATA_TYPE:
        return "NEService::REQUEST_DATA_TYPE";

    case    NEService::RESPONSE_DATA_TYPE:
        return "NEService::RESPONSE_DATA_TYPE";

    case    NEService::ATTRIBUTE_DATA_TYPE:
        return "NEService::ATTRIBUTE_DATA_TYPE";

    default:
        ASSERT(false);
        return "ERR: Undefined NEService::eMessageDataType value!";
    }
}

inline const char* NEService::getString( NEService::eServiceConnection serviceConnection )
{
    switch (serviceConnection)
    {
    case NEService::ServiceConnectionUnknown:
        return "NEService::ServiceConnectionUnknown";
    case NEService::ServiceDisconnected:
        return "NEService::ServiceDisconnected";
    case NEService::ServicePending:
        return "NEService::ServicePending";
    case NEService::ServiceConnected:
        return "NEService::ServiceConnected";
    case NEService::ServiceRejected:
        return "NEService::ServiceRejected";
    case NEService::ServiceFailed:
        return "NEService::ServiceFailed";
    case NEService::ServiceShutdown:
        return "NEService::ServiceShutdown";
    default:
        ASSERT(false);
        return "ERR: Undefined NEService::eServiceConnection value!";
    }
}

inline const char * NEService::getString( NEService::eServiceRequestType svcRequestType )
{
    switch ( svcRequestType )
    {
    case NEService::SERVICE_REQUEST_REGISTER_CLIENT:
        return "NEService::SERVICE_REQUEST_REGISTER_CLIENT";
    case NEService::SERVICE_REQUEST_UNREGISTER_CLIENT:
        return "NEService::SERVICE_REQUEST_UNREGISTER_CLIENT";
    case NEService::SERVICE_REQUEST_REGISTER_STUB:
        return "NEService::SERVICE_REQUEST_REGISTER_STUB";
    case NEService::SERVICE_REQUEST_UNREGISTER_STUB:
        return "NEService::SERVICE_REQUEST_UNREGISTER_STUB";
    default:
        return "ERR: Unexpected NEService::eServiceRequestType value!!!";
    }
}

inline const char * NEService::getString( NEService::eServiceType srvcType )
{
    switch ( srvcType )
    {
    case NEService::ServiceLocal:
        return "NEService::ServiceLocal";
    case NEService::ServiceRemote:
        return "NEService::ServiceRemote";
    case NEService::ServiceAny:
        return "NEService::ServiceAny";
    default:
        return "ERR: Unexpected NEService::eServiceRequestType value!!!";
    }
}


inline const char * NEService::getString( NEService::eFuncIdRange funcId )
{
    switch ( funcId )
    {
    case NEService::EMPTY_FUNCTION_ID:
        return "NEService::EMPTY_FUNCTION_ID";
    case NEService::SI_REQUEST_VERSION:
        return "NEService::SI_REQUEST_VERSION";
    case NEService::SI_NOTIFY_VERSION:
        return "NEService::SI_NOTIFY_VERSION";
    case NEService::SI_SERVICE_CONNECTION_REQUEST:
        return "NEService::SI_SERVICE_CONNECTION_REQUEST";
    case NEService::SI_SERVICE_CONNECTION_NOTIFY:
        return "NEService::SI_SERVICE_CONNECTION_NOTIFY";
    case NEService::SI_ROUTER_CONNECT:
        return "NEService::SI_BROKER_CONNECT";
    case NEService::SI_ROUTER_DISCONNECT:
        return "NEService::SI_BROKER_DISCONNECT";
    case NEService::SI_ROUTER_NOTIFY:
        return "NEService::SI_BROKER_NOTIFY";
    case NEService::SI_ROUTER_REGISTER:
        return "NEService::SI_BROKER_REGISTER";
    case NEService::SI_ROUTER_QUERY:
        return "NEService::SI_BROKER_QUERY";
    case NEService::SI_ROUTER_REGISTER_NOTIFY:
        return "NEService::SI_BROKER_REGISTER_NOTIFY";
    default:
        if ( funcId >= NEService::REQUEST_ID_FIRST && funcId <= NEService::REQUEST_ID_LAST )
            return "Request ID range";
        else if ( funcId >= NEService::RESPONSE_ID_FIRST && funcId <= NEService::RESPONSE_ID_LAST )
            return "Response ID range";
        else if ( funcId >= NEService::ATTRIBUTE_ID_FIRST && funcId <= NEService::ATTRIBUTE_ID_LAST )
            return "Attribute ID range";
        else
            return "ERR: Unexpected ID";
    }
}

#endif  //AREG_COMPONENT_NESERVICE_HPP
