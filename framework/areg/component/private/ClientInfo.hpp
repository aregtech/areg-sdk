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
#include "areg/component/NEService.hpp"

class StubAddress;
//////////////////////////////////////////////////////////////////////////
// ClientInfo class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This is a helper class used of servicing interface client used by
 *          Service Manager and it contains information of client proxy address
 *          and client state.
 **/
class ClientInfo
{
//////////////////////////////////////////////////////////////////////////
// ClientInfo static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Predefines invalid client info object
     **/
    static const ClientInfo & getInvalidClientInfo( void );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes default values.
     **/
    ClientInfo( void );
    /**
     * \brief   Initializes object and sets client Proxy address value.
     * \param   client  The Proxy address of the client
     **/
    explicit ClientInfo( const ProxyAddress & client );
    /**
     * \brief   Initializes object and sets client Proxy address value.
     * \param   client  The Proxy address of the client
     **/
    explicit ClientInfo( ProxyAddress && client ) noexcept;
    /**
     * \brief   Copies values from given source.
     * \param   src     The source of data to copy.
     **/
    ClientInfo( const ClientInfo & src );
    /**
     * \brief   Moves values from given source.
     * \param   src     The source of data to move.
     **/
    ClientInfo( ClientInfo && src ) noexcept;
    /**
     * \brief   Destructor
     **/
    ~ClientInfo( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies Client Info data from given source.
     * \param   src     The source of Client Info to get data.
     **/
    ClientInfo & operator = ( const ClientInfo & src );
    /**
     * \brief   Moves Client Info data from given source.
     * \param   src     The source of Client Info to get data.
     **/
    ClientInfo & operator = ( ClientInfo && src ) noexcept;
    /**
     * \brief   Copies passed client Proxy address, if Service Info
     *          object is valid, it sets client in Waiting state
     * \param   client  The source of proxy address to extract data.
     **/
    ClientInfo & operator = ( const ProxyAddress & client );
    /**
     * \brief   Moves passed client Proxy address, if Service Info
     *          object is valid, it sets client in Waiting state
     * \param   client  The source of proxy address to extract data.
     **/
    ClientInfo & operator = ( ProxyAddress && client ) noexcept;

    /**
     * \brief   Checks equality of 2 Client Info objects. 2 objects are equal if
     *          Service Info and the Proxy addresses are equal.
     * \param   other   The second instance of client info to check.
     * \return  Returns true if Service Info and the Proxy addresses are equal
     **/
    bool operator == ( const ClientInfo & other ) const;
    /**
     * \brief   Checks equality of data of client and given proxy and returns true
     *          if proxy address of client service and if proxy addresses are equal.
     *          Checking state of client is ignored.
     * \param   client  The proxy address to check equality.
     * \return  Returns true if proxy address of client service and given proxy address 
     *          are equal.
     **/
    bool operator == ( const ProxyAddress & client ) const;
    /**
     * \brief   Check equality / compatibility of servicing client and
     *          given stub address, and returns true if the proxy address
     *          of client is compatible with given stub address.
     *          Checking state of client is ignored.
     * \param   server  The Stub address to check equality / compatibility.
     * \return  Returns true if existing proxy address is compatible with
     *          given proxy address.
     **/
    bool operator == ( const StubAddress & server ) const;

    /**
     * \brief   Operator to covert client info to 32-bit unsigned integer.
     **/
    explicit operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
    * \brief   Sets target server object in client info.
    * \param   addrStub    The address of targeted server component to set in client information.
    **/
    void setTargetServer( const StubAddress & addrStub );

    /**
     * \brief   Called when server is in connected state.
     *          It will set the address of Stub of server and will change the state of client.
     *          If specified address of Stub is valid, it sets client state connected.
     *          Otherwise, if Service Info is valid, it will set client state waiting.
     *          Otherwise, it sets client state undefined.
     *          Service Info is valid, if Role Name and Service Names are valid.
     *          For more information of Service Info see description of ServiceInfo class.
     * \param   newConnection   The address of the Stub of server (service provider).
     *                          If valid address, this will change the state of client to connected.
     **/
    void setConnectionStatus( NEService::eServiceConnection newConnection );

    /**
     * \brief   Returns the state of client.
     *          For more information, see description of states
     *          defined in ClientInfo::eClientState
     **/
    inline NEService::eServiceConnection getConnectionStatus( void ) const;

    /**
     * \brief   Returns the address of Proxy of client
     **/
    inline const ProxyAddress & getAddress( void ) const;

    /**
     * \brief   Returns true if client is in waiting state
     **/
    inline bool isWaitingConnection( void ) const;

    /**
     * \brief   Returns true if client is in connected state.
     **/
    inline bool isConnected( void ) const;

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
    NEService::eServiceConnection  mClientState;
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
        //! A function to convert ClientInfo object to unsigned int.
        inline unsigned int operator()(const ClientInfo& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// ClientInfo class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline NEService::eServiceConnection ClientInfo::getConnectionStatus( void ) const
{
    return mClientState;
}

inline const ProxyAddress & ClientInfo::getAddress( void ) const
{
    return mClientAddress;
}

inline bool ClientInfo::isWaitingConnection( void ) const
{
    return (mClientState == NEService::eServiceConnection::ServicePending);
}

inline bool ClientInfo::isConnected( void ) const
{
    return (mClientState == NEService::eServiceConnection::ServiceConnected);
}

#endif  // AREG_COMPONENT_PRIVATE_CLIENTINFO_HPP
