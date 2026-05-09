#ifndef AREG_COMPONENT_SERVICE_STATE_HPP
#define AREG_COMPONENT_SERVICE_STATE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/service/state.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, areg namespace contains
 *              collection of classes, structures and types defining
 *              Service Interface.
 *              Classes declared in the file:
 *              StateArrayBase        - Fixed-size array of data states
 *              PrameterArray         - Manages parameter state information for all responses
 *              ProxyData             - Container for proxy-related data derived from service
 *
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

#include "areg/component/service/types.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

namespace areg {
    class RemoteMessage;
    class Channel;
}

//////////////////////////////////////////////////////////////////////////
// areg namespace for services
//////////////////////////////////////////////////////////////////////////
namespace areg {

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
 * \brief   Sends a pre-serialized RemoteMessage directly to the IPC send thread,
 *          bypassing all event dispatch and serialization overhead.
 *          The caller is responsible for ensuring the message was built while the
 *          connection was valid and the target cookie is still active.
 *
 * \param   msg     The pre-built message to send.
 * \return  Returns true if the message was accepted by the send thread.
 **/
AREG_API bool send_raw_message(const areg::RemoteMessage& msg) noexcept;

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
    return ( (msgId  & static_cast<uint32_t>(areg::ServiceCallType::RequestFunction) ) != 0 ||
             (msgId  & static_cast<uint32_t>(areg::ServiceCallType::ResponseFunction)) != 0 ||
             (msgId  & static_cast<uint32_t>(areg::ServiceCallType::AttributeUpdate) ) != 0 ||
             (msgId == static_cast<uint32_t>(areg::ServiceCallType::NoFunction)      ) );
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

#endif // AREG_COMPONENT_SERVICE_STATE_HPP
