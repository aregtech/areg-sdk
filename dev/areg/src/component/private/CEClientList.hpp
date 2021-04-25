#ifndef AREG_COMPONENT_PRIVATE_CECLIENTLIST_HPP
#define AREG_COMPONENT_PRIVATE_CECLIENTLIST_HPP
/************************************************************************
 * \file        areg/src/component/private/CEClientList.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Client List declaration
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/TELinkedList.hpp"
#include "areg/src/component/private/CEClientInfo.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEProxyAddress;
class CEServerInfo;

//////////////////////////////////////////////////////////////////////////
// CEClientList class declaration
//////////////////////////////////////////////////////////////////////////
class CEClientListImpl  : public TEListImpl<const CEClientInfo &>
{
public:
    /**
     * \brief   Compares 2 values, returns true if they are equal.
     * \param   Value1  The right-side value to compare.
     * \param   Value2  The left-side value to compare.
     * \return  Returns true if 2 values are equal.
     **/
    inline bool ImplEqualValues( const CEClientInfo & value1, const CEClientInfo & value2) const
    {
        return ( value1.GetAddress() == value2.GetAddress() && value1.GetAddress().GetCookie() == value2.GetAddress().GetCookie() );
    }
};
/**
 * \brief   CEClientList is a linked list object containing the list
 *          of Client Info objects related to one server Stub address.
 *          The object is used in Router Service to control clients.
 **/
class CEClientList    : public TELinkedList<CEClientInfo, const CEClientInfo &, CEClientListImpl>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty list.
     **/
    CEClientList( void );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    CEClientList( const CEClientList & src );

    /**
     * \brief   Destructor
     **/
    ~CEClientList( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigning operator. Copies the list of Client Info from given source
     * \param   src     The source of Client Info list
     **/
    const CEClientList & operator = ( const CEClientList & src );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Checks whether there is a Client Info with specified Proxy address
     *          exists in the list, and if exists, returns true.
     *          Otherwise, it returns false.
     * \param   client  The address of Proxy of client to check.
     * \return  Returns true, if there is a Client Info object in the list
     *          with specified Proxy address. Otherwise returns false.
     **/
    bool ClientExist( const CEProxyAddress & client ) const;

    /**
     * \brief   Searches in the list Client Info object with specified address,
     *          and there is Client Info object with specified Proxy address,
     *          it will return valid Client Info. Otherwise, it will return
     *          invalid CEClientInfo::INVALID_CLIENT_INFO object.
     *          Check validation of Client Info or call ClientExist() method.
     * \param   whichClient The address of Proxy of client to search.
     * \return  If there is a Client Info object in the list with specified address
     *          of Proxy of client, it will return valid object. Otherwise, it will return
     *          invalid CEClientInfo::INVALID_CLIENT_INFO object.
     **/
    const CEClientInfo & GetClient( const CEProxyAddress & whichClient ) const;

    /**
     * \brief   Registers client in the list by given address of Proxy and Server Info, 
     *          containing address of Stub and state of server, and returns 
     *          Client Info object of registered client.  If there is  already 
     *          Client Info object exists in the list, it will increase number
     *          of instances and return existing object. Otherwise, it will add new 
     *          Client Info element.
     * \param   whichClient The address of Proxy of client
     * \param   server      The Server Info object, containing Stub address.
     * \return  If there is already registered client in the list, it will increase
     *          the number of instances by one and will return existing Client Info
     *          object. Otherwise, it will create new Client Info entry with initial
     *          number of instances 1.
     **/
    const CEClientInfo & RegisterClient( const CEProxyAddress & whichClient, const CEServerInfo & server );

    /**
     * \brief   It searches servicing Client entry in the list by the given address of Proxy.
     *          If requested proxy address found in the list of servicing client, on output, 
     *          the out_client contains information of unregistered client. Otherwise, it is unchanged.
     * \param   whichClient The address of Proxy to search
     * \param   out_client  If a servicing client with given proxy address exists, on output, this 
     *                      will contains information of unregistered client.
     * \return  Return true if found an entry with specified proxy and operation to unregister client succeeded.
     **/
    bool UnregisterClient( const CEProxyAddress & whichClient, CEClientInfo & out_client );

    /**
     * \brief   Call to notify all pending clients the servicing server availability.
     *          The function changes states of waiting for connection clients and on output, 
     *          the out_clientList parameter contains list of all clients.
     * \param   whichServer     The servicing server helper object, which is available.
     * \param   out_clientList  On output it contains list of connected clients.
     **/
    void SetServerAvailable( const CEServerInfo & whichServer, CEClientList & out_clientList );

    /**
     * \brief   Called to change the state of connected clients when server is disconnected. 
     *          If there are connected clients, on output out_clientList parameter contains list of
     *          disconnected clients.
     * \param   out_clientList  On output, this contains list of disconnected client.
     **/
    void SetServerUnavailable( CEClientList & out_clientList );
};

#endif  // AREG_COMPONENT_PRIVATE_CECLIENTLIST_HPP
