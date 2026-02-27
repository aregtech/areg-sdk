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
#include "areg/base/GEGlobal.h"
#include "areg/component/ServiceAddress.hpp"
#include "areg/component/Channel.hpp"

#include <utility>

/************************************************************************
 * Dependencies
 ************************************************************************/
class InStream;
class ProxyAddress;
class ServiceRequestEvent;

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
    static String to_path( const StubAddress & addrStub );

    /**
     * \brief   Parses a stub path string and creates a stub address from it.
     *
     * \param   pathStub        The stub path string to parse.
     * \param[out] out_nextPart    If not null, receives pointer to remaining unparsed data.
     * \return  Parsed stub address object.
     **/
    static StubAddress from_path(const char* pathStub, const char** out_nextPart = nullptr);

    /**
     * \brief   Returns a predefined invalid stub address for validation.
     **/
    static const StubAddress & invalid_stub_address();

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates an invalid stub address.
     **/
    StubAddress();

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
               , NEService::ServiceType serviceType
               , const String & roleName
               , const String & threadName = String::empty_string() );

    /**
     * \brief   Creates a stub address from a service item and component role name.
     *
     * \param   service         Service item containing basic service information.
     * \param   roleName        The role name of the holder component.
     * \param   threadName      The thread name of the stub. If empty, uses the current thread.
     **/
    StubAddress( const ServiceItem & service, const String & roleName, const String & threadName = String::empty_string() );

    /**
     * \brief   Creates a stub address from service interface data and component role name.
     *
     * \param   siData          Service interface data containing basic service information.
     * \param   roleName        The role name of the holder component.
     * \param   threadName      The thread name of the stub. If empty, uses the current thread.
     **/
    StubAddress( const NEService::InterfaceData & siData, const String & roleName, const String & threadName = String::empty_string() );

    /**
     * \brief
     *
     * \param   source      The source stub address to copy.
     **/
    StubAddress( const StubAddress & source );

    /**
     * \brief
     *
     * \param   source      The source stub address to move.
     * \note    Move overload. Takes ownership of the source.
     **/
    StubAddress( StubAddress && source ) noexcept;

    /**
     * \brief   Creates a stub address by copying a service address.
     *
     * \param   source      The service address to copy.
     **/
    explicit StubAddress( const ServiceAddress & source );

    /**
     * \brief   Creates a stub address by moving a service address.
     *
     * \param   source      The service address to move.
     **/
    explicit StubAddress( ServiceAddress && source );

    /**
     * \brief   Creates a stub address by reading from a stream.
     *
     * \param   stream      The input stream to read from.
     **/
    StubAddress( const InStream & stream);

    /**
     * \brief   Destructor.
     **/
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
    inline bool operator == ( const StubAddress & other ) const;

    /**
     * \brief   Returns true if a proxy address is compatible with this stub address.
     *
     * \param   addrProxy       The proxy address to check for compatibility.
     **/
    inline bool operator == ( const ProxyAddress & addrProxy ) const;

    /**
     * \brief   Returns true if two stub addresses are not equal.
     *
     * \param   other       The stub address to compare.
     **/
    inline bool operator != ( const StubAddress & other ) const;

    /**
     * \brief   Converts the stub address to a 32-bit hash value.
     **/
    inline explicit operator uint32_t () const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/

    /**
     * \brief   Reads and initializes a stub address from a stream.
     *
     * \param   stream      The input stream.
     * \param[out] input       The stub address to initialize from stream data.
     **/
    friend AREG_API const InStream & operator >> ( const InStream & stream, StubAddress & input );

    /**
     * \brief   Writes a stub address to a stream.
     *
     * \param   stream      The output stream.
     * \param   output      The stub address to serialize.
     **/
    friend AREG_API OutStream & operator << ( OutStream & stream, const StubAddress & output);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the stub address is for a local service.
     **/
    inline bool is_local_address() const;

    /**
     * \brief   Returns true if the stub address is for a remote service.
     **/
    inline bool is_remote_address() const;

    /**
     * \brief   Returns true if the source of the communication channel is local (same process).
     **/
    inline bool is_source_local() const;

    /**
     * \brief   Returns true if the source of the communication channel is external (different
     *          process).
     **/
    inline bool is_source_public() const;

    /**
     * \brief   Returns true if the target of the communication channel is local (same process).
     **/
    inline bool is_target_local() const;

    /**
     * \brief   Returns true if the target of the communication channel is external (different
     *          process).
     **/
    inline bool is_target_public() const;

    /**
     * \brief   Returns the communication channel of this stub.
     **/
    inline const Channel & channel() const;

    /**
     * \brief   Sets the communication channel for this stub.
     *
     * \param   channel     The channel to set.
     **/
    inline void set_channel( const Channel & channel );

    /**
     * \brief   Returns the cookie value of this stub.
     **/
    inline const ITEM_ID & cookie() const;

    /**
     * \brief   Sets the cookie value for this stub.
     *
     * \param   cookie      The cookie value to set.
     **/
    inline void set_cookie(const ITEM_ID & cookie );

    /**
     * \brief   Returns the source ID set in the communication channel.
     **/
    inline const ITEM_ID & source() const;

    /**
     * \brief   Sets the source ID in the communication channel.
     *
     * \param   source      The source ID to set.
     **/
    inline void set_source(const ITEM_ID & source );

    /**
     * \brief   Returns the thread name of the service owner.
     **/
    inline const String & thread() const;

    /**
     * \brief   Sets the thread name of the service owner.
     *
     * \param   threadName      The thread name to set.
     **/
    void set_thread( const String & threadName );

    /**
     * \brief   Returns true if the stub address is valid.
     **/
    bool is_valid() const;

    /**
     * \brief   Marks the communication channel as invalid.
     **/
    void invalidate_channel();

    /**
     * \brief   Returns true if the specified proxy address is compatible with this stub.
     *
     * \param   proxyAddress    The proxy address to check for compatibility.
     * \return  True if the proxy is compatible; false otherwise.
     **/
    bool is_proxy_compatible( const ProxyAddress & proxyAddress ) const;

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
    String to_string() const;

    /**
     * \brief   Parses a stub path string and initializes this address from it.
     *
     * \param   pathStub        The stub path string to parse.
     * \param[out] out_nextPart    If not null, receives pointer to remaining unparsed data.
     **/
    void conv_from_string(const char* pathStub, const char** out_nextPart = nullptr);

protected:
    /**
     * \brief   Returns true if the stub address data is valid.
     **/
    bool is_validated() const;

private:
    inline StubAddress& self();
    /**
     * \brief   Computes a hash value for a stub address.
     *
     * \param   addrStub    The stub address to hash.
     * \return  Hash value of the stub address.
     **/
    static uint32_t _magic_number( const StubAddress & addrStub );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The name of owner thread.
     **/
    String          mThreadName;
    /**
     * \brief   The communication channel.
     **/
    Channel         mChannel;

private:
    /**
     * \brief   The calculated number of stub address.
     **/
    uint32_t    mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of StubAddress class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the StubAddress.
 */
namespace std
{
    //! Calculates the hash value of the StubAddress object
    template<> struct hash<StubAddress>
    {
        //! A function operator to convert StubAddress object to hash value.
        inline uint32_t operator()(const StubAddress& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };

    //!< Compares 2 StubAddress objects
    template<> struct equal_to<StubAddress>
    {
        //! A function operator to compare 2 StubAddress objects.
        inline bool operator() (const StubAddress& key1, const StubAddress& key2) const
        {
            return static_cast<const ServiceAddress&>(key1) == static_cast<const ServiceAddress&>(key2);
        }
    };
}

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
        mThreadName = source.mThreadName;
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
        mThreadName = std::move(source.mThreadName);
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
        mThreadName = String::empty_string();
        mChannel    = Channel();
        mMagicNum   = StubAddress::_magic_number(*this);
    }

    return (*this);
}

inline StubAddress & StubAddress::operator = ( ServiceAddress && addrService ) noexcept
{
    if ( static_cast<const ServiceAddress *>(this) != &addrService )
    {
        static_cast<ServiceAddress &>(*this) = static_cast<ServiceAddress &&>(addrService);
        mThreadName = String::empty_string();
        mChannel    = Channel( );
        mMagicNum   = StubAddress::_magic_number( *this );
    }

    return (*this);
}

inline bool StubAddress::operator == ( const StubAddress & other ) const
{
    return (mMagicNum == other.mMagicNum) && (mChannel.cookie() == other.mChannel.cookie());
}

inline bool StubAddress::operator != ( const StubAddress& other ) const
{
    return (mMagicNum != other.mMagicNum) || (mChannel.cookie() != other.mChannel.cookie());
}

inline bool StubAddress::operator == ( const ProxyAddress & addrProxy ) const
{
    return is_proxy_compatible(addrProxy);
}

inline StubAddress::operator uint32_t () const
{
    return mMagicNum;
}

inline bool StubAddress::is_local_address() const
{
    return mChannel.cookie() == NEService::COOKIE_LOCAL;
}

inline bool StubAddress::is_remote_address() const
{
    return (mChannel.cookie() >= NEService::COOKIE_ANY);
}

inline bool StubAddress::is_source_local() const
{
    return (mChannel.cookie( ) == NEService::COOKIE_LOCAL) && (mChannel.source( ) != 0);
}

inline bool StubAddress::is_source_public() const
{
    return (mChannel.cookie( ) >= NEService::COOKIE_REMOTE_SERVICE) && (mChannel.source( ) != 0);
}

inline bool StubAddress::is_target_local() const
{
    return (mChannel.cookie( ) == NEService::COOKIE_LOCAL) && (mChannel.target( ) != 0);
}

inline bool StubAddress::is_target_public() const
{
    return (mChannel.cookie( ) >= NEService::COOKIE_LOCAL) && (mChannel.target( ) != 0);
}

inline const String & StubAddress::thread() const
{
    return mThreadName;
}

inline const Channel & StubAddress::channel() const
{
    return mChannel;
}

inline void StubAddress::set_channel(const Channel & channel)
{
    mChannel = channel;
}

inline const ITEM_ID & StubAddress::cookie() const
{
    return mChannel.cookie();
}

inline void StubAddress::set_cookie(const ITEM_ID & cookie )
{
    mChannel.set_cookie(cookie);
}

inline const ITEM_ID & StubAddress::source() const
{
    return mChannel.source();
}

inline void StubAddress::set_source(const ITEM_ID & source )
{
    return mChannel.set_source(source);
}

inline StubAddress& StubAddress::self()
{
    return (*this);
}

#endif  // AREG_COMPONENT_STUBADDRESS_HPP
