#ifndef AREG_COMPONENT_PRIVATE_SERVERINFO_HPP
#define AREG_COMPONENT_PRIVATE_SERVERINFO_HPP
/************************************************************************
 * \file        areg/component/private/ServerInfo.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
 * \brief   The Server Info is a helper class used by Server Broker,
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
    ServerInfo( const StubAddress & server );

    /**
     * \brief   Initialization constructor.
     *          Creates Server Info object, copies server address data from proxy address.
     * \param   proxy   The Proxy address to extract data of Stub address
     **/
    ServerInfo( const ProxyAddress & proxy );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    ServerInfo( const ServerInfo & src );

    /**
     * \brief   Destructor
     **/
    ~ServerInfo( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigning operator. Copies Server Info data from given source.
     * \param   src     The source of Server Info to copy data.
     **/
    const ServerInfo & operator = ( const ServerInfo & src );

    /**
     * \brief   Sets the Stub server address without changing any other information and sets Server Info state to Connected
     *          if specified Stub address is valid. Otherwise, it will set state Undefined.
     * \param   server  The stub address to set.
     **/
    const ServerInfo & operator = ( const StubAddress & server );

    /**
     * \brief   Sets the Stub server address without changing any other information and sets Server Info state to Pending
     *          if specified Stub address is valid. Otherwise, it will set state Undefined.
     * \param   addService  Service address.
     **/
    const ServerInfo & operator = ( const ServiceAddress & addService );

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
    operator unsigned int ( void ) const;

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
    return mServerState == NEService::ServiceConnected;
}

inline bool ServerInfo::isWaiting( void ) const
{
    return mServerState == NEService::ServicePending;
}

#endif  // AREG_COMPONENT_PRIVATE_SERVERINFO_HPP
