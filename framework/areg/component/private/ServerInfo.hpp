#ifndef AREG_COMPONENT_PRIVATE_SERVERINFO_HPP
#define AREG_COMPONENT_PRIVATE_SERVERINFO_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServerInfo.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Server Info class declaration
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ServiceDefs.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class ClientInfo;

//////////////////////////////////////////////////////////////////////////
// ServerInfo class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Stores stub server information for the routing service, including address, state, and
 *          service details. Used to track server status and notify clients of availability.
 **/
class ServerInfo
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    ServerInfo();

    /**
     * \brief   Initializes with stub address; sets state to Registered if valid.
     * \param   server      The stub server address.
     **/
    explicit ServerInfo( const StubAddress & server );

    /**
     * \brief   Initializes with stub address (move); sets state to Registered if valid.
     * \param   server      The stub server address.
     **/
    explicit ServerInfo( StubAddress && server );

    /**
     * \brief   Initializes with stub address extracted from proxy address.
     * \param   proxy       The proxy address to extract stub address from.
     **/
    explicit ServerInfo( const ProxyAddress & proxy );

    ServerInfo( const ServerInfo & src );

    ServerInfo( ServerInfo && src ) noexcept;

    ~ServerInfo() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies server info data from given source.
     *
     * \param   src     The source to copy.
     **/
    ServerInfo & operator = ( const ServerInfo & src );

    /**
     * \brief   Moves server info data from given source.
     *
     * \param   src     The source to move.
     **/
    ServerInfo & operator = ( ServerInfo && src ) noexcept;

    /**
     * \brief   Sets stub address and state to Connected if valid, otherwise Undefined.
     *
     * \param   server      The stub address to set.
     **/
    ServerInfo & operator = ( const StubAddress & server );

    /**
     * \brief   Sets stub address (move) and state to Connected if valid, otherwise Undefined.
     *
     * \param   server      The stub address to set.
     **/
    ServerInfo & operator = ( StubAddress && server ) noexcept;

    /**
     * \brief   Sets stub address and state to Pending if valid, otherwise Undefined.
     *
     * \param   addService      The service address to convert.
     **/
    ServerInfo & operator = ( const ServiceAddress & addService );

    /**
     * \brief   Sets stub address (move) and state to Pending if valid, otherwise Undefined.
     *
     * \param   addService      The service address to convert.
     **/
    ServerInfo & operator = ( ServiceAddress && addService ) noexcept;

    /**
     * \brief   Returns true if server info objects are equal.
     *
     * \param   other       The server info to compare.
     **/
    bool operator == ( const ServerInfo & other ) const;

    /**
     * \brief   Returns true if stub addresses are equal.
     *
     * \param   server      The stub address to compare.
     **/
    bool operator == ( const StubAddress & server ) const;

    /**
     * \brief   Returns true if the stub address of this and the proxy are equal.
     *
     * \param   proxy       The proxy address to compare.
     **/
    bool operator == ( const ProxyAddress & proxy ) const;

    /**
     * \brief   Converts server info to 32-bit unsigned integer.
     **/
    explicit operator uint32_t () const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the stub server address.
     **/
    inline const StubAddress & address() const;

    /**
     * \brief   Returns the server connection state.
     **/
    [[nodiscard]]
    inline areg::ServiceConnectionState connection_status() const noexcept;

    /**
     * \brief   Sets the connection status.
     *
     * \param   newConnection       The service connection status.
     **/
    void set_connection_status( areg::ServiceConnectionState newConnection );

    /**
     * \brief   Returns true if server is connected.
     **/
    [[nodiscard]]
    inline bool is_connected() const noexcept;

    /**
     * \brief   Returns true if server is pending.
     **/
    [[nodiscard]]
    inline bool is_waiting() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The Server Stub address
     **/
    StubAddress mServerAddress;
    /**
     * \brief   The State of Server Info. State Registered set only when Stub address is valid.
     **/
    areg::ServiceConnectionState  mServerState;
};

//////////////////////////////////////////////////////////////////////////
// ServerInfo class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline const StubAddress& ServerInfo::address() const
{
    return mServerAddress;
}

inline areg::ServiceConnectionState ServerInfo::connection_status() const noexcept
{
    return mServerState;
}

inline bool ServerInfo::is_connected() const noexcept
{
    return mServerState == areg::ServiceConnectionState::Connected;
}

inline bool ServerInfo::is_waiting() const noexcept
{
    return mServerState == areg::ServiceConnectionState::Pending;
}

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Hasher of ServerInfo class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ServerInfo.
 */
namespace std {
    //! Calculates the hash value of the ServerInfo object
    template<> struct hash<areg::ServerInfo>
    {
        //! A function to convert ServerInfo object to uint32_t.
        inline uint32_t operator()(const areg::ServerInfo& key) const
        {
            return static_cast<uint32_t>(static_cast<const areg::ServiceAddress&>(key.address()));
        }
    };

    //!< Compares 2 ServerInfo objects
    template<> struct equal_to<areg::ServerInfo>
    {
        //! A function operator to compare 2 ServerInfo objects.
        inline bool operator() (const areg::ServerInfo& key1, const areg::ServerInfo& key2) const
        {
            return static_cast<const areg::ServiceAddress&>(key1.address()) == static_cast<const areg::ServiceAddress&>(key2.address());
        }
    };
} // namespace std

#endif  // AREG_COMPONENT_PRIVATE_SERVERINFO_HPP
