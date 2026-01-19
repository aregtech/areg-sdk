#ifndef AREG_COMPONENT_PRIVATE_STUBCONNECTEVENT_HPP
#define AREG_COMPONENT_PRIVATE_STUBCONNECTEVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StubConnectEvent.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Stub Connection event class declaration.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/ServiceRequestEvent.hpp"

/**
 * \brief   An event sent by Service Manager to Stub to notify client Proxy connection status.
 *          Triggered by system and delivered to Sub to handle when need to change connection status.
 **/
class StubConnectEvent  : public    ServiceRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Runtime Event
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(StubConnectEvent)   //!< Runtime data to identify event.

//////////////////////////////////////////////////////////////////////////
// Declare Runtime Event
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructor. Creates Event to set the address of Sub and connection status.
     * \param   stubTarget      The target Stub address
     * \param   connectStatus   The connection status of Stub
     **/
    StubConnectEvent( const StubAddress & stubTarget, NEService::eServiceConnection connectStatus );

    /**
     * \brief   Constructor. Creates event to trigger request to set Proxy connection.
     * \param   proxyClient     The client Proxy address, which requests to establish connection
     * \param   stubTarget      The target Stub address, which receives request
     * \param   connectStatus   The connection status to notify.
     **/
    StubConnectEvent( const ProxyAddress & proxyClient, const StubAddress & stubTarget, NEService::eServiceConnection connectStatus );

    /**
     * \brief   Constructor. Reads data from streaming object.
     * \param   stream      The instance of streaming object, which contains information.
     **/
    StubConnectEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~StubConnectEvent( void ) = default;


//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns current connection status of client Proxy.
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
    virtual const IEInStream & readStream( const IEInStream & stream ) override;

    /**
     * \brief   Writes event data to streaming object
     * \param   stream  The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    virtual IEOutStream & writeStream( IEOutStream & stream ) const override;

private:
    /**
     * \brief   The connection status set in event.
     **/
    NEService::eServiceConnection   mConnectionStatus;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    StubConnectEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( StubConnectEvent );
};

//////////////////////////////////////////////////////////////////////////
// StubConnectEvent class inline methods.
//////////////////////////////////////////////////////////////////////////

inline NEService::eServiceConnection StubConnectEvent::getConnectionStatus( void ) const
{
    return mConnectionStatus;
}

#endif  // AREG_COMPONENT_PRIVATE_STUBCONNECTEVENT_HPP
