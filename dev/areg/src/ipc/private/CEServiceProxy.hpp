#ifndef AREG_IPC_PRIVATE_CESERVICEPROXY_HPP
#define AREG_IPC_PRIVATE_CESERVICEPROXY_HPP
/************************************************************************
 * \file        areg/src/ipc/private/CEServiceProxy.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Proxy Service object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEProxyAddress.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEStubAddress;

//////////////////////////////////////////////////////////////////////////
// CEServiceProxy class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote servicing proxy object, which contains the remote proxy address.
 *          The entry is used by server connection service to list available proxy objects in network.
 **/
class AREG_API CEServiceProxy
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    CEServiceProxy( void );

    /**
     * \brief   The remote proxy address to set in object
     * \param   addrProxy   The address of remote proxy object.
     **/
    CEServiceProxy( const CEProxyAddress & addrProxy );

    /**
     * \brief   Copies data from given source
     * \param   serviceProxy    The source object to copy data
     **/
    CEServiceProxy( const CEServiceProxy & serviceProxy );

    /**
     * \brief   Destructor
     **/
    ~CEServiceProxy( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies data from given source
     * \param   serviceProxy    The source of remote servicing data to copy
     **/
    const CEServiceProxy & operator = ( const CEServiceProxy & serviceProxy );

    /**
     * \brief   Copies remote servicing proxy address data from given source
     * \param   addrProxy   The address of remote proxy object to set in object.
     **/
    const CEServiceProxy & operator = ( const CEProxyAddress & addrProxy );

    /**
     * \brief   Checks equality of 2 service proxy objects.
     *          2 remote servicing proxies are equal if the generated cookie
     *          and instantiated thread values are equal.
     * \param   serviceProxy    Another remote servicing proxy object to compare.
     * \return  Returns true if 2 objects are equal.
     **/
    bool operator == ( const CEServiceProxy & serviceProxy ) const;

    /**
     * \brief   Checks equality of 2 service proxy addresses.
     *          2 remote servicing proxy addresses are equal if the generated cookie
     *          and instantiated thread values are equal.
     * \param   addrProxy       The address of remote servicing proxy to check.
     * \return  Returns true if 2 objects are equal.
     **/
    bool operator == ( const CEProxyAddress & addrProxy ) const;

    /**
     * \brief   Checks equality of service proxy and remote stub objects.
     *          Remote servicing proxy and stub are equal if proxy address is equal
     *          to stub address, i.e. they have logical connection.
     * \param   addrStub        The address of remote servicing stub to check.
     * \return  Returns true if 2 objects are equal.
     **/
    bool operator == ( const CEStubAddress & addrStub ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if a remote proxy object is valid, i.e. the proxy address is valid.
     **/
    inline bool IsValid( void ) const;

    /**
     * \brief   Returns true if a remote proxy object status is connected
     **/
    inline bool IsConnected( void ) const;

    /**
     * \brief   Returns true if a remote proxy object status is waiting
     **/
    inline bool IsWaiting( void ) const;

    /**
     * \brief   Returns true the service connection status value
     **/
    inline NEService::eServiceConnection GetServiceStatus( void ) const;

    /**
     * \brief   Returns the address of remote proxy
     **/
    inline const CEProxyAddress & GetServiceAddress( void ) const;

    /**
     * \brief   Called when the remote service stub of proxy is available.
     *          The system sets the stub address as target address of proxy.
     * \param   addrStub    The stub address to set.
     * \return  Returns true if proxy entry status is connected, 
     *          i.e. at least of proxy client was waiting for connection
     **/
    bool StubAvailable( const CEStubAddress & addrStub );

    /**
     * \brief   Called when the remote service stub of proxy is not available anymore.
     *          The system resets the stub address as target address of proxy.
     * \return  Returns true if proxy entry status is waiting for connection,
     *          i.e. there was at least one client proxy connected or waiting for a connection.
     **/
    bool StubUnavailable( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Sets servicing new status. The call is ignored if proxy target is unknown.
     * \param   newStatus   New status of proxy to set.
     **/
    void _setServiceStatus( NEService::eServiceConnection newStatus );
    /**
     * \brief   Sets servicing proxy address and the connection status.
     * \param   addrProxy       The remote servicing proxy address to set.
     * \param   connectStatus   The connection status to update.
     **/
    void _setService( const CEProxyAddress & addrProxy, NEService::eServiceConnection connectStatus = NEService::ServicePending );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The remote servicing proxy address
     **/
    CEProxyAddress                  mProxyAddress;
    /**
     * \brief   The remote servicing connection status.
     **/
    NEService::eServiceConnection   mConnectStatus;
};

//////////////////////////////////////////////////////////////////////////
// CEServiceProxy class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const CEProxyAddress & CEServiceProxy::GetServiceAddress(void) const
{   return mProxyAddress;                                       }

inline NEService::eServiceConnection CEServiceProxy::GetServiceStatus(void) const
{   return mConnectStatus;                                      }

inline bool CEServiceProxy::IsValid(void) const
{   return mProxyAddress.IsValid();                             }

inline bool CEServiceProxy::IsConnected( void ) const
{   return ( mConnectStatus == NEService::ServiceConnected );  }

inline bool CEServiceProxy::IsWaiting( void ) const
{   return ( mConnectStatus == NEService::ServicePending );    }

#endif  // AREG_IPC_PRIVATE_CESERVICEPROXY_HPP
