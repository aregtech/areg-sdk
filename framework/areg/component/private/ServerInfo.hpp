#ifndef AREG_COMPONENT_PRIVATE_SERVERINFO_HPP
#define AREG_COMPONENT_PRIVATE_SERVERINFO_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServerInfo.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Server Info class declaration
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/StubAddress.hpp"
#include "areg/component/NEService.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class ClientInfo;

//////////////////////////////////////////////////////////////////////////
// ServerInfo class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Server Info is a helper class used by Routing Service,
 *          which contains information of Stub address,
 *          target Stub address, server state and Service Info objects.
 *          Every instantiated Stub is registered in Router Service,
 *          which will notify clients about service availability.
 **/
class ServerInfo
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default Constructor.
     *          Creates Server Info with invalid Stub address and sets state Undefined.
     **/
    ServerInfo( void );

    /**
     * \brief   Initialization constructor.
     *          Creates Server Info object, copies given address
     *          and sets state to Registered if specified server address is valid.
     * \param   server  The address of Stub server
     **/
    explicit ServerInfo( const StubAddress & server );

    /**
     * \brief   Initialization constructor.
     *          Creates Server Info object, copies given address
     *          and sets state to Registered if specified server address is valid.
     * \param   server  The address of Stub server
     **/
    explicit ServerInfo( StubAddress && server );

    /**
     * \brief   Initialization constructor.
     *          Creates Server Info object, copies server address data from proxy address.
     * \param   proxy   The Proxy address to extract data of Stub address
     **/
    explicit ServerInfo( const ProxyAddress & proxy );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    ServerInfo( const ServerInfo & src );

    /**
     * \brief   Move constructor.
     * \param   src     The source of data to move.
     **/
    ServerInfo( ServerInfo && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~ServerInfo( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies Server Info data from given source.
     * \param   src     The source of Server Info to copy data.
     **/
    ServerInfo & operator = ( const ServerInfo & src );

    /**
     * \brief   Moves Server Info data from given source.
     * \param   src     The source of Server Info to move data.
     **/
    ServerInfo & operator = ( ServerInfo && src ) noexcept;

    /**
     * \brief   Sets the Stub server address without changing any other information and sets Server Info state to Connected
     *          if specified Stub address is valid. Otherwise, it will set state Undefined.
     * \param   server  The stub address to set.
     **/
    ServerInfo & operator = ( const StubAddress & server );

    /**
     * \brief   Sets the Stub server address without changing any other information and sets Server Info state to Connected
     *          if specified Stub address is valid. Otherwise, it will set state Undefined.
     * \param   server  The stub address to set.
     **/
    ServerInfo & operator = ( StubAddress && server ) noexcept;

    /**
     * \brief   Sets the Stub server address without changing any other information and sets Server Info state to Pending
     *          if specified Stub address is valid. Otherwise, it will set state Undefined.
     * \param   addService  Service address.
     **/
    ServerInfo & operator = ( const ServiceAddress & addService );

    /**
     * \brief   Sets the Stub server address without changing any other information and sets Server Info state to Pending
     *          if specified Stub address is valid. Otherwise, it will set state Undefined.
     * \param   addService  Service address.
     **/
    ServerInfo & operator = ( ServiceAddress && addService ) noexcept;

    /**
     * \brief   Returns true, if 2 Server Info objects are equal. Otherwise returns false.
     * \param   other   Server Info object to compare
     **/
    bool operator == ( const ServerInfo & other ) const;

    /**
     * \brief   Returns true, if specified and the existing Stub address objects are equal. Otherwise returns false.
     * \param   server  The Stub server address to compare
     **/
    bool operator == ( const StubAddress & server ) const;

    /**
     * \brief   Returns true, if the Service Info of specified Client Info object and the existing Service Info objects are equal.
     *          Otherwise returns false.
     * \param   proxy   The Proxy client address to compare
     **/
    bool operator == ( const ProxyAddress & proxy ) const;

    /**
     * \brief   Converts Server Info object to unsigned integer.
     *          The conversion is done based on Service Info conversion mechanism.
     **/
    explicit operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns Stub server address
     **/
    inline const StubAddress & getAddress( void ) const;

    /**
     * \brief   Returns the state of Server Info.
     *          The Server Info state is updated only when Stub address is set.
     *          No separate Set-method presented
     **/
    inline NEService::eServiceConnection getConnectionStatus( void ) const;

    /**
     * \brief   Changes the state of server
     * \param   serverState     The service connection state of server.
     **/
    void setConnectionStatus( NEService::eServiceConnection newConnection );

    /**
     * \brief   Returns true if server status is Connected
     **/
    inline bool isConnected( void ) const;

    /**
     * \brief   Returns true if server status is Pending
     **/
    inline bool isWaiting( void ) const;

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
    NEService::eServiceConnection  mServerState;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of ServerInfo class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ServerInfo.
 */
namespace std
{
    //! Calculates the hash value of the ServerInfo object
    template<> struct hash<ServerInfo>
    {
        //! A function to convert ServerInfo object to unsigned int.
        inline unsigned int operator()(const ServerInfo& key) const
        {
            return static_cast<unsigned int>(static_cast<const ServiceAddress &>(key.getAddress()));
        }
    };

    //!< Compares 2 ServerInfo objects
    template<> struct equal_to<ServerInfo>
    {
        //! A function operator to compare 2 ServerInfo objects.
        inline bool operator() (const ServerInfo& key1, const ServerInfo& key2) const
        {
            return static_cast<const ServiceAddress&>(key1.getAddress()) == static_cast<const ServiceAddress&>(key2.getAddress());
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// ServerInfo class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline const StubAddress& ServerInfo::getAddress( void ) const
{
    return mServerAddress;
}

inline NEService::eServiceConnection ServerInfo::getConnectionStatus( void ) const
{
    return mServerState;
}

inline bool ServerInfo::isConnected( void ) const
{
    return mServerState == NEService::eServiceConnection::ServiceConnected;
}

inline bool ServerInfo::isWaiting( void ) const
{
    return mServerState == NEService::eServiceConnection::ServicePending;
}

#endif  // AREG_COMPONENT_PRIVATE_SERVERINFO_HPP
