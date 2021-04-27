#ifndef AREG_IPC_PRIVATE_CELISTSERVICEPROXIES_HPP
#define AREG_IPC_PRIVATE_CELISTSERVICEPROXIES_HPP

/************************************************************************
 * \file        areg/ipc/private/CEListServiceProxies.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Proxy Service object, list of proxies
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TELinkedList.hpp"
#include "areg/ipc/private/CEServiceProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEServiceStub;

//////////////////////////////////////////////////////////////////////////
// CEListServiceProxies class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The list of remote service proxies registered in network.
 **/
class AREG_API CEListServiceProxies  : public TELinkedList<CEServiceProxy, const CEServiceProxy &>
{
//////////////////////////////////////////////////////////////////////////
// The internal constants and types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Invalid proxy service
     **/
    static const CEServiceProxy     InvalidProxyService;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty proxy list
     **/
    CEListServiceProxies( void );

    /**
     * \brief   Copies the list of proxies from the given source.
     * \param   source  The source of proxy list to copy
     **/
    CEListServiceProxies( const CEListServiceProxies & source );

    /**
     * \brief   Destructor
     **/
    virtual ~CEListServiceProxies( void );

//////////////////////////////////////////////////////////////////////////
// Operators, operations and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Empties existing list and copies all entries from given source.
     * \param   source  The source of proxy list to copy
     **/
    const CEListServiceProxies & operator = ( const CEListServiceProxies & source );

    /**
     * \brief   Returns true if specified proxy is already registered in the list.
     * \param   addrProxy   The address of proxy service to check.
     **/
    inline bool IsServiceRegistered( const CEProxyAddress & addrProxy ) const;

    /**
     * \brief   Returns existing service proxy entry. If specified proxy address
     *          is not registered, returns invalid entry, which proxy address is invalid.
     **/
    const CEServiceProxy & GetService( const CEProxyAddress & addrProxy ) const;

    /**
     * \brief   Registers existing proxy service entry or creates new if
     *          list does not contain service with specified proxy address.
     *          If proxy address is not valid, no entry is created and returned
     *          object is invalid proxy service object.
     *          The call of method assumes that stub service is not available yet,
     *          and if proxy address is valid, it set service connection status
     *          to pending state.
     * \param   addrProxy   The entry of service with proxy address to search.
     * \return  If specified proxy address is valid, returns either existing or 
     *          new proxy service entry. Otherwise, returns invalid service object.
     **/
    CEServiceProxy & RegisterService( const CEProxyAddress & addrProxy );

    /**
     * \brief   Registers existing proxy service entry or creates new if
     *          list does not contain service with specified proxy address.
     *          If proxy address is not valid, no entry is created and returned
     *          object is invalid proxy service object.
     *          The call of method assumes that stub service is available if
     *          stub service is valid and state is connected. If stub service is
     *          connected, it will set proxy service state to connected state.
     *          Otherwise, the connection status is pending.
     * \param   addrProxy   The entry of service with proxy address to search.
     * \return  If specified proxy address is valid, returns either existing or 
     *          new proxy service entry. Otherwise, returns invalid service object.
     **/
    CEServiceProxy & RegisterService( const CEProxyAddress & addrProxy, const CEServiceStub & stubService );

    /**
     * \brief   Unregisters and removes entry of proxy service with specified
     *          proxy address. If no entry exists, the return service is not valid.
     * \param   addrProxy   The address of proxy to unregister.
     * \return  Returns address of unregistered proxy. If proxy was not found
     *          or proxy address was not found, returns invalid proxy service.
     **/
    CEServiceProxy UnregisterService( const CEProxyAddress & addrProxy );

    /**
     * \brief   Sets all registered proxy services in connected state
     *          and returns number of entries modified in list. If list contains
     *          no entry, the return value is zero.
     *          The target address of all proxy service entries will be
     *          modified and the state of registered services will be connected.
     * \param   addrStub    Address of stub service, which is available.
     * \return  If list of proxy services is not empty, the return value is
     *          not zero. Otherwise, it returns zero.
     **/
    int StubServiceAvailable( const CEStubAddress & addrStub );

    /**
     * \brief   Sets all registered proxy service in pending state
     *          and returns number of entries modified in list.
     *          The target address of all registered proxies will
     *          be reset and the state will be pending.
     * \return  If list of proxy services is not empty, the return value is
     *          not zero. Otherwise, it returns zero.
     **/
    int StubServiceUnavailable( void );

    /**
     * \brief   Search in the list and collects all proxies, which cookie is equal
     *          to specified cookie value and adds to out_listProxies list.
     * \param   out_listProxies The list of proxy service entries, which found in list.
     *                          The existing entries in the list will not be removed.
     * \param   cookie          The cookie value to lookup.
     * \return  Returns number of entries found in the list. If no entry found, 
     *          return value is zero.
     **/
    int GetSpecificService( CEListServiceProxies & out_listProxies, ITEM_ID cookie );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Search in list the service proxy entry, which proxy is equal to 
     *          given proxy address.
     * \param   addrProxy   Address of proxy to search
     * \return  Returns valid position value if entry found. Otherwise, returns NULL.
     **/
    LISTPOS findProxy( const CEProxyAddress & addrProxy ) const;
};

//////////////////////////////////////////////////////////////////////////
// CEListServiceProxies class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool CEListServiceProxies::IsServiceRegistered( const CEProxyAddress & addrProxy ) const
{   return ( findProxy(addrProxy) != NULL );    }

#endif  // AREG_IPC_PRIVATE_CELISTSERVICEPROXIES_HPP
