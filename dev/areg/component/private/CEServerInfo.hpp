#ifndef AREG_COMPONENT_PRIVATE_CESERVERINFO_HPP
#define AREG_COMPONENT_PRIVATE_CESERVERINFO_HPP
/************************************************************************
 * \file        areg/component/private/CEServerInfo.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Server Info class declaration
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/CEStubAddress.hpp"
#include "areg/component/NEService.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEClientInfo;

//////////////////////////////////////////////////////////////////////////
// CEServerInfo class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Server Info is a helper class used by Server Broker,
 *          which contains information of Stub address,
 *          target Stub address, server state and Service Info objects.
 *          Every instantiated Stub is registered in Router Service,
 *          which will notify clients about service availability.
 **/
class CEServerInfo
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default Constructor.
     *          Creates Server Info with invalid Stub address and sets state Undefined.
     **/
    CEServerInfo( void );

    /**
     * \brief   Initialization constructor.
     *          Creates Server Info object, copies given address
     *          and sets state to Registered if specified server address is valid.
     * \param   server  The address of Stub server
     **/
    CEServerInfo( const CEStubAddress & server );

    /**
     * \brief   Initialization constructor.
     *          Creates Server Info object, copies server address data from proxy address.
     * \param   proxy   The Proxy address to extract data of Stub address
     **/
    CEServerInfo( const CEProxyAddress & proxy );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    CEServerInfo( const CEServerInfo & src );

    /**
     * \brief   Destructor
     **/
    ~CEServerInfo( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigning operator. Copies Server Info data from given source.
     * \param   src     The source of Server Info to copy data.
     **/
    const CEServerInfo & operator = ( const CEServerInfo & src );

    /**
     * \brief   Sets the Stub server address without changing any other information and sets Server Info state to Connected
     *          if specified Stub address is valid. Otherwise, it will set state Undefined.
     * \param   server  The stub address to set.
     **/
    const CEServerInfo & operator = ( const CEStubAddress & server );

    /**
     * \brief   Sets the Stub server address without changing any other information and sets Server Info state to Pending
     *          if specified Stub address is valid. Otherwise, it will set state Undefined.
     * \param   addService  Service address.
     **/
    const CEServerInfo & operator = ( const CEServiceAddress & addService );

    /**
     * \brief   Returns true, if 2 Server Info objects are equal. Otherwise returns false.
     * \param   other   Server Info object to compare
     **/
    bool operator == ( const CEServerInfo & other ) const;

    /**
     * \brief   Returns true, if specified and the existing Stub address objects are equal. Otherwise returns false.
     * \param   server  The Stub server address to compare
     **/
    bool operator == ( const CEStubAddress & server ) const;

    /**
     * \brief   Returns true, if the Service Info of specified Client Info object and the existing Service Info objects are equal.
     *          Otherwise returns false.
     * \param   proxy   The Proxy client address to compare
     **/
    bool operator == ( const CEProxyAddress & proxy ) const;

    /**
     * \brief   Converts Server Info object to unsigned integer.
     *          The conversion is done based on Service Info conversion mechanism.
     **/
    operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns Stub server address
     **/
    inline const CEStubAddress & GetAddress( void ) const;

    /**
     * \brief   Returns the state of Server Info.
     *          The Server Info state is updated only when Stub address is set.
     *          No separate Set-method presented
     **/
    inline NEService::eServiceConnection GetConnectionState( void ) const;

    /**
     * \brief   Changes the state of server
     * \param   serverState     The service connection state of server.
     **/
    void SetConnectionStatus( NEService::eServiceConnection newConnection );

    /**
     * \brief   Returns true if server status is Connected
     **/
    inline bool IsConnected( void ) const;

    /**
     * \brief   Returns true if server status is Pending
     **/
    inline bool IsWaiting( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The Server Stub address
     **/
    CEStubAddress                   mServerAddress;
    /**
     * \brief   The State of Server Info. State Registered set only when Stub address is valid.
     **/
    NEService::eServiceConnection  mServerState;
};

//////////////////////////////////////////////////////////////////////////
// CEServerInfo class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline const CEStubAddress& CEServerInfo::GetAddress( void ) const
{   return mServerAddress;                                  }

inline NEService::eServiceConnection CEServerInfo::GetConnectionState( void ) const
{   return mServerState;                                    }

inline bool CEServerInfo::IsConnected( void ) const
{   return mServerState == NEService::ServiceConnected;     }

inline bool CEServerInfo::IsWaiting( void ) const
{   return mServerState == NEService::ServicePending;       }

#endif  // AREG_COMPONENT_PRIVATE_CESERVERINFO_HPP
