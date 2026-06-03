#ifndef AREG_COMPONENT_STUBADDRESS_HPP
#define AREG_COMPONENT_STUBADDRESS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/StubAddress.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Stub Address class.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/ServiceAddress.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/component/Channel.hpp"

#include <utility>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class ProxyAddress;
    class ServiceRequestEvent;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// StubAddress class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Structured address for stub (server) objects, identifying a service provider within the
 *          system for receiving service request events.
 **/
class AREG_API StubAddress    : public    ServiceAddress
{
//////////////////////////////////////////////////////////////////////////
// Static operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts a stub address to a path string.
     *
     * \param   addrStub    The stub address to convert.
     * \return  Path string containing stub address information.
     **/
    [[nodiscard]]
    static String to_path( const StubAddress & addrStub );

    /**
     * \brief   Parses a stub path string and creates a stub address from it.
     *
     * \param       pathStub    The stub path string to parse.
     * \param[out]  nextPart    If not null, receives pointer to remaining unparsed data.
     * \return  Parsed stub address object.
     **/
    [[nodiscard]]
    static StubAddress from_path(const char* pathStub, const char** nextPart = nullptr);

    /**
     * \brief   Returns a predefined invalid stub address for validation.
     **/
    [[nodiscard]]
    static const StubAddress & invalid_stub_address();

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    StubAddress();

    StubAddress(StubAddress&& source) noexcept;

    /**
     * \brief   Creates a stub address from service details and component role name.
     *
     * \param   serviceName         The name of the implemented service interface.
     * \param   serviceVersion      The version of the implemented service interface.
     * \param   serviceType         The type of service.
     * \param   roleName            The role name of the holder component.
     * \param   threadName          The thread name of the stub. If empty, uses the current thread.
     **/
    StubAddress( const String & serviceName
               , const Version & serviceVersion
               , areg::ServiceType serviceType
               , const String & roleName
               , const String & threadName = String::empty_string() );

    /**
     * \brief   Creates a stub address from service details and component role name.
     *
     * \param   serviceNum          The name of the implemented service interface.
     * \param   serviceVersion      The version of the implemented service interface.
     * \param   serviceType         The type of service.
     * \param   roleNum             The role name of the holder component.
     * \param   threadNum           The unique thread number of the stub. If zero, uses the current thread.
     * \param   stubNum             The unique number of the stub.
     **/
    StubAddress( const UniqueNumber serviceNum
               , const Version & serviceVersion
               , areg::ServiceType serviceType
               , const UniqueNumber roleNum
               , const UniqueNumber threadNum
               , const UniqueNumber stubNum = areg::CHECKSUM_IGNORE);

    /**
     * \brief   Creates a stub address from a service item and component role name.
     *
     * \param   service         Service item containing basic service information.
     * \param   roleName        The role name of the holder component.
     * \param   threadName      The thread name of the stub. If empty, uses the current thread.
     **/
    StubAddress( const ServiceItem & service, const String & roleName, const String & threadName = String::empty_string() );

    /**
     * \brief   Creates a stub address from a service item and component role number.
     *
     * \param   service         Service item containing basic service address.
     * \param   roleNum         The role number of the holder component.
     * \param   threadNum       The thread number of the stub. If zero, uses the current thread.
     * \param   stubNum         The unique number of the stub.
     **/
    StubAddress(const ServiceAddress& service, const UniqueNumber threadNum, const UniqueNumber stubNum = areg::CHECKSUM_IGNORE);

    /**
     * \brief   Creates a stub address from service interface data and component role name.
     *
     * \param   siData          Service interface data containing basic service information.
     * \param   roleName        The role name of the holder component.
     * \param   threadName      The thread name of the stub. If empty, uses the current thread.
     **/
    StubAddress( const areg::InterfaceData & siData, const String & roleName, const String & threadName = String::empty_string() );

    /**
     * \brief   Creates a stub address from shared service identity and endpoint fields.
     *
     * \param   rawService  Shared service interface identity.
     * \param   endPoint    Provider endpoint (provider field of EventHeader).
     **/
    StubAddress(const areg::RawService& rawService, const areg::Endpoint& endPoint);

    /**
     * \brief   Creates a stub address from an EventHeader (provider field).
     **/
    StubAddress(const areg::EventHeader& header);

    /**
     * \brief   Creates a stub address by copying a service address.
     * \param   source      The service address to copy.
     **/
    explicit StubAddress( const ServiceAddress & source );

    explicit StubAddress(const ProxyAddress& proxy);

    /**
     * \brief   Creates a stub address by moving a service address.
     * \param   source      The service address to move.
     **/
    explicit StubAddress( ServiceAddress && source );

    StubAddress(const StubAddress& source) = default;

    virtual ~StubAddress() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief   Copies a stub address.
     *
     * \param   source      The source stub address to copy.
     * \return  Reference to this stub address.
     **/
    inline StubAddress & operator = ( const StubAddress & source );

    /**
     * \brief   Moves a stub address.
     *
     * \param   source      The source stub address to move.
     * \return  Reference to this stub address.
     **/
    inline StubAddress & operator = ( StubAddress && source ) noexcept;

    /**
     * \brief   Copies stub address data from a service address.
     *
     * \param   addrService     The service address to copy from.
     * \return  Reference to this stub address.
     **/
    inline StubAddress & operator = ( const ServiceAddress & addrService );

    /**
     * \brief   Moves stub address data from a service address.
     *
     * \param   addrService     The service address to move from.
     * \return  Reference to this stub address.
     **/
    inline StubAddress & operator = ( ServiceAddress && addrService ) noexcept;

    /**
     * \brief   Returns true if two stub addresses are equal.
     *
     * \param   other       The stub address to compare.
     **/
    [[nodiscard]]
    inline bool operator == ( const StubAddress & other ) const noexcept;

    /**
     * \brief   Returns true if a proxy address is compatible with this stub address.
     *
     * \param   addrProxy       The proxy address to check for compatibility.
     **/
    [[nodiscard]]
    inline bool operator == ( const ProxyAddress & addrProxy ) const noexcept;

    /**
     * \brief   Returns true if two stub addresses are not equal.
     *
     * \param   other       The stub address to compare.
     **/
    [[nodiscard]]
    inline bool operator != ( const StubAddress & other ) const noexcept;

    /**
     * \brief   Converts the stub address to a 32-bit hash value.
     **/
    [[nodiscard]]
    inline explicit operator uint32_t () const noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the stub address is for a local service.
     **/
    [[nodiscard]]
    inline bool is_local_address() const noexcept;

    /**
     * \brief   Returns true if the stub address is for a remote service.
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
     * \brief   Returns the communication channel of this stub.
     **/
    [[nodiscard]]
    inline const Channel & channel() const noexcept;

    /**
     * \brief   Sets the communication channel for this stub.
     *
     * \param   channel     The channel to set.
     **/
    inline void set_channel( const Channel & channel ) noexcept;

    /**
     * \brief   Returns the cookie value of this stub.
     **/
    [[nodiscard]]
    inline ITEM_ID cookie() const noexcept;

    /**
     * \brief   Sets the cookie value for this stub.
     *
     * \param   cookie      The cookie value to set.
     **/
    inline void set_cookie(const ITEM_ID & cookie ) noexcept;

    /**
     * \brief   Returns the source ID set in the communication channel.
     **/
    [[nodiscard]]
    inline ITEM_ID source() const noexcept;

    /**
     * \brief   Sets the source ID in the communication channel.
     *
     * \param   source      The source ID to set.
     **/
    inline void set_source(const ITEM_ID & source ) noexcept;

    /**
     * \brief   Returns the thread address of the service owner.
     **/
    [[nodiscard]]
    inline const ThreadAddress & thread() const noexcept;

    /**
     * \brief   Sets the thread address of the service owner.
     *
     * \param   thread      The thread address to set.
     **/
    void set_thread( const ThreadAddress& thread );

    /**
     * \brief   Sets the thread name to the thread address of the service owner.
     *
     * \param   thread      The thread name to set.
     **/
    void set_thread(const String& thread);

    /**
     * \brief   Returns true if the stub address is valid.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Marks the communication channel as invalid.
     **/
    inline void invalidate_channel() noexcept;

    /**
     * \brief   Returns true if the specified proxy address is compatible with this stub.
     *
     * \param   proxyAddress    The proxy address to check for compatibility.
     * \return  True if the proxy is compatible; false otherwise.
     **/
    [[nodiscard]]
    bool is_proxy_compatible( const ProxyAddress & proxyAddress ) const noexcept;

    /**
     * \brief   Delivers a service request event to the target.
     *
     * \param   serviceEvent    The service event to deliver.
     * \return  True if the event was successfully delivered or queued; false otherwise.
     * \note    For remote events, return value indicates queueing success, not reception by target.
     **/
    bool deliver_service_event( ServiceRequestEvent & serviceEvent ) const;

    /**
     * \brief   Converts the stub address to a path string.
     *
     * \return  Path string containing stub address information.
     **/
    [[nodiscard]]
    String to_string() const;

    /**
     * \brief   Parses a stub path string and initializes this address from it.
     *
     * \param   pathStub        The stub path string to parse.
     * \param[out] nextPart    If not null, receives pointer to remaining unparsed data.
     **/
    void from_string(const char* pathStub, const char** nextPart = nullptr);

    /**
     * \brief   Initialize stub address data from shared service identity and endpoint fields.
     *
     * \param   rawService  Shared service interface identity (role/service hash, type).
     * \param   endPoint    Provider endpoint carrying thread, version, and routing fields.
     **/
    inline void from_endpoint(const areg::RawService& rawService, const areg::Endpoint& endPoint) noexcept;

    /**
     * \brief   Write stub address data into shared service identity and endpoint fields.
     *
     * \param   rawService  Receives role/service hash and type.
     * \param   endPoint    Receives thread, version, routing, and magic number.
     **/
    inline void to_endpoint(areg::RawService& rawService, areg::Endpoint& endPoint) const noexcept;

    /**
     * \brief   Initialize stub address data from an EventHeader (provider field).
     **/
    inline void from_event(const areg::EventHeader& header) noexcept;

    /**
     * \brief   Write stub address data into an EventHeader (provider field).
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

private:
    [[nodiscard]]
    inline StubAddress& self() noexcept;
    /**
     * \brief   Computes a hash value for a stub address.
     *
     * \param   addrStub    The stub address to hash.
     * \return  Hash value of the stub address.
     **/
    [[nodiscard]]
    static uint32_t _magic_number( const StubAddress & addrStub ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The name of owner thread.
     **/
    ThreadAddress   mThread;
    /**
     * \brief   The communication channel.
     **/
    Channel         mChannel;

    /**
     * \brief   The calculated number of stub address.
     **/
    uint32_t        mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// StubAddress class inline functions implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

inline StubAddress & StubAddress::operator = ( const StubAddress & source )
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

inline StubAddress & StubAddress::operator = ( StubAddress && source ) noexcept
{
    if ( this != &source )
    {
        static_cast<ServiceAddress &>(*this) = static_cast<ServiceAddress &&>(source);
        mThread     = std::move(source.mThread);
        mChannel    = std::move(source.mChannel);
        mMagicNum   = source.mMagicNum;
    }

    return (*this);
}

inline StubAddress & StubAddress::operator = (const ServiceAddress & addrService)
{
    if ( static_cast<const ServiceAddress *>(this) != &addrService)
    {
        static_cast<ServiceAddress &>(*this) = static_cast<const ServiceAddress &>(addrService);
        mThread     = ThreadAddress::invalid_thread_address();
        mChannel    = Channel();
        mMagicNum   = areg::INVALID_VALUE;
    }

    return (*this);
}

inline StubAddress & StubAddress::operator = ( ServiceAddress && addrService ) noexcept
{
    if ( static_cast<const ServiceAddress *>(this) != &addrService )
    {
        static_cast<ServiceAddress &>(*this) = static_cast<ServiceAddress &&>(addrService);
        mThread     = ThreadAddress::invalid_thread_address();
        mChannel    = Channel( );
        mMagicNum   = areg::INVALID_VALUE;
    }

    return (*this);
}

inline bool StubAddress::operator == ( const StubAddress & other ) const noexcept
{
    return (mMagicNum == other.mMagicNum) && (mChannel.cookie() == other.mChannel.cookie());
}

inline bool StubAddress::operator != ( const StubAddress& other ) const noexcept
{
    return (mMagicNum != other.mMagicNum) || (mChannel.cookie() != other.mChannel.cookie());
}

inline bool StubAddress::operator == ( const ProxyAddress & addrProxy ) const noexcept
{
    return is_proxy_compatible(addrProxy);
}

inline StubAddress::operator uint32_t () const noexcept
{
    return mMagicNum;
}

inline bool StubAddress::is_valid() const noexcept
{
    return areg::crc32_valid(mMagicNum) && mChannel.is_valid();
}

inline bool StubAddress::is_local_address() const noexcept
{
    return mChannel.cookie() == areg::COOKIE_LOCAL;
}

inline bool StubAddress::is_remote_address() const noexcept
{
    return (mChannel.cookie() >= areg::COOKIE_ANY);
}

inline bool StubAddress::is_source_local() const noexcept
{
    return (mChannel.cookie( ) == areg::COOKIE_LOCAL) && (mChannel.source( ) != 0);
}

inline bool StubAddress::is_source_public() const noexcept
{
    return (mChannel.cookie( ) >= areg::COOKIE_REMOTE_SERVICE) && (mChannel.source( ) != 0);
}

inline bool StubAddress::is_target_local() const noexcept
{
    return (mChannel.cookie( ) == areg::COOKIE_LOCAL) && (mChannel.target( ) != 0);
}

inline bool StubAddress::is_target_public() const noexcept
{
    return (mChannel.cookie( ) >= areg::COOKIE_LOCAL) && (mChannel.target( ) != 0);
}

inline const ThreadAddress & StubAddress::thread() const noexcept
{
    return mThread;
}

inline const Channel & StubAddress::channel() const noexcept
{
    return mChannel;
}

inline void StubAddress::set_channel(const Channel & channel) noexcept
{
    mChannel = channel;
}

inline ITEM_ID StubAddress::cookie() const noexcept
{
    return mChannel.cookie();
}

inline void StubAddress::set_cookie(const ITEM_ID & cookie ) noexcept
{
    mChannel.set_cookie(cookie);
}

inline ITEM_ID StubAddress::source() const noexcept
{
    return mChannel.source();
}

inline void StubAddress::set_source(const ITEM_ID & source ) noexcept
{
    mChannel.set_source(source);
}

inline void StubAddress::from_endpoint(const areg::RawService& rawService, const areg::Endpoint& endPoint) noexcept
{
    ServiceAddress::from_endpoint(rawService, endPoint);
    mThread.from_endpoint(endPoint);
    mChannel.set_cookie(endPoint.id);
    mChannel.set_source(endPoint.thread);
}

inline void StubAddress::to_endpoint(areg::RawService& rawService, areg::Endpoint& endPoint) const noexcept
{
    ServiceAddress::to_endpoint(rawService, endPoint);
    mThread.to_endpoint(endPoint);
    endPoint.id     = mChannel.cookie();
    endPoint.number = mMagicNum;
}

inline void StubAddress::from_event(const areg::EventHeader& header) noexcept
{
    from_endpoint(header.rawService, header.provider);
}

inline void StubAddress::to_event(areg::EventHeader& header) const noexcept
{
    to_endpoint(header.rawService, header.provider);
}

inline StubAddress& StubAddress::self() noexcept
{
    return (*this);
}

inline const InStream& StubAddress::from_stream(const InStream& stream)
{
    ITEM_ID cookie = areg::COOKIE_LOCAL;
    ServiceAddress::from_stream(stream);
    stream >> mMagicNum;
    stream >> mThread;
    stream >> cookie;
    set_cookie(cookie);

    return stream;
}

inline OutStream& StubAddress::to_stream(OutStream& stream) const
{
    ServiceAddress::to_stream(stream);
    stream << mMagicNum;
    stream << mThread;
    stream << cookie();
    return stream;
}

inline const InStream & operator >> (const InStream & stream, StubAddress & input)
{
    return input.from_stream(stream);
}

inline OutStream & operator << (OutStream & stream, const StubAddress & output)
{
    return output.to_stream(stream);
}

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Hasher of StubAddress class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the StubAddress.
 */
namespace std {
    //! Calculates the hash value of the StubAddress object
    template<> struct hash<areg::StubAddress>
    {
        //! A function operator to convert StubAddress object to hash value.
        inline uint32_t operator()(const areg::StubAddress& key) const noexcept
        {
            return static_cast<uint32_t>(key);
        }
    };

    //!< Compares 2 StubAddress objects
    template<> struct equal_to<areg::StubAddress>
    {
        //! A function operator to compare 2 StubAddress objects.
        inline bool operator() (const areg::StubAddress& key1, const areg::StubAddress& key2) const noexcept
        {
            return static_cast<const areg::ServiceAddress&>(key1) == static_cast<const areg::ServiceAddress&>(key2);
        }
    };
} // namespace std

#endif  // AREG_COMPONENT_STUBADDRESS_HPP
