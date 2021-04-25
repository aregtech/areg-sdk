#ifndef AREG_COMPONENT_PRIVATE_CEPROXYCONNECTEVENT_HPP
#define AREG_COMPONENT_PRIVATE_CEPROXYCONNECTEVENT_HPP
/************************************************************************
 * \file        areg/src/component/private/CEProxyConnectEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Connection event class declaration.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEServiceResponseEvent.hpp"
#include "areg/src/component/CEStubAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// CEProxyConnectEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event class to send connection established notification
 *          to the Proxy object. The Event is sent from Router Service
 *          to notify Proxy object that connection with Stub is established
 *          or Proxy is disconnected from Stub. When connection is established,
 *          the Event will contain Stub address as server for Proxy and
 *          starting from that point, the Proxy and Stub can start communication.
 **/
class CEProxyConnectEvent   : public CEServiceResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class CEProxyBase;

//////////////////////////////////////////////////////////////////////////
// Declare Runtime Event
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CEProxyConnectEvent)

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
    CEProxyConnectEvent( const CEProxyAddress & proxy, const CEStubAddress & server, NEService::eServiceConnection connectStatus );

    /**
     * \brief   Copy constructor.
     * \param   target  The target Proxy address to initialize event.
     * \param   src     The source of data to copy.
     **/
    CEProxyConnectEvent(const CEProxyAddress & target, const CEProxyConnectEvent & src);

    /**
     * \brief   Initialization constructor.
     *          Creates event from streaming object and initializes data
     * \param   stream  The streaming object to read data
     **/
    CEProxyConnectEvent(const IEInStream & stream);

    /**
     * \brief   Destructor
     **/
    virtual ~CEProxyConnectEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the target address Stub object.
     **/
    inline const CEStubAddress & GetServerAddress( void ) const;

    /**
     * \brief   Returns the current connection status set in proxy connect event.
     **/
    inline NEService::eServiceConnection GetConnectionStatus( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// CEStreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Reads and initialize event data from streaming object.
     * \param   stream  The streaming object to read out event data
     * \return  Returns streaming object to read out data.
     **/
    virtual const IEInStream & ReadFromStream( const IEInStream & stream );

    /**
     * \brief   Writes event data to streaming object
     * \param   stream  The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    virtual IEOutStream & WriteToStream( IEOutStream & stream ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The address of Stub server object.
     **/
    CEStubAddress                   mImplAddress;

    /**
     * \brief   The proxy connection status.
     **/
    NEService::eServiceConnection  mConnectionStatus;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    CEProxyConnectEvent( void );
    CEProxyConnectEvent(const CEProxyConnectEvent &);
    const CEProxyConnectEvent& operator = (const CEProxyConnectEvent &);
};

//////////////////////////////////////////////////////////////////////////
// CEProxyConnectEvent class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline const CEStubAddress & CEProxyConnectEvent::GetServerAddress( void ) const
{   return mImplAddress;        }

inline NEService::eServiceConnection CEProxyConnectEvent::GetConnectionStatus( void ) const
{   return mConnectionStatus;   }

#endif  // AREG_COMPONENT_PRIVATE_CEPROXYCONNECTEVENT_HPP
