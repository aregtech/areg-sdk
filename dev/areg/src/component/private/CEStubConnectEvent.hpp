#ifndef AREG_COMPONENT_PRIVATE_CESTUBCONNECTEVENT_HPP
#define AREG_COMPONENT_PRIVATE_CESTUBCONNECTEVENT_HPP
/************************************************************************
 * \file        areg/src/component/private/CEStubConnectEvent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Stub Connection event class declaration.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEServiceRequestEvent.hpp"

/**
 * \brief   An event sent by Service Manager to Stub to notify client Proxy connection status.
 *          Triggered by system and delivered to Sub to handle when need to change connection status.
 **/
class CEStubConnectEvent    : public    CEServiceRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Runtime Event
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CEStubConnectEvent)   //!< Runtime data to identify event.

//////////////////////////////////////////////////////////////////////////
// Declare Runtime Event
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructor. Creates Event to set the address of Sub and connection status.
     * \param   stubTarget      The target Stub address
     * \param   connectStatus   The connection status of Stub
     **/
    CEStubConnectEvent( const CEStubAddress & stubTarget, NEService::eServiceConnection connectStatus );

    /**
     * \brief   Constructor. Creates event to trigger request to set Proxy connection.
     * \param   proxyClient     The client Proxy address, which requests to establish connection
     * \param   stubTarget      The target Stub address, which receives request
     * \param   connectStatus   The connection status to notify.
     **/
    CEStubConnectEvent( const CEProxyAddress & proxyClient, const CEStubAddress & stubTarget, NEService::eServiceConnection connectStatus );

    /**
     * \brief   Constructor. Reads data from streaming object.
     * \param   stream      The instance of streaming object, which contains information.
     **/
    CEStubConnectEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEStubConnectEvent( void );


//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns current connection status of client Proxy.
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

private:
    /**
     * \brief   The connection status set in event.
     **/
    NEService::eServiceConnection  mConnectionStatus;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEStubConnectEvent( void );
    CEStubConnectEvent( const CEStubConnectEvent & /*src*/ );
    const CEStubConnectEvent & operator = ( const CEStubConnectEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEStubConnectEvent class inline methods.
//////////////////////////////////////////////////////////////////////////

inline NEService::eServiceConnection CEStubConnectEvent::GetConnectionStatus( void ) const
{   return mConnectionStatus;       }

#endif  // AREG_COMPONENT_PRIVATE_CESTUBCONNECTEVENT_HPP
