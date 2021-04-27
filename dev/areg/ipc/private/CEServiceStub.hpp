#ifndef AREG_IPC_PRIVATE_CESERVICESTUB_HPP
#define AREG_IPC_PRIVATE_CESERVICESTUB_HPP
/************************************************************************
 * \file        areg/ipc/private/CEServiceStub.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Stub Service object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/CEStubAddress.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEProxyAddress;

//////////////////////////////////////////////////////////////////////////
// CEServiceStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Stub service object to register at broker.
 **/
class AREG_API CEServiceStub
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor, creates invalid Stub service object
     **/
    CEServiceStub( void );

    /**
     * \brief   Initializes Stub address of remote service
     * \param   addrStub    The Stub address to set.
     **/
    CEServiceStub( const CEStubAddress & addrStub );
    
    /**
     * \brief   Extracts relevant data from given Proxy address and initializes Stub service object.
     * \param   addrProxy   The Proxy address to extract information.
     **/
    CEServiceStub( const CEProxyAddress & addrProxy );
    
    /**
     * \brief   Copies Stub service data from given source.
     * \param   stubService     The source of Stub service object to copy data.
     **/
    CEServiceStub( const CEServiceStub & stubService );
    
    /**
     * \brief   Destructor
     **/
    ~CEServiceStub( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies Stub address data from given source
     * \param   addrStub    The Stub address data to copy
     **/
    const CEServiceStub & operator = ( const CEStubAddress & addrStub );

    /**
     * \brief   Creates and copies Stub address data out of Proxy address.
     * \param   addrProxy   The Proxy address to generate Stub address information
     **/
    const CEServiceStub & operator = ( const CEProxyAddress & addrProxy );

    /**
     * \brief   Copies data from given source
     * \param   stubService The source of Stub service to copy data.
     **/
    const CEServiceStub & operator = ( const CEServiceStub & stubService );

    /**
     * \brief   Checks equality of address set in Service and given Stub address
     * \param   addrStub    The address of Stub to check
     **/
    bool operator == ( const CEStubAddress & addrStub ) const;

    /**
     * \brief   Checks equality of Stub address set in service and Proxy address.
     *          Stub and Proxy addresses are equal if they are compatible.
     * \param   addrProxy   The Proxy address to check.
     **/
    bool operator == ( const CEProxyAddress & addrProxy ) const;

    /**
     * \brief   Check equality of Stub service objects.
     * \param   stubService The Stub service object to check equality.
     **/
    bool operator == ( const CEServiceStub & stubService ) const;

    /**
     * \brief   Coverts data of Stub service object into 32-bit integer value.
     **/
    operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if connection status of Stub service is connected
     **/
    inline bool IsConnected( void ) const;

    /**
     * \brief   Returns true if connection status of Stub service is pending
     **/
    inline bool IsWaiting( void ) const;

    /**
     * \brief   Returns the current connection status of Stub service
     **/
    inline NEService::eServiceConnection GetServiceStatus( void ) const;

    /**
     * \brief   Returns Stub remote address of service
     **/
    inline const CEStubAddress & GetServiceAddress( void ) const;

    /**
     * \brief   Returns true if Stub service object is valid.
     **/
    bool IsValid( void ) const;

    /**
     * \brief   Sets new connection status of Stub service
     **/
    void SetServiceStatus( NEService::eServiceConnection newStatus );

    /**
     * \brief   Sets service data. Sets Stub address and connection status.
     * \param   addrStub        The address of remote Stub to set.
     * \param   connectStatus   The connection status to set.
     **/
    void SetService( const CEStubAddress & addrStub, NEService::eServiceConnection connectStatus = NEService::ServiceConnected );

private:
    /**
     * \brief   The address of remote Stub
     **/
    CEStubAddress                   mStubAddress;
    /**
     * \brief   The connection status of service.
     **/
    NEService::eServiceConnection   mConnectStatus;
};

//////////////////////////////////////////////////////////////////////////
// CEServiceStub class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool CEServiceStub::IsConnected( void ) const
{   return ( mConnectStatus == NEService::ServiceConnected );  }

inline bool CEServiceStub::IsWaiting( void ) const
{   return ( mConnectStatus == NEService::ServicePending );    }

inline NEService::eServiceConnection CEServiceStub::GetServiceStatus( void ) const
{   return mConnectStatus;                                      }

inline const CEStubAddress & CEServiceStub::GetServiceAddress( void ) const
{   return mStubAddress;                                        }

#endif  // AREG_IPC_PRIVATE_CESERVICESTUB_HPP
