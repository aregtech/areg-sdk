#ifndef AREG_COMPONENT_PRIVATE_CESERVERLIST_HPP
#define AREG_COMPONENT_PRIVATE_CESERVERLIST_HPP
/************************************************************************
 * \file        areg/src/component/private/CEServerList.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Server List declaration
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/TEHashMap.hpp"
#include "areg/src/component/private/CEServerInfo.hpp"
#include "areg/src/component/private/CEClientList.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEStubAddress;
class CEProxyAddress;

//////////////////////////////////////////////////////////////////////////
// CEServerList class declaration
//////////////////////////////////////////////////////////////////////////
class CEServerListImpl  : public TEHashMapImpl<const CEServerInfo &, const CEClientList &>
{
public:
    /**
     * \brief   Called to calculate the 32-bit hash key value.
     * \ param  Key     The object to calculate 32-bit hash key.
     * \return  Returns 32-bit hash key value.
     **/
    inline unsigned int ImplGetHashKey( const CEServerInfo & Key ) const
    {
        return static_cast<unsigned int>(Key);
    }
};

/**
 * \brief   Server List is a Hash Map class containing information
 *          of Server Info and Client List. The Key entry of Server List
 *          is Server Info and the Values are Client List. For every
 *          Stub of Service Interface, it is created List of Clients, to
 *          track the connection of Stub server and Proxies as clients.
 *          The controlling of server and connected clients done via
 *          registration of client(s).
 **/
class CEServerList  : public TEHashMap<CEServerInfo, CEClientList, const CEServerInfo &, const CEClientList &, CEServerListImpl>
{
//////////////////////////////////////////////////////////////////////////
// Predefined types and constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   CEServerList::ServiceBlock
     *          Define Block class of Hash Map as ServiceBlock. Defined to short syntax.
     **/
    typedef TEHashMap<CEServerInfo, CEClientList, const CEServerInfo &, const CEClientList &, CEServerListImpl>::CEBlock  ServiceBlock;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default Constructor. Creates empty Server List.
     **/
    CEServerList( void );
    /**
     * \brief   Destructor
     **/
    ~CEServerList( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if server with requested address is registered.
     **/
    bool IsServerRegistered( const CEStubAddress & server ) const;

    /**
     * \brief   Registers requested client by given address in the list.
     *          The function creates Client Info object, looks up whether
     *          there is registered Server existing in the system. If found,
     *          inserts Client Info in existing Client List. If did not find,
     *          creates new Server Info with Undefined state and inserts
     *          Client Info in the list.
     *          The function returns registered Client Info object.
     * \param   whichClient     The address of Proxy-client to register in the list.
     * \param   serviceType     Type of service. If ServiceAny, it will depend whether 
     *                          Proxy address is local or remote
     * \return  Returns registered Client Info object.
     **/
    const CEServerInfo & RegisterClient( const CEProxyAddress & whichClient, CEClientInfo & out_client );

    /**
     * \brief   Unregisters specified Proxy client from Server List.
     *          If operation succeeded, on output the out_client contains information
     *          of Client and returns the number of clients of Service Interface.
     *          If function returns -1, either there is no such registered client
     *          in the system, or the Stub server is in Undefined state and
     *          there are no more clients waiting for Service Interface.
     * \param   whichClient The address of Proxy client to unregister.
     * \param   out_client  On output, this will contain Client information
     * \return  If returns -1, either client was not registered, or the list
     *          of clients is empty and the server is in Undefined state.
     *          Any other value specifies number of existing client related
     *          with Service Interface.
     **/
    CEServerInfo UnregisterClient( const CEProxyAddress & whichClient, CEClientInfo & out_client );

    /**
     * \brief   Registers specified Stub Server in the list, if the address is valid, it sets Server state
     *          connected. On output, out_clientWaitingList will contain list of Client Info waiting
     *          for server connection. The function returns updated and registered Server Info object.
     * \param   server              The Stub server address to register. 
     *                              If valid, it sets Server Info state to connected.
     * \param   serviceType         Type of service. If ServiceAny, it will depend whether Server stub
     *                              address is local or remote
     * \param   out_waitingClients  On output, contains list of Client Info objects waiting for
     *                              Server to be connected.
     * \return  Returns updated and registered Server Info object.
     **/
    const CEServerInfo & RegisterServer( const CEStubAddress & addrStub, CEClientList & out_clinetList );

    /**
     * \brief   Unregisters specified Stub Server from the list and returns related
     *          to Service Interface Client Info object list.
     * \param   whichServer The address of Stub Server object to unregister
     * \return  Returns related to Service Interface Client Info object list.
     **/
    CEServerInfo UnregisterServer( const CEStubAddress & whichServer, CEClientList & out_clinetList );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the state of specified Stub server. The state is either
     *          Undefined, if Server is not registered yet, or Connected, if
     *          Server already registered and runs.
     * \param   whichServer The address of Stub Server to check the state.
     * \return  Returns CEServerInfo::SERVER_UNDEFINED state if specified Server
     *          is not registered yet, or returns CEServerInfo::SERVER_REGISTERED
     *          if Server is registered and runs.
     **/
    NEService::eServiceConnection GetServerState( const CEStubAddress & whichServer ) const;

    /**
     * \brief   Returns Client Info List related with specified server.
     * \param   whichServer The address of Stub Server to get list.
     * \return  Returns Client Info List related with specified server.
     *          If list is empty, there is no Client connected to server yet.
     **/
    const CEClientList & GetClientList( const CEStubAddress & whichServer ) const;

    /**
     * \brief   Searches the server info object for given client.
     *          Returns valid pointer if found any. Otherwise, returns null.
     * \param   whichClient     The instance of client information object to extract data for search.
     * \return  Returns valid pointer if found registered server. Otherwise, it returns null.
     **/
    const CEServerInfo * FindClientServer( const CEProxyAddress & whichClient ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    MAPPOS FindServer( const CEProxyAddress & whichClient ) const;

    MAPPOS FindServer( const CEStubAddress & whichServer ) const;

    MAPPOS FindServer( const CEServerInfo & server ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEServerList( const CEServerList & /*src*/);
    const CEServerList & operator = ( const CEServerList & /*src*/);
};

//////////////////////////////////////////////////////////////////////////
// CEServerList class inline functions
//////////////////////////////////////////////////////////////////////////

#endif  // AREG_COMPONENT_PRIVATE_CESERVERLIST_HPP
