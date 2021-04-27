#ifndef AREG_COMPONENT_PRIVATE_CECLIENTINFO_HPP
#define AREG_COMPONENT_PRIVATE_CECLIENTINFO_HPP
/************************************************************************
 * \file        areg/component/private/CEClientInfo.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Client Info class declaration
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/CEProxyAddress.hpp"
#include "areg/component/NEService.hpp"

class CEStubAddress;
//////////////////////////////////////////////////////////////////////////
// CEClientInfo class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This is a helper class used of servicing interface client used by
 *          Service Manager and it contains information of client proxy address
 *          and client state.
 **/
class CEClientInfo
{
//////////////////////////////////////////////////////////////////////////
// CEClientInfo predefined types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   CEClientInfo::INVALID_CLIENT_INFO
     *          Defines invalid client info object
     **/
    static const CEClientInfo   INVALID_CLIENT_INFO;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes default values.
     **/
    CEClientInfo( void );
    /**
     * \brief   Initializes object and sets client Proxy address value.
     * \param   client  The Proxy address of the client
     **/
    CEClientInfo( const CEProxyAddress & client );
    /**
     * \brief   Initializes data by copying values from given source.
     * \param   src     The source of data to copy.
     **/
    CEClientInfo( const CEClientInfo & src );
    /**
     * \brief   Destructor
     **/
    virtual ~CEClientInfo( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies Client Info data from given source.
     * \param   src     The source of Client Info to get data.
     **/
    const CEClientInfo & operator = ( const CEClientInfo & src );
    /**
     * \brief   Copies passed client Proxy address, if Service Info
     *          object is valid, it sets client in Waiting state
     * \param   client  The source of proxy address to extract data.
     **/
    const CEClientInfo & operator = ( const CEProxyAddress & client );

    /**
     * \brief   Checks equality of 2 Client Info objects. 2 objects are equal if
     *          Service Info and the Proxy addresses are equal.
     * \param   other   The second instance of client info to check.
     * \return  Returns true if Service Info and the Proxy addresses are equal
     **/
    bool operator == ( const CEClientInfo & other ) const;
    /**
     * \brief   Checks equality of data of client and given proxy and returns true
     *          if proxy address of client service and if proxy addresses are equal.
     *          Checking state of client is ignored.
     * \param   client  The proxy address to check equality.
     * \return  Returns true if proxy address of client service and given proxy address 
     *          are equal.
     **/
    bool operator == ( const CEProxyAddress & client ) const;
    /**
     * \brief   Check equality / compatibility of servicing client and
     *          given stub address, and returns true if the proxy address
     *          of client is compatible with given stub address.
     *          Checking state of client is ignored.
     * \param   server  The Stub address to check equality / compatibility.
     * \return  Returns true if existing proxy address is compatible with
     *          given proxy address.
     **/
    bool operator == ( const CEStubAddress & server ) const;

    /**
     * \brief   Operator to covert client info to 32-bit unsigned integer.
     **/
    operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
    * \brief   Sets target server object in client info.
    * \param   addrStub    The address of targeted server component to set in client information.
    **/
    void SetTargetServer( const CEStubAddress & addrStub );

    /**
     * \brief   Called when server is in connected state.
     *          It will set the address of Stub of server and will change the state of client.
     *          If specified address of Stub is valid, it sets client state connected.
     *          Otherwise, if Service Info is valid, it will set client state waiting.
     *          Otherwise, it sets client state undefined.
     *          Service Info is valid, if Role Name and Service Names are valid.
     *          For more information of Service Info see description of CEServiceInfo class.
     * \param   server  The address of Stub of server.
     *                  If valid address, this will change the state of client to connected.
     **/
    void SetConnectionStatus( NEService::eServiceConnection newConnection );

    /**
     * \brief   Returns the state of client.
     *          For more information, see description of states
     *          defined in CEClientInfo::eClientState
     **/
    inline NEService::eServiceConnection GetConnectionState( void ) const;

    /**
     * \brief   Returns the address of Proxy of client
     **/
    inline const CEProxyAddress & GetAddress( void ) const;

    /**
     * \brief   Returns true if client is in waiting state
     **/
    inline bool IsWaiting( void ) const;

    /**
     * \brief   Returns true if client is in connected state.
     **/
    inline bool IsConnected( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The address of Proxy of client.
     **/
    CEProxyAddress                  mClientAddress;

    /**
     * \brief   The current state of client
     **/
    NEService::eServiceConnection  mClientState;
};

//////////////////////////////////////////////////////////////////////////
// CEClientInfo class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline NEService::eServiceConnection CEClientInfo::GetConnectionState( void ) const
{   return mClientState;                                                        }

inline const CEProxyAddress & CEClientInfo::GetAddress( void ) const
{   return mClientAddress;                                                      }

inline bool CEClientInfo::IsWaiting( void ) const
{   return (mClientState == NEService::ServicePending);                         }

inline bool CEClientInfo::IsConnected( void ) const
{   return (mClientState == NEService::ServiceConnected);                       }

#endif  // AREG_COMPONENT_PRIVATE_CECLIENTINFO_HPP
