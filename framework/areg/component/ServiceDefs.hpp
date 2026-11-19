#ifndef AREG_COMPONENT_SERVICEDEFS_HPP
#define AREG_COMPONENT_SERVICEDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/base/MathDefs.hpp"
#include "areg/component/EventDefs.hpp"

#include <limits>
#include <new>


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
    , RequestUnsupported =  8259 /*0x2043*/  //!< the provider has no such request.      Bits: 0010 0000 0100 0011

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
    , ProviderRestarting    = 4096  //!< The service provider is being restarted by the watchdog.
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

//!< The highest message ID of the system. A system message has no interface call type.
constexpr uint32_t  SYSTEM_ID_LAST      { 0x0000FFFFu };

//!< The index of no entry: the ID is not an entry of the kind.
constexpr uint32_t  INVALID_MESSAGE_INDEX   { 0x0000FFFFu };

//!< The response index of a request without response.
constexpr uint32_t  RESPONSE_INDEX_NONE     { areg::INVALID_MESSAGE_INDEX };

//!< The number of kinds of service interface entries: request, response, broadcast and attribute.
constexpr uint32_t  MESSAGE_KIND_COUNT      { 4u };

/**
 * \brief   The message IDs of the system. A system message is told apart from an interface entry
 *          by its call type, not by its value.
 **/
enum class FuncIdRange  : uint32_t
{
    //!< Empty function ID.
      EmptyFunctionId       = 0u
    //!< Sent by service manager to targets (client or stub) to notify connection status update.
    , ResponseServiceProviderConnection = 0x00008004
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
    //!< Sent by observer or log collector service to the log source to start or stop sending the logs.
    , ServiceLogUpdateSourceState
    //!< Sent by log source to notify that it started or stopped sending the logs.
    , ServiceLogSourceStateUpdated
    //!< Sent by observer or log collector service to the log source to read the log configuration file again.
    , ServiceLogRestoreConfiguration
    //!< Sent by log source to notify that the configuration file is read again.
    , ServiceLogConfigurationRestored
    //!< Sent by Routing Service to both processes when a public role name is registered twice.
    , SystemServiceNotifyDuplicate
};

static_assert(static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyDuplicate) <= areg::SYSTEM_ID_LAST, "A system message ID must stay in the reserved range.");

/**
 * \brief   Returns string representation of FuncIdRange.
 **/
[[nodiscard]]
inline constexpr const char* as_string( areg::FuncIdRange value ) noexcept;

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

/**
 * \brief   Returns the position of an entry kind in InterfaceData::idTables: request 0, response 1,
 *          broadcast 2, attribute 3, and MESSAGE_KIND_COUNT for any other call type.
 **/
[[nodiscard]]
inline constexpr uint32_t kind_index(areg::CallType kind) noexcept
{
    const uint32_t pos{ static_cast<uint32_t>(kind) - static_cast<uint32_t>(areg::CallType::Request) };
    return (pos < areg::MESSAGE_KIND_COUNT ? pos : areg::MESSAGE_KIND_COUNT);
}

//////////////////////////////////////////////////////////////////////////
// MessageTable structure declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The entries of one kind of a service interface: their message IDs by index, and the
 *          perfect-hash table that finds the index of a message ID.
 *          The slot of an ID is (ID * mtFactor) >> mtShift. A slot holds an index; a slot no ID
 *          maps to holds 0, which the comparison of the IDs then refuses.
 **/
struct MessageTable
{
    const uint32_t *    mtIds   { nullptr };    //!< The message ID of every entry, by index. nullptr if there is none.
    const uint16_t *    mtSlots { nullptr };    //!< The index of every slot of the hash table. nullptr if there is no entry.
    uint32_t            mtCount { 0u };         //!< The number of entries.
    uint32_t            mtFactor{ 0u };         //!< The odd multiplier of the hash.
    uint32_t            mtShift { 0u };         //!< The shift of the hash: 32 minus the bits of the table size.

    /**
     * \brief   Returns the index of the entry with the message ID, or areg::INVALID_MESSAGE_INDEX.
     **/
    [[nodiscard]]
    inline constexpr uint32_t find(uint32_t msgId) const noexcept
    {
        if (mtCount == 0u)
            return areg::INVALID_MESSAGE_INDEX;

        const uint32_t index{ mtSlots[static_cast<uint32_t>(msgId * mtFactor) >> mtShift] };
        return (mtIds[index] == msgId ? index : areg::INVALID_MESSAGE_INDEX);
    }

    /**
     * \brief   Returns true if every entry is found at its own index.
     **/
    [[nodiscard]]
    inline constexpr bool finds_every_entry() const noexcept
    {
        for (uint32_t i = 0u; i < mtCount; ++i)
        {
            if (find(mtIds[i]) != i)
                return false;
        }

        return true;
    }
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
    const char *        idServiceName{ nullptr };               //!< The name of service interface.
    Version             idVersion{ 0, 0, 0 };                   //!< Service interface implementation version
    ServiceType         idServiceType{ ServiceType::Invalid };  //!< The service type. Should be either local or public / remote.

    /**
     * \brief   The entries of every kind, at the position areg::kind_index() of the kind.
     **/
    MessageTable        idTables[areg::MESSAGE_KIND_COUNT]{ };

    /**
     * \brief   The response index of every request, by request index.
     *          A request without response has areg::RESPONSE_INDEX_NONE. nullptr if there is no request.
     **/
    const uint16_t *    idRequestToResponse{ nullptr };

    /**
     * \brief   The number of parameters of every response, by response index, followed by
     *          the number of parameters of every broadcast, by broadcast index.
     **/
    const uint32_t *    idParamCountMap{ nullptr };

    /**
     * \brief   Returns the requests of the interface.
     **/
    [[nodiscard]]
    inline constexpr const MessageTable & requests() const noexcept
    {
        return idTables[0u];
    }

    /**
     * \brief   Returns the responses of the interface.
     **/
    [[nodiscard]]
    inline constexpr const MessageTable & responses() const noexcept
    {
        return idTables[1u];
    }

    /**
     * \brief   Returns the broadcasts of the interface.
     **/
    [[nodiscard]]
    inline constexpr const MessageTable & broadcasts() const noexcept
    {
        return idTables[2u];
    }

    /**
     * \brief   Returns the attributes of the interface.
     **/
    [[nodiscard]]
    inline constexpr const MessageTable & attributes() const noexcept
    {
        return idTables[3u];
    }

    /**
     * \brief   Returns the number of entries of the kind; 0 for a call type that is no entry kind.
     **/
    [[nodiscard]]
    inline constexpr uint32_t count(areg::CallType kind) const noexcept
    {
        const uint32_t pos{ areg::kind_index(kind) };
        return (pos < areg::MESSAGE_KIND_COUNT ? idTables[pos].mtCount : 0u);
    }

    /**
     * \brief   Returns the index of the entry of the kind with the message ID, or areg::INVALID_MESSAGE_INDEX.
     **/
    [[nodiscard]]
    inline constexpr uint32_t find(areg::CallType kind, uint32_t msgId) const noexcept
    {
        const uint32_t pos{ areg::kind_index(kind) };
        return (pos < areg::MESSAGE_KIND_COUNT ? idTables[pos].find(msgId) : areg::INVALID_MESSAGE_INDEX);
    }

    /**
     * \brief   Returns the message ID of the entry of the kind at the index. The entry must exist.
     **/
    [[nodiscard]]
    inline constexpr uint32_t message_id(areg::CallType kind, uint32_t index) const noexcept
    {
        return idTables[areg::kind_index(kind)].mtIds[index];
    }

    /**
     * \brief   Returns the response index of the request at the index, or areg::RESPONSE_INDEX_NONE.
     **/
    [[nodiscard]]
    inline constexpr uint32_t response_index(uint32_t reqIndex) const noexcept
    {
        return (reqIndex < idTables[0u].mtCount ? static_cast<uint32_t>(idRequestToResponse[reqIndex]) : areg::RESPONSE_INDEX_NONE);
    }
};

//////////////////////////////////////////////////////////////////////////
// ListenerSlots class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A fixed number of listener lists, one per slot, allocated once at construction.
 *          Every list starts empty and reserves nothing. The slot of an interface entry is
 *          areg::listener_slot().
 **/
template <typename LIST>
class ListenerSlots
{
public:
    /**
     * \brief   Allocates 'count' empty lists. On a failed allocation the object holds none and is invalid.
     **/
    explicit ListenerSlots(uint32_t count) noexcept
        : mLists(count != 0u ? static_cast<LIST *>(::operator new(sizeof(LIST) * count, std::nothrow)) : nullptr)
        , mCount(count)
    {
        if (mLists == nullptr)
            return;

        for (uint32_t i = 0u; i < mCount; ++i)
        {
            new (mLists + i) LIST(0u);
        }
    }

    ~ListenerSlots()
    {
        if (mLists == nullptr)
            return;

        for (uint32_t i = 0u; i < mCount; ++i)
        {
            mLists[i].~LIST();
        }

        ::operator delete(mLists);
    }

    /**
     * \brief   Returns true if every list is allocated.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept
    {
        return (mCount == 0u) || (mLists != nullptr);
    }

    /**
     * \brief   Returns the number of slots, 0 if the allocation failed.
     **/
    [[nodiscard]]
    inline uint32_t size() const noexcept
    {
        return (mLists != nullptr ? mCount : 0u);
    }

    /**
     * \brief   Returns the list of the slot, nullptr for a slot out of range.
     **/
    [[nodiscard]]
    inline LIST * at(uint32_t slot) noexcept
    {
        return ((mLists != nullptr) && (slot < mCount) ? mLists + slot : nullptr);
    }

    /**
     * \brief   Returns the list of the slot, nullptr for a slot out of range.
     **/
    [[nodiscard]]
    inline const LIST * at(uint32_t slot) const noexcept
    {
        return ((mLists != nullptr) && (slot < mCount) ? mLists + slot : nullptr);
    }

    /**
     * \brief   Removes every entry of every list. Keeps the slots.
     **/
    inline void clear() noexcept
    {
        for (uint32_t i = 0u; i < size(); ++i)
        {
            mLists[i].clear();
        }
    }

private:
    LIST *      mLists; //!< The lists, one per slot.
    uint32_t    mCount; //!< The number of slots.

    ListenerSlots(const ListenerSlots &) = delete;
    ListenerSlots & operator = (const ListenerSlots &) = delete;
};

/**
 * \brief   Returns the number of listener slots of the interface: one per response, broadcast and attribute.
 **/
[[nodiscard]]
inline constexpr uint32_t listener_slot_count(const InterfaceData & ifData) noexcept
{
    return ifData.responses().mtCount + ifData.broadcasts().mtCount + ifData.attributes().mtCount;
}

/**
 * \brief   Returns the listener slot of the response, broadcast or attribute at the index of its kind:
 *          the responses come first, then the broadcasts, then the attributes. Returns
 *          areg::INVALID_MESSAGE_INDEX for a request, any other kind or an index out of range.
 **/
[[nodiscard]]
inline constexpr uint32_t listener_slot(const InterfaceData & ifData, areg::CallType kind, uint32_t index) noexcept
{
    const uint32_t responses { ifData.responses().mtCount };
    const uint32_t broadcasts{ ifData.broadcasts().mtCount };
    switch (kind)
    {
    case areg::CallType::Response:
        return (index < responses ? index : areg::INVALID_MESSAGE_INDEX);
    case areg::CallType::Broadcast:
        return (index < broadcasts ? responses + index : areg::INVALID_MESSAGE_INDEX);
    case areg::CallType::Attribute:
        return (index < ifData.attributes().mtCount ? responses + broadcasts + index : areg::INVALID_MESSAGE_INDEX);
    default:
        return areg::INVALID_MESSAGE_INDEX;
    }
}

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
 *          response parameter indexed by the index of the response in the response list.
 *
 * \example Parameter Array
 *
 *          For example, if there is a response function prototype
 *          response_areg(const bool &param1, const int32_t &param2)
 *          at index 2 of the response list, the param1 state is states[2][0].
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
     * \param   row     The index of the response in the response list.
     * \param   col     The parameter index (zero-based).
     **/
    [[nodiscard]]
    inline areg::DataState at(uint32_t row, uint32_t col) const noexcept;

    /**
     * \brief   Sets the state of a parameter at the given response and parameter indices.
     *
     * \param   row         The index of the response in the response list.
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
     * \param   whichParam      The index of the response in the response list.
     * \param   newState        The state to set for all parameters.
     **/
    inline void set_param_state(uint32_t whichParam, areg::DataState newState) noexcept;

    /**
     * \brief   Resets parameter states for the response at the given index.
     *
     * \param   whichParam      The index of the response in the response list.
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

    /**
     * \brief   Returns the number of bytes one entry of the parameter buffer occupies: the
     *          StateArray followed by its states, rounded up to alignof(StateArray) so that the
     *          next entry starts on an address a StateArray may live at.
     *
     * \note    The reservation and the placement must both use this function, never the raw sum.
     *
     * \param   paramCount  The number of states of the entry.
     **/
    static constexpr uint32_t param_entry_space(uint32_t paramCount) noexcept;

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
     * \brief   Sets the data state of an attribute, a response or a broadcast of the interface.
     *
     * \param   kind        Attribute, Response or Broadcast. Any other kind is ignored.
     * \param   index       The index of the entry in its kind. An index out of range is ignored.
     * \param   newState    The state to set. For a response or broadcast, the state of all its parameters.
     **/
    inline void set_data_state(areg::CallType kind, uint32_t index, areg::DataState newState) noexcept;

    /**
     * \brief   Sets the state of the attribute at the index. An index out of range is ignored.
     **/
    inline void set_attribute_state(uint32_t index, areg::DataState newState) noexcept;

    /**
     * \brief   Sets the state of all parameters of the response or broadcast at the index of its kind.
     *          Any other kind and an index out of range are ignored.
     **/
    inline void set_param_state(areg::CallType kind, uint32_t index, areg::DataState newState) noexcept;

    /**
     * \brief   Returns the data state of an attribute, a response or a broadcast.
     *          Returns DataUnexpectedError for any other kind or an index out of range.
     **/
    [[nodiscard]]
    inline areg::DataState data_state(areg::CallType kind, uint32_t index) const noexcept;

    /**
     * \brief   Returns the response index of the request at the index, or areg::RESPONSE_INDEX_NONE
     *          if the request has no response or the index is out of range.
     **/
    [[nodiscard]]
    inline uint32_t response_index(uint32_t reqIndex) const noexcept;

    /**
     * \brief   Returns the data state of the attribute at the index, or DataUnexpectedError if the index is out of range.
     **/
    [[nodiscard]]
    inline areg::DataState attribute_state(uint32_t index) const noexcept;

    /**
     * \brief   Returns the parameter state of the response or broadcast at the index of its kind,
     *          or DataUnexpectedError for any other kind or an index out of range.
     **/
    [[nodiscard]]
    inline areg::DataState param_state(areg::CallType kind, uint32_t index) const noexcept;

    /**
     * \brief   Returns the row of the parameter table of the response or broadcast at the index of
     *          its kind: the responses come first, then the broadcasts. Returns a row out of range
     *          for any other kind or an index out of range.
     **/
    [[nodiscard]]
    inline uint32_t param_row(areg::CallType kind, uint32_t index) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
public:

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
AREG_IMPLEMENT_STREAMABLE(areg::ServiceConnectionState)
AREG_IMPLEMENT_STREAMABLE(areg::DisconnectReason)
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
    case areg::DisconnectReason::ProviderRestarting:
        return areg::ServiceConnectionState::Disconnected;

    case areg::DisconnectReason::SystemShutdown:
        return areg::ServiceConnectionState::Shutdown;

    default:
        return areg::ServiceConnectionState::Connected;
    }
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

constexpr uint32_t areg::ParameterArray::param_entry_space(uint32_t paramCount) noexcept
{
    constexpr uint32_t align{ static_cast<uint32_t>(alignof(areg::StateArray)) };
    const uint32_t used{ static_cast<uint32_t>(sizeof(areg::StateArray)
                                               + paramCount * sizeof(areg::DataState)) };
    return (used + align - 1u) & ~(align - 1u);
}

inline uint32_t areg::ParameterArray::count_param_space(const uint32_t* params, uint32_t count) noexcept
{
    uint32_t result = 0;
    // space for size of class areg::StateArray +
    // space for size of areg::DataState multiplied on number of parameters.
    // If number of parameters is zero, do not reserve.
    for (uint32_t i = 0; i < count; ++i)
    {
        result += params[i] != 0 ? param_entry_space(params[i]) : 0;
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
    : mIfData     (ifData)
    , mAttrState  (ifData.attributes().mtCount)
    , mParamState (ifData)
{
    reset();
}

inline void areg::ProxyData::reset() noexcept
{
    mAttrState.reset();
    mParamState.reset();
}

inline void areg::ProxyData::set_data_state(areg::CallType kind, uint32_t index, areg::DataState newState) noexcept
{
    if (kind == areg::CallType::Attribute)
    {
        set_attribute_state(index, newState);
    }
    else
    {
        set_param_state(kind, index, newState);
    }
}

inline void areg::ProxyData::set_attribute_state(uint32_t index, areg::DataState newState) noexcept
{
    if (index < mAttrState.size())
    {
        mAttrState[index] = newState;
    }
}

inline uint32_t areg::ProxyData::param_row(areg::CallType kind, uint32_t index) const noexcept
{
    const uint32_t responses{ mIfData.responses().mtCount };
    if (kind == areg::CallType::Response)
        return (index < responses ? index : areg::INVALID_MESSAGE_INDEX);
    else if (kind == areg::CallType::Broadcast)
        return (index < mIfData.broadcasts().mtCount ? responses + index : areg::INVALID_MESSAGE_INDEX);
    else
        return areg::INVALID_MESSAGE_INDEX;
}

inline void areg::ProxyData::set_param_state(areg::CallType kind, uint32_t index, areg::DataState newState) noexcept
{
    const uint32_t row{ param_row(kind, index) };
    if (mParamState.is_valid_index(row))
    {
        mParamState.set_param_state(row, newState);
    }
}

inline areg::DataState areg::ProxyData::data_state(areg::CallType kind, uint32_t index) const noexcept
{
    return (kind == areg::CallType::Attribute ? attribute_state(index) : param_state(kind, index));
}

inline uint32_t areg::ProxyData::response_index(uint32_t reqIndex) const noexcept
{
    return mIfData.response_index(reqIndex);
}

inline areg::DataState areg::ProxyData::attribute_state( uint32_t index ) const noexcept
{
    return (index < mAttrState.size() ? mAttrState[index] : areg::DataState::DataUnexpectedError);
}

inline areg::DataState areg::ProxyData::param_state( areg::CallType kind, uint32_t index ) const noexcept
{
    const uint32_t row{ param_row(kind, index) };
    if (!mParamState.is_valid_index(row))
        return areg::DataState::DataUnexpectedError;

    const areg::StateArray& param = mParamState[row];
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
    case    areg::ResultType::RequestUnsupported:
        return "areg::ResultType::RequestUnsupported";

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
        return "ERR: Undefined areg::RequestType value!";
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
    case areg::DisconnectReason::ProviderRestarting:
        return "areg::DisconnectReason::ProviderRestarting";
    default:
        return "ERR: Undefined areg::DisconnectReason value!";
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
        return "ERR: Unexpected areg::ServiceType value!!!";
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
    case areg::FuncIdRange::ServiceLogUpdateSourceState:
        return "areg::FuncIdRange::ServiceLogUpdateSourceState";
    case areg::FuncIdRange::ServiceLogSourceStateUpdated:
        return "areg::FuncIdRange::ServiceLogSourceStateUpdated";
    case areg::FuncIdRange::ServiceLogRestoreConfiguration:
        return "areg::FuncIdRange::ServiceLogRestoreConfiguration";
    case areg::FuncIdRange::ServiceLogConfigurationRestored:
        return "areg::FuncIdRange::ServiceLogConfigurationRestored";
    case areg::FuncIdRange::SystemServiceNotifyDuplicate:
        return "areg::FuncIdRange::SystemServiceNotifyDuplicate";

    default:
        return "ERR: Unexpected ID";
    }
}

#endif  // AREG_COMPONENT_SERVICEDEFS_HPP
