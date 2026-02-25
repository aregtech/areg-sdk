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
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
#include "areg/base/GEGlobal.h"
#include "areg/component/ServiceDefs.hpp"
#include "areg/component/ServiceAddress.hpp"
#include "areg/component/Channel.hpp"

#include <utility>

/************************************************************************
 * Dependencies
 ************************************************************************/
class InStream;
class StubAddress;
class Event;
class ServiceRequestEvent;
class ServiceResponseEvent;

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
    static String to_path( const ProxyAddress & addrProxy );

    /**
     * \brief   Parses a proxy path string and creates a proxy address from it.
     *
     * \param   pathProxy       The proxy path string to parse.
     * \param[out] out_nextPart    If not null, receives pointer to remaining unparsed data in the
     *                             path string.
     * \return  Parsed proxy address object.
     **/
    static ProxyAddress from_path(const char * pathProxy, const char** out_nextPart = nullptr);

    /**
     * \brief   Returns a predefined invalid proxy address for validation.
     **/
    static const ProxyAddress & invalid_proxy_address();

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Default constructor. Creates an invalid proxy address.
     **/
    ProxyAddress();

    /**
     * \brief   Creates a proxy address from service details and component role name.
     *
     * \param   serviceName         The name of the service.
     * \param   serviceVersion      The version of the service.
     * \param   serviceType         The type of service (local or remote).
     * \param   roleName            The role name of the component.
     * \param   threadName          The name of the thread where the proxy acts. If empty, uses the
     *                              current thread.
     **/
    ProxyAddress( const String & serviceName
                , const Version & serviceVersion
                , NEService::ServiceType serviceType
                , const String & roleName
                , const String & threadName = String::empty_string() );
    /**
     * \brief   Creates a proxy address from a service item and component role name.
     *
     * \param   service         Service item containing name, version, and type.
     * \param   roleName        The role name of the component.
     * \param   threadName      The name of the thread where the proxy acts. If empty, uses the
     *                          current thread.
     **/
    ProxyAddress( const ServiceItem & service, const String & roleName, const String & threadName = String::empty_string() );
    /**
     * \brief   Creates a proxy address from service interface data and component role name.
     *
     * \param   siData          Service interface data containing service information.
     * \param   roleName        The role name of the component.
     * \param   threadName      The name of the thread where the proxy acts. If empty, uses the
     *                          current thread.
     **/
    ProxyAddress( const NEService::InterfaceData & siData, const String & roleName, const String & threadName = String::empty_string() );

    /**
     * \brief
     *
     * \param   source      The source proxy address to copy.
     **/
    ProxyAddress( const ProxyAddress & source );

    /**
     * \brief
     *
     * \param   source      The source proxy address to move.
     * \note    Move overload. Takes ownership of the source.
     **/
    ProxyAddress( ProxyAddress && source ) noexcept;

    /**
     * \brief   Creates a proxy address by copying a service address.
     *
     * \param   source      The service address to copy.
     **/
    explicit ProxyAddress(const ServiceAddress & source);

    /**
     * \brief   Creates a proxy address by moving a service address.
     *
     * \param   source      The service address to move.
     **/
    explicit ProxyAddress(ServiceAddress && source);

    /**
     * \brief   Creates a proxy address by reading from a stream.
     *
     * \param   stream      The input stream to read from.
     **/
    ProxyAddress(const InStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~ProxyAddress() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief   Copies a proxy address.
     *
     * \param   source      The source proxy address to copy.
     * \return  Reference to this proxy address.
     **/
    inline ProxyAddress & operator = ( const ProxyAddress & source );

    /**
     * \brief   Moves a proxy address.
     *
     * \param   source      The source proxy address to move.
     * \return  Reference to this proxy address.
     **/
    inline ProxyAddress & operator = ( ProxyAddress && source ) noexcept;

    /**
     * \brief   Returns true if two proxy addresses are equal.
     *
     * \param   other       The proxy address to compare.
     **/
    inline bool operator == ( const ProxyAddress & other ) const;

    /**
     * \brief   Returns true if a stub address is compatible with this proxy address.
     *
     * \param   addrStub    The stub address to check for compatibility.
     **/
    inline bool operator == (const StubAddress & addrStub ) const;

    /**
     * \brief   Returns true if two proxy addresses are not equal.
     *
     * \param   other       The proxy address to compare.
     **/
    inline bool operator != ( const ProxyAddress & other ) const;

    /**
     * \brief   Converts the proxy address to a 32-bit hash value.
     **/
    inline explicit operator uint32_t () const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/

    /**
     * \brief   Reads and initializes a proxy address from a stream.
     *
     * \param   stream      The input stream.
     * \param[out] input       The proxy address to initialize from stream data.
     **/
    friend AREG_API const InStream & operator >> ( const InStream & stream, ProxyAddress & input );

    /**
     * \brief   Writes a proxy address to a stream.
     *
     * \param   stream      The output stream.
     * \param   output      The proxy address to serialize.
     **/
    friend AREG_API OutStream & operator << ( OutStream & stream, const ProxyAddress & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if the proxy address is for a local service.
     **/
    inline bool is_local_address() const;

    /**
     * \brief   Returns true if the proxy address is for a remote service.
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
     * \brief   Returns the thread name associated with this proxy.
     **/
    inline const String & thread() const;
    /**
     * \brief   Sets the thread name for this proxy.
     *
     * \param   threadName      The thread name to set.
     **/
    void set_thread( const String & threadName );
    /**
     * \brief   Returns the communication channel of this proxy.
     **/
    inline const Channel & channel() const;
    /**
     * \brief   Sets the communication channel for this proxy.
     *
     * \param   channel     The channel to set.
     **/
    inline void set_channel( const Channel & channel );
    /**
     * \brief   Returns the cookie value of this proxy.
     **/
    inline const ITEM_ID & cookie() const;
    /**
     * \brief   Sets the cookie value for this proxy.
     *
     * \param   cookie      The cookie value to set.
     **/
    inline void set_cookie(const ITEM_ID & cookie );
    /**
     * \brief   Returns the source ID of this proxy.
     **/
    inline const ITEM_ID & source() const;
    /**
     * \brief   Sets the source ID for this proxy.
     *
     * \param   source      The source ID to set.
     **/
    inline void set_source(const ITEM_ID & source );
    /**
     * \brief   Returns the target ID of this proxy.
     **/
    inline const ITEM_ID & target() const;
    /**
     * \brief   Sets the target ID for this proxy.
     *
     * \param   target      The target ID to set.
     **/
    inline void set_target(const ITEM_ID & target);

    /**
     * \brief   Returns true if the proxy address is valid.
     **/
    bool is_valid() const;

    /**
     * \brief   Marks the communication channel as invalid.
     **/
    void invalidate_channel();

    /**
     * \brief   Returns true if the specified stub address is compatible with this proxy.
     *
     * \param   addrStub    The stub address to check for compatibility.
     * \return  True if the stub address is compatible.
     **/
    bool is_stub_compatible( const StubAddress & addrStub ) const;

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
     * \note    For remote events, return value indicates queueing success, not reception by target.
     **/
    bool deliver_service_event( ServiceResponseEvent & proxyEvent ) const;

    /**
     * \brief   Converts the proxy address to a path string.
     *
     * \return  Path string containing process ID, thread name, role name, and service name.
     **/
    String to_string() const;

    /**
     * \brief   Parses a proxy path string and initializes this address from it.
     *
     * \param   pathProxy       The proxy path string to parse.
     * \param[out] out_nextPart    If not null, receives pointer to remaining unparsed data.
     **/
    void conv_from_string(const char * pathProxy, const char** out_nextPart = nullptr);

protected:
    /**
     * \brief   Returns true if the proxy address data is valid.
     **/
    bool is_validated() const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief
     **/
    inline ProxyAddress& self();
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
    static uint32_t _magic_number( const ProxyAddress & proxy );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Thread name of Proxy
     **/
    String          mThreadName;
    /**
     * \brief   Communication channel of Proxy.
     **/
    Channel         mChannel;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The calculated number of proxy address
     **/
    uint32_t    mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of ProxyAddress class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ProxyAddress.
 */
namespace std
{
    template<>
    struct hash<ProxyAddress>
    {
        //! A function to convert ProxyAddress object to uint32_t.
        inline uint32_t operator()(const ProxyAddress& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// ProxyAddress class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool ProxyAddress::operator == ( const StubAddress & addrStub ) const
{
    return is_stub_compatible(addrStub);
}

inline ProxyAddress & ProxyAddress::operator = ( const ProxyAddress & source )
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

inline ProxyAddress & ProxyAddress::operator = ( ProxyAddress && source )noexcept
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

inline bool ProxyAddress::operator == ( const ProxyAddress & other ) const
{
    return (mMagicNum == other.mMagicNum) && (mChannel.cookie() == other.mChannel.cookie());
}

inline bool ProxyAddress::operator != ( const ProxyAddress & other ) const
{
    return (mMagicNum != other.mMagicNum) || (mChannel.cookie() != other.mChannel.cookie());
}

inline ProxyAddress::operator uint32_t() const
{
    return mMagicNum;
}

inline bool ProxyAddress::is_local_address() const
{
    return (mChannel.cookie() == NEService::COOKIE_LOCAL);
}

inline bool ProxyAddress::is_remote_address() const
{
    return (mChannel.cookie() >= NEService::COOKIE_ANY);
}

inline bool ProxyAddress::is_source_local() const
{
    return (mChannel.cookie() == NEService::COOKIE_LOCAL) && (mChannel.source() != 0);
}

inline bool ProxyAddress::is_source_public() const
{
    return (mChannel.cookie( ) >= NEService::COOKIE_REMOTE_SERVICE) && (mChannel.source( ) != 0);
}

inline bool ProxyAddress::is_target_local() const
{
    return (mChannel.cookie( ) == NEService::COOKIE_LOCAL) && (mChannel.target( ) != 0);
}

inline bool ProxyAddress::is_target_public() const
{
    return (mChannel.cookie( ) >= NEService::COOKIE_LOCAL) && (mChannel.target( ) != 0);
}

inline const String & ProxyAddress::thread() const
{
    return mThreadName;
}

inline const Channel & ProxyAddress::channel() const
{
    return mChannel;
}

inline void ProxyAddress::set_channel( const Channel & channel )
{
    mChannel = channel;
}

inline const ITEM_ID & ProxyAddress::cookie() const
{
    return mChannel.cookie();
}

inline void ProxyAddress::set_cookie(const ITEM_ID & cookie )
{
    mChannel.set_cookie(cookie);
}

inline const ITEM_ID & ProxyAddress::source() const
{
    return mChannel.source();
}

inline void ProxyAddress::set_source(const ITEM_ID & source )
{
    return mChannel.set_source(source);
}

inline const ITEM_ID & ProxyAddress::target() const
{
    return mChannel.target();
}

inline void ProxyAddress::set_target(const ITEM_ID & target )
{
    return mChannel.set_target(target);
}

inline ProxyAddress& ProxyAddress::self()
{
    return (*this);
}

#endif  // AREG_COMPONENT_PROXYADDRESS_HPP
