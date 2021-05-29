#ifndef AREG_IPC_PRIVATE_SERVICESTUB_HPP
#define AREG_IPC_PRIVATE_SERVICESTUB_HPP
/************************************************************************
 * \file        areg/ipc/private/ServiceStub.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Stub Service object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/StubAddress.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class ProxyAddress;

//////////////////////////////////////////////////////////////////////////
// ServiceStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Stub service object to register at message router.
 **/
class AREG_API ServiceStub
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor, creates invalid Stub service object
     **/
    ServiceStub( void );

    /**
     * \brief   Initializes Stub address of remote service
     * \param   addrStub    The Stub address to set.
     **/
    ServiceStub( const StubAddress & addrStub );
    
    /**
     * \brief   Extracts relevant data from given Proxy address and initializes Stub service object.
     * \param   addrProxy   The Proxy address to extract information.
     **/
    ServiceStub( const ProxyAddress & addrProxy );
    
    /**
     * \brief   Copies Stub service data from given source.
     * \param   stubService     The source of Stub service object to copy data.
     **/
    ServiceStub( const ServiceStub & stubService );
    
    /**
     * \brief   Destructor
     **/
    ~ServiceStub( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies Stub address data from given source
     * \param   addrStub    The Stub address data to copy
     **/
    const ServiceStub & operator = ( const StubAddress & addrStub );

    /**
     * \brief   Creates and copies Stub address data out of Proxy address.
     * \param   addrProxy   The Proxy address to generate Stub address information
     **/
    const ServiceStub & operator = ( const ProxyAddress & addrProxy );

    /**
     * \brief   Copies data from given source
     * \param   stubService The source of Stub service to copy data.
     **/
    const ServiceStub & operator = ( const ServiceStub & stubService );

    /**
     * \brief   Checks equality of address set in Service and given Stub address
     * \param   addrStub    The address of Stub to check
     **/
    bool operator == ( const StubAddress & addrStub ) const;

    /**
     * \brief   Checks equality of Stub address set in service and Proxy address.
     *          Stub and Proxy addresses are equal if they are compatible.
     * \param   addrProxy   The Proxy address to check.
     **/
    bool operator == ( const ProxyAddress & addrProxy ) const;

    /**
     * \brief   Check equality of Stub service objects.
     * \param   stubService The Stub service object to check equality.
     **/
    bool operator == ( const ServiceStub & stubService ) const;

    /**
     * \brief   Coverts data of Stub service object into 32-bit integer value.
     **/
    operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if connection status of Stub service is connected
     **/
    inline bool isConnected( void ) const;

    /**
     * \brief   Returns true if connection status of Stub service is pending
     **/
    inline bool isWaiting( void ) const;

    /**
     * \brief   Returns the current connection status of Stub service
     **/
    inline NEService::eServiceConnection getServiceStatus( void ) const;

    /**
     * \brief   Returns Stub remote address of service
     **/
    inline const StubAddress & getServiceAddress( void ) const;

    /**
     * \brief   Returns true if Stub service object is valid.
     **/
    bool isValid( void ) const;

    /**
     * \brief   Sets new connection status of Stub service
     **/
    void setServiceStatus( NEService::eServiceConnection newStatus );

    /**
     * \brief   Sets service data. Sets Stub address and connection status.
     * \param   addrStub        The address of remote Stub to set.
     * \param   connectStatus   The connection status to set.
     **/
    void setService( const StubAddress & addrStub, NEService::eServiceConnection connectStatus = NEService::ServiceConnected );

private:
    /**
     * \brief   The address of remote Stub
     **/
    StubAddress                   mStubAddress;
    /**
     * \brief   The connection status of service.
     **/
    NEService::eServiceConnection   mConnectStatus;
};

//////////////////////////////////////////////////////////////////////////
// ServiceStub class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool ServiceStub::isConnected( void ) const
{
    return ( mConnectStatus == NEService::ServiceConnected );
}

inline bool ServiceStub::isWaiting( void ) const
{
    return ( mConnectStatus == NEService::ServicePending );
}

inline NEService::eServiceConnection ServiceStub::getServiceStatus( void ) const
{
    return mConnectStatus;
}

inline const StubAddress & ServiceStub::getServiceAddress( void ) const
{
    return mStubAddress;
}

#endif  // AREG_IPC_PRIVATE_SERVICESTUB_HPP
