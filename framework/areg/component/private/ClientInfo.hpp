#ifndef AREG_COMPONENT_PRIVATE_CLIENTINFO_HPP
#define AREG_COMPONENT_PRIVATE_CLIENTINFO_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ClientInfo.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Client Info class declaration
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/ServiceDefs.hpp"

namespace areg {
    class StubAddress;
} // namespace areg

namespace areg {
//////////////////////////////////////////////////////////////////////////
// ClientInfo class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Helper class for servicing interface clients; contains proxy address and client state.
 **/
class ClientInfo
{
//////////////////////////////////////////////////////////////////////////
// ClientInfo static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns a predefined invalid client info object.
     **/
    [[nodiscard]]
    static const ClientInfo & invalid_client_info() noexcept;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ClientInfo();

    /**
     * \brief   Initializes the object with the given proxy address.
     **/
    explicit ClientInfo( const ProxyAddress & client );

    /**
     * \brief   Initializes the object with the given proxy address.
     **/
    explicit ClientInfo( ProxyAddress && client ) noexcept;

    ClientInfo(const ClientInfo& src) = default;

    ClientInfo(ClientInfo&& src) noexcept;

    ~ClientInfo() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    inline ClientInfo& operator = (ClientInfo&& src) noexcept;

    inline ClientInfo& operator = (const ProxyAddress& client);

    inline ClientInfo & operator = ( ProxyAddress && client ) noexcept;

    [[nodiscard]]
    inline bool operator == ( const ClientInfo & other ) const noexcept;

    [[nodiscard]]
    inline bool operator == ( const ProxyAddress & client ) const noexcept;

    [[nodiscard]]
    inline bool operator == ( const StubAddress & server ) const noexcept;

    /**
     * \brief   Converts client info to a 32-bit unsigned integer.
     **/
    [[nodiscard]]
    explicit inline operator uint32_t () const noexcept;

    ClientInfo& operator = (const ClientInfo& src) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Sets the target service provider address.
     * \param   addrStub    The address of the service provider.
     **/
    void set_target( const StubAddress & addrStub );

    /**
     * \brief   Sets the client connection state based on the server stub address.
     *
     * \param   newConnection       The stub address of the server; valid address sets client to
     *                              Connected, otherwise to Waiting or Undefined.
     **/
    void set_connection_status( areg::ServiceConnectionState newConnection ) noexcept;

    /**
     * \brief   Returns the client connection state.
     **/
    [[nodiscard]]
    inline areg::ServiceConnectionState connection_status() const noexcept;

    /**
     * \brief   Returns the proxy address of the client.
     **/
    [[nodiscard]]
    inline const ProxyAddress & address() const noexcept;

    /**
     * \brief   Returns true if the client is in Waiting state.
     **/
    [[nodiscard]]
    inline bool is_waiting() const noexcept;

    /**
     * \brief   Returns true if the client is in Connected state.
     **/
    [[nodiscard]]
    inline bool is_connected() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ProxyAddress                  mClientAddress;   //!< The address of Proxy of client.
    areg::ServiceConnectionState  mClientState;     //!< The current state of client
};

//////////////////////////////////////////////////////////////////////////
// ClientInfo class inline functions implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

inline ClientInfo& ClientInfo::operator = (ClientInfo&& src) noexcept
{
    mClientAddress = std::move(src.mClientAddress);
    mClientState = src.mClientState;

    return (*this);
}

inline ClientInfo& ClientInfo::operator = (const ProxyAddress& client)
{
    mClientAddress = client;
    set_connection_status(areg::ServiceConnectionState::Pending);
    return (*this);
}

inline ClientInfo& ClientInfo::operator = (ProxyAddress&& client) noexcept
{
    mClientAddress = std::move(client);
    set_connection_status(areg::ServiceConnectionState::Pending);
    return (*this);
}

inline bool ClientInfo::operator == (const ProxyAddress& client) const noexcept
{
    return mClientAddress == client;
}

inline bool ClientInfo::operator == (const ClientInfo& other) const noexcept
{
    return ((this == &other) || (mClientAddress == other.mClientAddress));
}

inline bool ClientInfo::operator == (const StubAddress& server) const noexcept
{
    return mClientAddress == server;
}

inline ClientInfo::operator uint32_t () const noexcept
{
    const ServiceAddress& addrService = static_cast<const ServiceAddress&>(mClientAddress);
    return static_cast<uint32_t>(addrService);
}

//////////////////////////////////////////////////////////////////////////
// Inline attributes
//////////////////////////////////////////////////////////////////////////

inline areg::ServiceConnectionState ClientInfo::connection_status() const noexcept
{
    return mClientState;
}

inline const ProxyAddress & ClientInfo::address() const noexcept
{
    return mClientAddress;
}

inline bool ClientInfo::is_waiting() const noexcept
{
    return (mClientState == areg::ServiceConnectionState::Pending);
}

inline bool ClientInfo::is_connected() const noexcept
{
    return (mClientState == areg::ServiceConnectionState::Connected);
}

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Hasher of ClientInfo class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ClientInfo.
 */
namespace std {
    template<>
    struct hash<areg::ClientInfo>
    {
        //! A function to convert ClientInfo object to uint32_t.
        [[nodiscard]]
        inline uint32_t operator()(const areg::ClientInfo& key) const noexcept
        {
            return static_cast<uint32_t>(key);
        }
    };
} // namespace std

#endif  // AREG_COMPONENT_PRIVATE_CLIENTINFO_HPP
