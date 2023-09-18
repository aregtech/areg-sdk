#ifndef AREG_MCROUTER_TCP_PRIVATE_LISTSERVICEPROXIES_HPP
#define AREG_MCROUTER_TCP_PRIVATE_LISTSERVICEPROXIES_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/tcp/private/ListServiceProxies.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Proxy Service object, list of proxies
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TELinkedList.hpp"
#include "mcrouter/tcp/private/ServiceProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class ServiceStub;

using ListServiceProxiesBase = TELinkedList<ServiceProxy>;
//////////////////////////////////////////////////////////////////////////
// ListServiceProxies class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The list of remote service proxies registered in network.
 **/
class ListServiceProxies  : public ListServiceProxiesBase
{
//////////////////////////////////////////////////////////////////////////
// The internal constants and types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Invalid proxy service
     **/
    static const ServiceProxy     InvalidProxyService;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty proxy list
     **/
    ListServiceProxies( void ) = default;

    /**
     * \brief   Copies the list of proxies from the given source.
     * \param   source  The source of proxy list to copy
     **/
    ListServiceProxies( const ListServiceProxies & source ) = default;

    /**
     * \brief   Moves the list of proxies from the given source.
     * \param   source  The source of proxy list to move.
     **/
    ListServiceProxies( ListServiceProxies && source ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~ListServiceProxies( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators, operations and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Empties existing list and copies all entries from given source.
     * \param   source  The source of proxy list to copy
     **/
    ListServiceProxies & operator = ( const ListServiceProxies & source ) = default;

    /**
     * \brief   Empties existing list and moves all entries from given source.
     * \param   source  The source of proxy list to move.
     **/
    ListServiceProxies & operator = ( ListServiceProxies && source ) noexcept = default;

    /**
     * \brief   Returns true if specified proxy is already registered in the list.
     * \param   addrProxy   The address of proxy service to check.
     **/
    inline bool isServiceRegistered( const ProxyAddress & addrProxy ) const;

    /**
     * \brief   Returns existing service proxy entry. If specified proxy address
     *          is not registered, returns invalid entry, which proxy address is invalid.
     **/
    const ServiceProxy & getService( const ProxyAddress & addrProxy ) const;
    
    /**
     * \brief   Returns existing service proxy entry. Returns nullptr, if specified proxy address
     *          is not registered.
     **/
    ServiceProxy * getService( const ProxyAddress & addrProxy );

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
    ServiceProxy & registerService( const ProxyAddress & addrProxy );

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
    ServiceProxy & registerService( const ProxyAddress & addrProxy, const ServiceStub & stubService );

    /**
     * \brief   Unregisters and removes entry of proxy service with specified
     *          proxy address. If no entry exists, the return service is not valid.
     * \param   addrProxy   The address of proxy to unregister.
     * \return  Returns address of unregistered proxy. If proxy was not found
     *          or proxy address was not found, returns invalid proxy service.
     **/
    ServiceProxy unregisterService( const ProxyAddress & addrProxy );

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
    int stubServiceAvailable( const StubAddress & addrStub );

    /**
     * \brief   Sets all registered proxy service in pending state
     *          and returns number of entries modified in list.
     *          The target address of all registered proxies will
     *          be reset and the state will be pending.
     * \return  If list of proxy services is not empty, the return value is
     *          not zero. Otherwise, it returns zero.
     **/
    int stubServiceUnavailable( void );

    /**
     * \brief   Search in the list and collects all proxies, which cookie is equal
     *          to specified cookie value and adds to out_listProxies list.
     * \param   out_listProxies The list of proxy service entries, which found in list.
     *                          The existing entries in the list will not be removed.
     * \param   cookie          The cookie value to lookup.
     * \return  Returns number of entries found in the list. If no entry found, 
     *          return value is zero.
     **/
    int getSpecificService( ListServiceProxies & out_listProxies, const ITEM_ID & cookie );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Search in list the service proxy entry, which proxy is equal to 
     *          given proxy address.
     * \param   addrProxy   Address of proxy to search
     * \return  Returns valid position value if entry found. Otherwise, returns nullptr.
     **/
    LISTPOS _findProxy( const ProxyAddress & addrProxy ) const;
};

//////////////////////////////////////////////////////////////////////////
// ListServiceProxies class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool ListServiceProxies::isServiceRegistered( const ProxyAddress & addrProxy ) const
{
    return ListServiceProxiesBase::isValidPosition(_findProxy(addrProxy));
}

#endif  // AREG_MCROUTER_TCP_PRIVATE_LISTSERVICEPROXIES_HPP
