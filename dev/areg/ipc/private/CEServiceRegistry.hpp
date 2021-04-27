#ifndef AREG_IPC_PRIVATE_CESERVICEREGISTRY_HPP
#define AREG_IPC_PRIVATE_CESERVICEREGISTRY_HPP
/************************************************************************
 * \file        areg/ipc/private/CEServiceRegistry.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Service Manager
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEHashMap.hpp"
#include "areg/ipc/private/CEServiceStub.hpp"
#include "areg/ipc/private/CEListServiceProxies.hpp"
#include "areg/base/TEArrayList.hpp"

//////////////////////////////////////////////////////////////////////////
// CEServiceRegistry class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CEServiceRegistryImpl    : public TEHashMapImpl<const CEServiceStub &, const CEListServiceProxies &>
{
public:
    /**
     * \brief   Called to calculate the 32-bit hash key value.
     * \ param  Key     The object to calculate 32-bit hash key.
     * \return  Returns 32-bit hash key value.
     **/
    inline unsigned int ImplHashKey( const CEServiceStub & Key ) const
    {
        return static_cast<unsigned int>(static_cast<const CEServiceAddress &>(Key.GetServiceAddress()));
    }
};

/**
 * \brief   The remote services registration map, which is a map of stub and list of connected proxies.
 **/
class AREG_API CEServiceRegistry  : public TEHashMap<CEServiceStub, CEListServiceProxies, const CEServiceStub &, const CEListServiceProxies &, CEServiceRegistryImpl>
{
//////////////////////////////////////////////////////////////////////////
// Predefined types and constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   CEServiceMap::ServiceBlock
     *          Defines Block class of Hash Map as ServiceBlock. Defined to short syntax.
     **/
    typedef TEHashMap<CEServiceStub, CEListServiceProxies, const CEServiceStub &, const CEListServiceProxies &, CEServiceRegistryImpl>::CEBlock ServiceBlock;

    /**
     * \brief   CEServiceMap::InvalidStubService
     *          Defines invalid stub service
     **/
    static const CEServiceStub          InvalidStubService;

    /**
     * \brief   CEServiceMap::EmptyProxiesList
     *          Defines empty list of remote connected proxies.
     **/
    static const CEListServiceProxies   EmptyProxiesList;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    CEServiceRegistry( void );
    /**
     * \brief   Destructor
     **/
    ~CEServiceRegistry( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if passed stub service is already registered.
     * \param   addrStub    The remote servicing stub object to check.
     **/
    bool IsServiceRegistered( const CEStubAddress & addrStub ) const;

    /**
     * \brief   Returns true if passed proxy address is already registered.
     * \param   addrProxy   The address of proxy to check.
     **/
    bool IsServiceRegistered( const CEProxyAddress & addrProxy ) const;

    /**
     * \brief   By given remote servicing address finds the address of stub,
     *          if there is any, or returns invalid stub address.
     * \param   addrService     The servicing address to check.
     * \return  Returns valid Stub address if it is registered. Otherwise, returns invalid Stub address.
     **/
    const CEServiceStub & GetStubService( const CEServiceAddress & addrService ) const;

    /**
     * \brief   By given remote servicing Stub address checks and returns the service connection status.
     * \param   addrStub        The address of remote Stub service to check the status.
     * \return  If found registered remote Stub address, returns the actual service connection status.
     *          Otherwise, returns unknown service connection status.
     **/
    NEService::eServiceConnection GetServiceStatus( const CEStubAddress & addrStub ) const;

    /**
     * \brief   By given remote servicing Proxy address checks and returns the service connection status.
     * \param   addrProxy       The address of remote Proxy service to check the status.
     * \return  If found registered remote Stub address as Proxy target, returns the actual service connection status.
     *          Otherwise, returns unknown service connection status.
     **/
    NEService::eServiceConnection GetServiceStatus( const CEProxyAddress & addrProxy ) const;

    /**
     * \brief   Returns the list of connected proxies address or empty list if no proxy is connected yet.
     * \param   addrService     The address of remote servicing stub to check.
     * \param   Returns not empty list if there is any proxy requested connection.
     *          Otherwise, returns empty list.
     **/
    const CEListServiceProxies & GetProxyServiceList( const CEServiceAddress & addrService ) const;

    /**
     * \brief   By given Proxy address, searches and returns the registered proxy service object.
     *          Returns invalid remote service proxy object if could not find any.
     * \param   addProxy     The address of remote servicing proxy to check.
     * \return  Returns valid servicing proxy object if found entry registered.
     *          Otherwise, returns invalid object.
     **/
    const CEServiceProxy & GetProxyService( const CEProxyAddress & addProxy ) const;

    /**
     * \brief   Registers remote Proxy address and returns the registered Stub and Proxy service object.
     *          On output, the 'out_proxyService' parameter contains the data of remote proxy service object.
     * \param   addProxy            The address of remote servicing proxy to register.
     * \param   out_proxyService    On output, contains data of remote proxy service.
     * \param   Returns servicing stub object with remote stub address and connection status data.
     **/
    const CEServiceStub & RegisterServiceProxy( const CEProxyAddress & addrProxy, CEServiceProxy & out_proxyService );

    /**
     * \brief   Unregisters remote Proxy address and returns the Stub and Proxy service object.
     *          On output, the 'out_proxyService' parameter contains the data of remote proxy service object.
     * \param   addProxy            The address of remote servicing proxy to register.
     * \param   out_proxyService    On output, contains data of remote proxy service.
     * \param   Returns servicing stub object with remote stub address and connection status data.
     **/
    const CEServiceStub & UnregisterServiceProxy( const CEProxyAddress & addrProxy, CEServiceProxy & out_proxyService );

    /**
     * \brief   Registers remote Stub address and returns the registered stub service object.
     *          On output, the 'out_listProxies' parameter contains the list of proxies waiting for Stub registration
     * \param   addrStub            The address of remote servicing Stub to register.
     * \param   out_listProxies     On output, contains list of remote service proxy waiting for connection
     * \param   Returns servicing stub object with remote stub address and connection status data.
     **/
    const CEServiceStub & RegisterServiceStub( const CEStubAddress & addrStub, CEListServiceProxies & out_listProxies );

    /**
     * \brief   Unregisters remote Stub address and returns the registered stub service object.
     *          On output, the 'out_listProxies' parameter contains the list of proxies previously connected to Stub
     * \param   addrStub            The address of remote servicing Stub to register.
     * \param   out_listProxies     On output, contains list of remote service proxy previously connected to Stub
     * \param   Returns servicing stub object with remote stub address and connection status data.
     **/
    const CEServiceStub & UnregisterServiceStub( const CEStubAddress & addrStub, CEListServiceProxies & out_listProxies );

    /**
     * \brief   Extracts the list of remote servicing Stub entries matching to generated cookie.
     * \param   cookie              The generated cookie value to filter.
     * \param   out_stubServiceList On output, contains list of remote servicing Stub objects filtered by given cookie value.
     * \param   Returns number of elements in the entry. Or zero if there is no entry.
     **/
    int GetServiceList( ITEM_ID cookie , TEArrayList<CEStubAddress, const CEStubAddress &> out_stubServiceList ) const;

    /**
     * \brief   Extracts the list of remote servicing proxy entries matching to generated cookie.
     * \param   cookie                  The generated cookie value to filter.
     * \param   out_proxyServiceList    On output, contains list of remote servicing proxy objects filtered by given cookie value.
     * \param   Returns number of elements in the entry. Or zero if there is no entry.
     **/
    int GetServiceList( ITEM_ID cookie , TEArrayList<CEProxyAddress, const CEProxyAddress &> out_proxyServiceList ) const;

    /**
     * \brief   Extracts the list of all remote servicing Stub entries.
     * \param   out_stubServiceList On output, contains list of remote servicing Stub objects.
     * \param   Returns number of elements in the entry. Or zero if there is no entry.
     **/
    int GetRemoteServiceList( TEArrayList<CEStubAddress, const CEStubAddress &> & out_stubServiceList ) const;

    /**
     * \brief   Extracts the list of remote servicing proxy entries.
     * \param   addrStub                The address of remote servicing Stub to register.
     * \param   out_proxyServiceList    On output, contains list of remote servicing proxy objects.
     * \param   Returns number of elements in the entry. Or zero if there is no entry.
     **/
    int GetRemoteServiceList( TEArrayList<CEProxyAddress, const CEProxyAddress &> & out_proxyServiceList ) const;

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
    MAPPOS findService( const CEServiceAddress & addrService ) const;
    /**
     * \brief   Called to generate hash-key value from given service address.
     * \param   addrService     The address of remote service.
     * \return  Returns generated hash-key value of remote servicing object
     **/
    unsigned int hashKey( const CEServiceAddress & addrService ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEServiceRegistry( const CEServiceRegistry & /*src*/ );
    const CEServiceRegistry & operator = ( const CEServiceRegistry & /*src*/ );
};

#endif  // AREG_IPC_PRIVATE_CESERVICEREGISTRY_HPP
