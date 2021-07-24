#ifndef AREG_COMPONENT_PRIVATE_PROXYCONNECTEVENT_HPP
#define AREG_COMPONENT_PRIVATE_PROXYCONNECTEVENT_HPP
/************************************************************************
 * \file        areg/component/private/ProxyConnectEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Connection event class declaration.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/StubAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// ProxyConnectEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event class to send connection established notification
 *          to the Proxy object. The Event is sent from Router Service
 *          to notify Proxy object that connection with Stub is established
 *          or Proxy is disconnected from Stub. When connection is established,
 *          the Event will contain Stub address as server for Proxy and
 *          starting from that point, the Proxy and Stub can start communication.
 **/
class ProxyConnectEvent   : public ServiceResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class ProxyBase;

//////////////////////////////////////////////////////////////////////////
// Declare Runtime Event
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(ProxyConnectEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Initialize Connection Event, which
     *          is indicating to Proxy specified in address that the connection
     *          with Stub, specified in address, either is established or lost.
     * \param   proxy       The address of Proxy to send Connection Event.
     * \param   server      The address of Stub object. If valid, Proxy can send request events.
     * \param   isConnected The flag, indicating whether Proxy established connection or it is disconnected
     **/
    ProxyConnectEvent( const ProxyAddress & proxy, const StubAddress & server, NEService::eServiceConnection connectStatus );

    /**
     * \brief   Copy constructor.
     * \param   target  The target Proxy address to initialize event.
     * \param   src     The source of data to copy.
     **/
    ProxyConnectEvent(const ProxyAddress & target, const ProxyConnectEvent & src);

    /**
     * \brief   Initialization constructor.
     *          Creates event from streaming object and initializes data
     * \param   stream  The streaming object to read data
     **/
    ProxyConnectEvent(const IEInStream & stream);

    /**
     * \brief   Destructor
     **/
    virtual ~ProxyConnectEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the target address Stub object.
     **/
    inline const StubAddress & getStubAddress( void ) const;

    /**
     * \brief   Returns the current connection status set in proxy connect event.
     **/
    inline NEService::eServiceConnection getConnectionStatus( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Reads and initialize event data from streaming object.
     * \param   stream  The streaming object to read out event data
     * \return  Returns streaming object to read out data.
     **/
    virtual const IEInStream & readStream( const IEInStream & stream );

    /**
     * \brief   Writes event data to streaming object
     * \param   stream  The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    virtual IEOutStream & writeStream( IEOutStream & stream ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The address of Stub server object.
     **/
    StubAddress                     mStubAddress;

    /**
     * \brief   The proxy connection status.
     **/
    NEService::eServiceConnection   mConnectionStatus;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    ProxyConnectEvent( void );
    ProxyConnectEvent(const ProxyConnectEvent &);
    const ProxyConnectEvent& operator = (const ProxyConnectEvent &);
};

//////////////////////////////////////////////////////////////////////////
// ProxyConnectEvent class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline const StubAddress & ProxyConnectEvent::getStubAddress( void ) const
{
    return mStubAddress;
}

inline NEService::eServiceConnection ProxyConnectEvent::getConnectionStatus( void ) const
{
    return mConnectionStatus;
}

#endif  // AREG_COMPONENT_PRIVATE_PROXYCONNECTEVENT_HPP
