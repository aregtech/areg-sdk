#ifndef AREG_IPC_PRIVATE_SERVICEREGISTRY_HPP
#define AREG_IPC_PRIVATE_SERVICEREGISTRY_HPP
/************************************************************************
 * \file        areg/ipc/private/ServiceRegistry.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Service Manager
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEHashMap.hpp"
#include "areg/ipc/private/ServiceStub.hpp"
#include "areg/ipc/private/ListServiceProxies.hpp"
#include "areg/base/TEArrayList.hpp"

//////////////////////////////////////////////////////////////////////////
// ServiceRegistry class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service registry map helper class.
 **/
class AREG_API ServiceRegistryImpl    : public TEHashMapImpl<const ServiceStub &, const ListServiceProxies &>
{
public:
    /**
     * \brief   Called to calculate the 32-bit hash key value.
     * \ param  Key     The object to calculate 32-bit hash key.
     * \return  Returns 32-bit hash key value.
     **/
    inline unsigned int implHashKey( const ServiceStub & Key ) const
    {
        return static_cast<unsigned int>(static_cast<const ServiceAddress &>(Key.getServiceAddress()));
    }
};

/**
 * \brief   The remote services registration map, which is a map of stub and list of connected proxies.
 **/
class AREG_API ServiceRegistry  : public TEHashMap<ServiceStub, ListServiceProxies, const ServiceStub &, const ListServiceProxies &, ServiceRegistryImpl>
{
//////////////////////////////////////////////////////////////////////////
// Predefined types and constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   ServiceMap::ServiceBlock
     *          Defines Block class of Hash Map as ServiceBlock. Defined to short syntax.
     **/
    typedef TEHashMap<ServiceStub, ListServiceProxies, const ServiceStub &, const ListServiceProxies &, ServiceRegistryImpl>::Block ServiceBlock;

    /**
     * \brief   ServiceMap::InvalidStubService
     *          Defines invalid stub service
     **/
    static const ServiceStub          InvalidStubService;

    /**
     * \brief   ServiceMap::EmptyProxiesList
     *          Defines empty list of remote connected proxies.
     **/
    static const ListServiceProxies   EmptyProxiesList;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    ServiceRegistry( void );
    /**
     * \brief   Destructor
     **/
    ~ServiceRegistry( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if passed stub service is already registered.
     * \param   addrStub    The remote servicing stub object to check.
     **/
    bool isServiceRegistered( const StubAddress & addrStub ) const;

    /**
     * \brief   Returns true if passed proxy address is already registered.
     * \param   addrProxy   The address of proxy to check.
     **/
    bool isServiceRegistered( const ProxyAddress & addrProxy ) const;

    /**
     * \brief   By given remote servicing address finds the address of stub,
     *          if there is any, or returns invalid stub address.
     * \param   addrService     The servicing address to check.
     * \return  Returns valid Stub address if it is registered. Otherwise, returns invalid Stub address.
     **/
    const ServiceStub & getStubService( const ServiceAddress & addrService ) const;

    /**
     * \brief   By given remote servicing Stub address checks and returns the service connection status.
     * \param   addrStub        The address of remote Stub service to check the status.
     * \return  If found registered remote Stub address, returns the actual service connection status.
     *          Otherwise, returns unknown service connection status.
     **/
    NEService::eServiceConnection getServiceStatus( const StubAddress & addrStub ) const;

    /**
     * \brief   By given remote servicing Proxy address checks and returns the service connection status.
     * \param   addrProxy       The address of remote Proxy service to check the status.
     * \return  If found registered remote Stub address as Proxy target, returns the actual service connection status.
     *          Otherwise, returns unknown service connection status.
     **/
    NEService::eServiceConnection getServiceStatus( const ProxyAddress & addrProxy ) const;

    /**
     * \brief   Returns the list of connected proxies address or empty list if no proxy is connected yet.
     * \param   addrService     The address of remote servicing stub to check.
     * \param   Returns not empty list if there is any proxy requested connection.
     *          Otherwise, returns empty list.
     **/
    const ListServiceProxies & getProxyServiceList( const ServiceAddress & addrService ) const;

    /**
     * \brief   By given Proxy address, searches and returns the registered proxy service object.
     *          Returns invalid remote service proxy object if could not find any.
     * \param   addProxy     The address of remote servicing proxy to check.
     * \return  Returns valid servicing proxy object if found entry registered.
     *          Otherwise, returns invalid object.
     **/
    const ServiceProxy & getProxyService( const ProxyAddress & addProxy ) const;

    /**
     * \brief   Registers remote Proxy address and returns the registered Stub and Proxy service object.
     *          On output, the 'out_proxyService' parameter contains the data of remote proxy service object.
     * \param   addProxy            The address of remote servicing proxy to register.
     * \param   out_proxyService    On output, contains data of remote proxy service.
     * \param   Returns servicing stub object with remote stub address and connection status data.
     **/
    const ServiceStub & registerServiceProxy( const ProxyAddress & addrProxy, ServiceProxy & out_proxyService );

    /**
     * \brief   Unregisters remote Proxy address and returns the Stub and Proxy service object.
     *          On output, the 'out_proxyService' parameter contains the data of remote proxy service object.
     * \param   addProxy            The address of remote servicing proxy to register.
     * \param   out_proxyService    On output, contains data of remote proxy service.
     * \param   Returns servicing stub object with remote stub address and connection status data.
     **/
    const ServiceStub & unregisterServiceProxy( const ProxyAddress & addrProxy, ServiceProxy & out_proxyService );

    /**
     * \brief   Registers remote Stub address and returns the registered stub service object.
     *          On output, the 'out_listProxies' parameter contains the list of proxies waiting for Stub registration
     * \param   addrStub            The address of remote servicing Stub to register.
     * \param   out_listProxies     On output, contains list of remote service proxy waiting for connection
     * \param   Returns servicing stub object with remote stub address and connection status data.
     **/
    const ServiceStub & registerServiceStub( const StubAddress & addrStub, ListServiceProxies & out_listProxies );

    /**
     * \brief   Unregisters remote Stub address and returns the registered stub service object.
     *          On output, the 'out_listProxies' parameter contains the list of proxies previously connected to Stub
     * \param   addrStub            The address of remote servicing Stub to register.
     * \param   out_listProxies     On output, contains list of remote service proxy previously connected to Stub
     * \param   Returns servicing stub object with remote stub address and connection status data.
     **/
    const ServiceStub & unregisterServiceStub( const StubAddress & addrStub, ListServiceProxies & out_listProxies );

    /**
     * \brief   Extracts the list of remote servicing Stub entries matching to generated cookie.
     * \param   cookie              The generated cookie value to filter.
     * \param   out_stubServiceList On output, contains list of remote servicing Stub objects filtered by given cookie value.
     * \param   Returns number of elements in the entry. Or zero if there is no entry.
     **/
    int getServiceList( ITEM_ID cookie , TEArrayList<StubAddress, const StubAddress &> out_stubServiceList ) const;

    /**
     * \brief   Extracts the list of remote servicing proxy entries matching to generated cookie.
     * \param   cookie                  The generated cookie value to filter.
     * \param   out_proxyServiceList    On output, contains list of remote servicing proxy objects filtered by given cookie value.
     * \param   Returns number of elements in the entry. Or zero if there is no entry.
     **/
    int getServiceList( ITEM_ID cookie , TEArrayList<ProxyAddress, const ProxyAddress &> out_proxyServiceList ) const;

    /**
     * \brief   Extracts the list of all remote servicing Stub entries.
     * \param   out_stubServiceList On output, contains list of remote servicing Stub objects.
     * \param   Returns number of elements in the entry. Or zero if there is no entry.
     **/
    int getRemoteServiceList( TEArrayList<StubAddress, const StubAddress &> & out_stubServiceList ) const;

    /**
     * \brief   Extracts the list of remote servicing proxy entries.
     * \param   addrStub                The address of remote servicing Stub to register.
     * \param   out_proxyServiceList    On output, contains list of remote servicing proxy objects.
     * \param   Returns number of elements in the entry. Or zero if there is no entry.
     **/
    int getRemoteServiceList( TEArrayList<ProxyAddress, const ProxyAddress &> & out_proxyServiceList ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Searches the entry of registered servicing by given address of service.
     * \param   addrService     The address of remote service.
     * \return  Returns valid position value if could find an entry matching given servicing address.
     *          Otherwise, returns NULL.
     **/
    MAPPOS findService( const ServiceAddress & addrService ) const;
    /**
     * \brief   Called to generate hash-key value from given service address.
     * \param   addrService     The address of remote service.
     * \return  Returns generated hash-key value of remote servicing object
     **/
    unsigned int hashKey( const ServiceAddress & addrService ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceRegistry( const ServiceRegistry & /*src*/ );
    const ServiceRegistry & operator = ( const ServiceRegistry & /*src*/ );
};

#endif  // AREG_IPC_PRIVATE_SERVICEREGISTRY_HPP
