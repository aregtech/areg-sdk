#ifndef AREG_COMPONENT_PROXYADDRESS_HPP
#define AREG_COMPONENT_PROXYADDRESS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/ProxyAddress.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Proxy Address class.
 *              Every Proxy is reached by unique address, which is a 
 *              part of Proxy object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/ServiceDefs.hpp"
#include "areg/component/ServiceAddress.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/component/Channel.hpp"

#include <utility>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class StubAddress;
    class Event;
    class ServiceRequestEvent;
    class ServiceResponseEvent;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ProxyAddress class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Structured address for proxy objects, containing thread, component, and service
 *          interface information needed to identify a proxy and deliver events.
 **/
class AREG_API ProxyAddress   : public    ServiceAddress
{
//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts a proxy address to a path string containing process ID, thread name, role
     *          name, and service name.
     *
     * \param   addrProxy       The proxy address to convert.
     * \return  Path string representation of the proxy address.
     **/
    [[nodiscard]]
    static String to_path( const ProxyAddress & addrProxy );

    /**
     * \brief   Parses a proxy path string and creates a proxy address from it.
     *
     * \param       pathProxy   The proxy path string to parse.
     * \param[out]  nextPart    If not null, receives pointer to remaining unparsed data in the path string.
     * \return  Parsed proxy address object.
     **/
    [[nodiscard]]
    static ProxyAddress from_path(const char * pathProxy, const char** nextPart = nullptr);

    /**
     * \brief   Returns a predefined invalid proxy address for validation.
     **/
    [[nodiscard]]
    static const ProxyAddress & invalid_proxy_address() noexcept;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    ProxyAddress();

    ProxyAddress(ProxyAddress&& source) noexcept;

    /**
     * \brief   Creates a proxy address from service details and component role name.
     *
     * \param   serviceName         The name of the service.
     * \param   serviceVersion      The version of the service.
     * \param   serviceType         The type of service (local or remote).
     * \param   roleName            The role name of the component.
     * \param   threadName          The name of the thread where the proxy acts. If empty, uses the current thread.
     **/
    ProxyAddress( const String & serviceName
                , const Version & serviceVersion
                , areg::ServiceType serviceType
                , const String & roleName
                , const String & threadName = String::empty_string() );
    /**
     * \brief   Creates a proxy address from service details and component role number.
     *
     * \param   serviceNum          The unique number of the service.
     * \param   serviceVersion      The version of the service.
     * \param   serviceType         The type of service (local or remote).
     * \param   roleNum             The role unique number of the component.
     * \param   threadNum           The unique number of the thread where the proxy acts. If zero, uses the current thread.
     * \param   proxyNum            The unique number of the proxy address.
     **/
    ProxyAddress( const UniqueNumber serviceNum
                , const Version& serviceVersion
                , areg::ServiceType serviceType
                , const UniqueNumber roleNum
                , const UniqueNumber threadNum
                , const UniqueNumber proxyNum = areg::CHECKSUM_IGNORE);
    /**
     * \brief   Creates a proxy address from a service item and component role name.
     *
     * \param   service         Service item containing name, version, and type.
     * \param   roleName        The role name of the component.
     * \param   threadName      The name of the thread where the proxy acts. If empty, uses the current thread.
     **/
    ProxyAddress( const ServiceItem & service, const String & roleName, const String & threadName = String::empty_string() );
    /**
     * \brief   Creates a proxy address from a service item and component role number.
     *
     * \param   service         Service item containing name, version, and type.
     * \param   roleNum             The role unique number of the component.
     * \param   threadNum           The unique number of the thread where the proxy acts. If zero, uses the current thread.
     * \param   proxyNum            The unique number of the proxy address.
     **/
    ProxyAddress(const ServiceItem& service, const UniqueNumber roleNum, const UniqueNumber threadNum, const UniqueNumber proxyNum = areg::CHECKSUM_IGNORE);
    /**
     * \brief   Creates a proxy address from service interface data and component role name.
     *
     * \param   siData          Service interface data containing service information.
     * \param   roleName        The role name of the component.
     * \param   threadName      The name of the thread where the proxy acts. If empty, uses the
     *                          current thread.
     **/
    ProxyAddress( const areg::InterfaceData & siData, const String & roleName, const String & threadName = String::empty_string() );

    /**
     * \brief   Creates a proxy address by copying a service address.
     **/
    explicit ProxyAddress(const ServiceAddress & source);

    explicit ProxyAddress(const StubAddress& source);

    /**
     * \brief   Creates a proxy address by moving a service address.
     **/
    explicit ProxyAddress(ServiceAddress && source);

    /**
     * \brief   Creates a proxy address from shared service identity and endpoint fields (consumer side).
     *
     * \param   rawService  Shared service interface identity.
     * \param   endPoint    Consumer endpoint (consumer field of EventHeader).
     **/
    explicit ProxyAddress(const areg::RawService& rawService, const areg::Endpoint& endPoint);

    /**
     * \brief   Creates a proxy address from an EventHeader (consumer field).
     **/
    explicit ProxyAddress(const areg::EventHeader& header);

    ProxyAddress(const ProxyAddress& source) = default;

    virtual ~ProxyAddress() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    inline ProxyAddress & operator = ( const ProxyAddress & source );

    inline ProxyAddress & operator = ( ProxyAddress && source ) noexcept;

    [[nodiscard]]
    inline bool operator == ( const ProxyAddress & other ) const noexcept;

    [[nodiscard]]
    inline bool operator == (const StubAddress & addrStub ) const noexcept;

    [[nodiscard]]
    inline bool operator != ( const ProxyAddress & other ) const noexcept;

    /**
     * \brief   Converts the proxy address to a 32-bit hash value.
     **/
    [[nodiscard]]
    inline explicit operator uint32_t () const noexcept;

    /**
     * \brief   Deserializes date and time value from stream.
     *
     * \param   stream      Streaming object containing serialized date and time.
     * \param[out] input    ProxyAddress object initialized from deserialized stream data.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, ProxyAddress & input );

    /**
     * \brief   Serializes date and time value to stream.
     *
     * \param[out] stream   Streaming object where date and time will be serialized.
     * \param   output      ProxyAddress object to serialize.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const ProxyAddress& output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if the proxy address is for a local service.
     **/
    [[nodiscard]]
    inline bool is_local_address() const noexcept;

    /**
     * \brief   Returns true if the proxy address is for a remote service.
     **/
    [[nodiscard]]
    inline bool is_remote_address() const noexcept;

    /**
     * \brief   Returns true if the source of the communication channel is local (same process).
     **/
    [[nodiscard]]
    inline bool is_source_local() const noexcept;

    /**
     * \brief   Returns true if the source of the communication channel is external (different
     *          process).
     **/
    [[nodiscard]]
    inline bool is_source_public() const noexcept;

    /**
     * \brief   Returns true if the target of the communication channel is local (same process).
     **/
    [[nodiscard]]
    inline bool is_target_local() const noexcept;

    /**
     * \brief   Returns true if the target of the communication channel is external (different
     *          process).
     **/
    [[nodiscard]]
    inline bool is_target_public() const noexcept;

    /**
     * \brief   Returns the thread address associated with this proxy.
     **/
    [[nodiscard]]
    inline const ThreadAddress & thread() const noexcept;

    /**
     * \brief   Sets the thread for this proxy by ThreadAddress.
     *
     * \param   thread      The thread address to set.
     **/
    void set_thread( const ThreadAddress & thread );

    /**
     * \brief   Sets the thread for this proxy by name.
     *
     * \param   threadName      The thread name to set.
     **/
    void set_thread( const String & threadName );
    /**
     * \brief   Returns the communication channel of this proxy.
     **/
    [[nodiscard]]
    inline const Channel & channel() const noexcept;
    /**
     * \brief   Sets the communication channel for this proxy.
     *
     * \param   channel     The channel to set.
     **/
    inline void set_channel( const Channel & channel ) noexcept;
    /**
     * \brief   Returns the cookie value of this proxy.
     **/
    [[nodiscard]]
    inline ITEM_ID cookie() const noexcept;
    /**
     * \brief   Sets the cookie value for this proxy.
     *
     * \param   cookie      The cookie value to set.
     **/
    inline void set_cookie(const ITEM_ID & cookie ) noexcept;
    /**
     * \brief   Returns the source ID of this proxy.
     **/
    [[nodiscard]]
    inline ITEM_ID source() const noexcept;
    /**
     * \brief   Sets the source ID for this proxy.
     *
     * \param   source      The source ID to set.
     **/
    inline void set_source(const ITEM_ID & source ) noexcept;
    /**
     * \brief   Returns the target ID of this proxy.
     **/
    [[nodiscard]]
    inline ITEM_ID target() const noexcept;
    /**
     * \brief   Sets the target ID for this proxy.
     *
     * \param   target      The target ID to set.
     **/
    inline void set_target(const ITEM_ID & target) noexcept;

    /**
     * \brief   Returns true if the proxy address is valid.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Marks the communication channel as invalid.
     **/
    inline void invalidate_channel() noexcept;

    /**
     * \brief   Returns true if the specified stub address is compatible with this proxy.
     *
     * \param   addrStub    The stub address to check for compatibility.
     * \return  True if the stub address is compatible.
     **/
    [[nodiscard]]
    bool is_provider_compatible( const StubAddress & addrStub ) const noexcept;

    /**
     * \brief   Delivers a service request event to the target stub.
     *
     * \param   stubEvent       The service request event to deliver.
     * \return  True if the event was successfully delivered or queued; false otherwise.
     **/
    bool deliver_service_event( ServiceRequestEvent & stubEvent ) const;

    /**
     * \brief   Delivers a service response event to the target proxy.
     *
     * \param   proxyEvent      The service response event to deliver.
     * \return  True if the event was successfully delivered or queued; false otherwise.
     * \note    For remote events, return value indicates queuing success, not reception by target.
     **/
    bool deliver_service_event( ServiceResponseEvent & proxyEvent ) const;

    /**
     * \brief   Converts the proxy address to a path string.
     *
     * \return  Path string containing process ID, thread name, role name, and service name.
     **/
    [[nodiscard]]
    String to_string() const;

    /**
     * \brief   Parses a proxy path string and initializes this address from it.
     *
     * \param       pathProxy   The proxy path string to parse.
     * \param[out]  nextPart    If not null, receives pointer to remaining unparsed data.
     **/
    void from_string(const char * pathProxy, const char** nextPart = nullptr);

    /**
     * \brief   Initialize proxy address data from shared service identity and endpoint fields (consumer side).
     *
     * \param   rawService  Shared service interface identity (role/service hash, type).
     * \param   endPoint    Consumer endpoint carrying thread, version, and routing fields.
     **/
    inline void from_endpoint(const areg::RawService& rawService, const areg::Endpoint& endPoint) noexcept;

    /**
     * \brief   Write proxy address data into shared service identity and endpoint fields (consumer side).
     *
     * \param   rawService  Receives role/service hash and type.
     * \param   endPoint    Receives thread, version, routing, and magic number.
     **/
    inline void to_endpoint(areg::RawService& rawService, areg::Endpoint& endPoint) const noexcept;

    /**
     * \brief   Initialize proxy address data from an EventHeader (consumer field).
     **/
    inline void from_event(const areg::EventHeader& header) noexcept;

    /**
     * \brief   Write proxy address data into an EventHeader (consumer field).
     **/
    inline void to_event(areg::EventHeader& header) const noexcept;

    /**
     * \brief   Reads ServiceAddress data from an input stream.
     *
     * \param   stream      The input stream to read from.
     **/
    inline const InStream& from_stream(const InStream& stream);

    /**
     * \brief   Writes ServiceAddress data to an output stream.
     *
     * \param   stream      The output stream to write to.
     **/
    inline OutStream& to_stream(OutStream& stream) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    [[nodiscard]]
    inline ProxyAddress& self() noexcept;
    /**
     * \brief   Delivers a service event to a target.
     *
     * \param   serviceEvent    The service event to deliver.
     * \param   idTarget        The ID of the target service.
     * \return  True if delivery succeeded; false otherwise.
     **/
    static bool _deliver_event( Event & serviceEvent, const ITEM_ID & idTarget );

    /**
     * \brief   Computes a hash value for a proxy address.
     *
     * \param   proxy       The proxy address to hash.
     * \return  Hash value of the proxy address.
     **/
    [[nodiscard]]
    static uint32_t _magic_number( const ProxyAddress & proxy ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Thread address of Proxy.
     **/
    ThreadAddress   mThread;
    /**
     * \brief   Communication channel of Proxy.
     **/
    Channel         mChannel;
    /**
     * \brief   The calculated hash of proxy address.
     **/
    uint32_t        mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// ProxyAddress class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool ProxyAddress::operator == ( const StubAddress & addrStub ) const noexcept
{
    return is_provider_compatible(addrStub);
}

inline ProxyAddress & ProxyAddress::operator = ( const ProxyAddress & source )
{
    if (this != &source)
    {
        static_cast<ServiceAddress &>(*this) = static_cast<const ServiceAddress &>(source);
        mThread     = source.mThread;
        mChannel    = source.mChannel;
        mMagicNum   = source.mMagicNum;
    }

    return (*this);
}

inline ProxyAddress & ProxyAddress::operator = ( ProxyAddress && source ) noexcept
{
    if ( this != &source )
    {
        static_cast<ServiceAddress &>(*this) = static_cast<ServiceAddress &&>(source);
        mThread     = std::move(source.mThread);
        mChannel    = std::move(source.mChannel);
        mMagicNum   = source.mMagicNum;
        source.mMagicNum = areg::CHECKSUM_IGNORE;
    }

    return (*this);
}

inline bool ProxyAddress::operator == ( const ProxyAddress & other ) const noexcept
{
    return (mMagicNum == other.mMagicNum) && (mChannel.cookie() == other.mChannel.cookie());
}

inline bool ProxyAddress::operator != ( const ProxyAddress & other ) const noexcept
{
    return (mMagicNum != other.mMagicNum) || (mChannel.cookie() != other.mChannel.cookie());
}

inline ProxyAddress::operator uint32_t() const noexcept
{
    return mMagicNum;
}

inline bool ProxyAddress::is_local_address() const noexcept
{
    return (mChannel.cookie() == areg::COOKIE_LOCAL);
}

inline bool ProxyAddress::is_remote_address() const noexcept
{
    return (mChannel.cookie() >= areg::COOKIE_ANY);
}

inline bool ProxyAddress::is_source_local() const noexcept
{
    return (mChannel.cookie() == areg::COOKIE_LOCAL) && (mChannel.source() != 0);
}

inline bool ProxyAddress::is_source_public() const noexcept
{
    return (mChannel.cookie( ) >= areg::COOKIE_REMOTE_SERVICE) && (mChannel.source( ) != 0);
}

inline bool ProxyAddress::is_target_local() const noexcept
{
    return (mChannel.cookie( ) == areg::COOKIE_LOCAL) && (mChannel.target( ) != 0);
}

inline bool ProxyAddress::is_target_public() const noexcept
{
    return (mChannel.cookie( ) >= areg::COOKIE_LOCAL) && (mChannel.target( ) != 0);
}

inline const ThreadAddress & ProxyAddress::thread() const noexcept
{
    return mThread;
}

inline const Channel & ProxyAddress::channel() const noexcept
{
    return mChannel;
}

inline void ProxyAddress::set_channel( const Channel & channel ) noexcept
{
    mChannel = channel;
}

inline ITEM_ID ProxyAddress::cookie() const noexcept
{
    return mChannel.cookie();
}

inline void ProxyAddress::set_cookie(const ITEM_ID & cookie ) noexcept
{
    mChannel.set_cookie(cookie);
}

inline ITEM_ID ProxyAddress::source() const noexcept
{
    return mChannel.source();
}

inline void ProxyAddress::set_source(const ITEM_ID & source ) noexcept
{
    mChannel.set_source(source);
}

inline ITEM_ID ProxyAddress::target() const noexcept
{
    return mChannel.target();
}

inline void ProxyAddress::set_target(const ITEM_ID & target ) noexcept
{
    mChannel.set_target(target);
}

inline ProxyAddress& ProxyAddress::self() noexcept
{
    return (*this);
}

inline bool ProxyAddress::is_valid() const noexcept
{
    return areg::crc32_valid(mMagicNum) && mChannel.is_valid();
}

inline void ProxyAddress::invalidate_channel() noexcept
{
    mChannel.invalidate();
}

inline void ProxyAddress::from_endpoint(const areg::RawService& rawService, const areg::Endpoint& endPoint) noexcept
{
    ServiceAddress::from_endpoint(rawService, endPoint);
    mThread.from_endpoint(endPoint);
    mChannel.set_cookie(endPoint.id);
    mChannel.set_source(endPoint.thread);
}

inline void ProxyAddress::to_endpoint(areg::RawService& rawService, areg::Endpoint& endPoint) const noexcept
{
    ServiceAddress::to_endpoint(rawService, endPoint);
    mThread.to_endpoint(endPoint);
    endPoint.id     = mChannel.cookie();
    endPoint.number = mMagicNum;
}

inline void ProxyAddress::from_event(const areg::EventHeader& header) noexcept
{
    from_endpoint(header.rawService, header.consumer);
}

inline void ProxyAddress::to_event(areg::EventHeader& header) const noexcept
{
    to_endpoint(header.rawService, header.consumer);
}

inline const InStream& ProxyAddress::from_stream(const InStream& stream)
{
    ITEM_ID cookie = areg::COOKIE_LOCAL;
    ServiceAddress::from_stream(stream);
    stream >> mMagicNum;
    stream >> mThread;
    stream >> cookie;
    set_cookie(cookie);

    return stream;
}

inline OutStream& ProxyAddress::to_stream(OutStream& stream) const
{
    ServiceAddress::to_stream(stream);
    stream << mMagicNum;
    stream << mThread;
    stream << cookie();
    return stream;
}

inline const InStream & operator >> (const InStream & stream, ProxyAddress & input)
{
    return input.from_stream(stream);
}

inline OutStream & operator << (OutStream & stream, const ProxyAddress & output)
{
    return output.to_stream(stream);
}

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Hasher of ProxyAddress class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ProxyAddress.
 */
namespace std {
    template<>
    struct hash<areg::ProxyAddress>
    {
        //! A function to convert ProxyAddress object to uint32_t.
        inline uint32_t operator()(const areg::ProxyAddress& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
} // namespace std

#endif  // AREG_COMPONENT_PROXYADDRESS_HPP
