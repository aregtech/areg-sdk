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
#include "areg/base/GEGlobal.h"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/ServiceDefs.hpp"

class StubAddress;
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
    static const ClientInfo & invalid_client_info();

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ClientInfo();
    /**
     * \brief   Initializes the object with the given proxy address.
     *
     * \param   client      The proxy address of the client.
     **/
    explicit ClientInfo( const ProxyAddress & client );
    /**
     * \brief   Initializes the object with the given proxy address.
     *
     * \param   client      The proxy address of the client.
     * \note    Move overload. Takes ownership of the proxy address.
     **/
    explicit ClientInfo( ProxyAddress && client ) noexcept;
    /**
     * \brief   Copies values from the given source.
     *
     * \param   src     The source of data to copy.
     **/
    ClientInfo( const ClientInfo & src );
    /**
     * \brief   Moves values from the given source.
     *
     * \param   src     The source of data to move.
     **/
    ClientInfo( ClientInfo && src ) noexcept;
    /**
     * \brief   Destructor
     **/
    ~ClientInfo() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies client info data from the given source.
     *
     * \param   src     The source of client info to copy.
     **/
    ClientInfo & operator = ( const ClientInfo & src );
    /**
     * \brief   Moves client info data from the given source.
     *
     * \param   src     The source of client info to move.
     **/
    ClientInfo & operator = ( ClientInfo && src ) noexcept;
    /**
     * \brief   Assigns a proxy address to client info; sets client to Waiting state if valid.
     *
     * \param   client      The proxy address to assign.
     **/
    ClientInfo & operator = ( const ProxyAddress & client );
    /**
     * \brief   Assigns a proxy address to client info; sets client to Waiting state if valid.
     *
     * \param   client      The proxy address to assign.
     * \note    Move overload. Takes ownership of the proxy address.
     **/
    ClientInfo & operator = ( ProxyAddress && client ) noexcept;

    /**
     * \brief   Returns true if both client info objects have equal proxy addresses.
     *
     * \param   other       The second client info instance to compare.
     * \return  Returns true if proxy addresses are equal; false otherwise.
     **/
    bool operator == ( const ClientInfo & other ) const;
    /**
     * \brief   Returns true if the client proxy address matches the given address; client state is
     *          ignored.
     *
     * \param   client      The proxy address to compare.
     * \return  Returns true if proxy addresses are equal; false otherwise.
     **/
    bool operator == ( const ProxyAddress & client ) const;
    /**
     * \brief   Returns true if the client proxy address is compatible with the given stub address;
     *          client state is ignored.
     *
     * \param   server      The stub address to compare.
     * \return  Returns true if proxy address is compatible with the stub address; false otherwise.
     **/
    bool operator == ( const StubAddress & server ) const;

    /**
     * \brief   Converts client info to a 32-bit unsigned integer.
     **/
    explicit operator uint32_t () const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Sets the target server address in the client info.
     *
     * \param   addrStub    The stub address of the target server.
     **/
    void set_target_server( const StubAddress & addrStub );

    /**
     * \brief   Sets the client connection state based on the server stub address.
     *
     * \param   newConnection       The stub address of the server; valid address sets client to
     *                              Connected, otherwise to Waiting or Undefined.
     **/
    void set_connection_status( NEService::ServiceConnectionState newConnection );

    /**
     * \brief   Returns the client connection state.
     **/
    inline NEService::ServiceConnectionState connection_status() const;

    /**
     * \brief   Returns the proxy address of the client.
     **/
    inline const ProxyAddress & address() const;

    /**
     * \brief   Returns true if the client is in Waiting state.
     **/
    inline bool is_waiting_connection() const;

    /**
     * \brief   Returns true if the client is in Connected state.
     **/
    inline bool is_connected() const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The address of Proxy of client.
     **/
    ProxyAddress                  mClientAddress;

    /**
     * \brief   The current state of client
     **/
    NEService::ServiceConnectionState  mClientState;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of ClientInfo class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ClientInfo.
 */
namespace std
{
    template<>
    struct hash<ClientInfo>
    {
        //! A function to convert ClientInfo object to uint32_t.
        inline uint32_t operator()(const ClientInfo& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// ClientInfo class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline NEService::ServiceConnectionState ClientInfo::connection_status() const
{
    return mClientState;
}

inline const ProxyAddress & ClientInfo::address() const
{
    return mClientAddress;
}

inline bool ClientInfo::is_waiting_connection() const
{
    return (mClientState == NEService::ServiceConnectionState::Pending);
}

inline bool ClientInfo::is_connected() const
{
    return (mClientState == NEService::ServiceConnectionState::Connected);
}

#endif  // AREG_COMPONENT_PRIVATE_CLIENTINFO_HPP
