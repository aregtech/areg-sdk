#ifndef AREG_COMPONENT_PRIVATE_STUBCONNECTEVENT_HPP
#define AREG_COMPONENT_PRIVATE_STUBCONNECTEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StubConnectEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Stub Connection event class declaration.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/ServiceRequestEvent.hpp"

/**
 * \brief   Event sent by Service Manager to Stub to notify client Proxy connection status.
 **/
class StubConnectEvent  : public    ServiceRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Runtime Event
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(StubConnectEvent)   //!< Runtime data to identify event.

//////////////////////////////////////////////////////////////////////////
// Declare Runtime Event
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Event to set the address of Stub and connection status.
     *
     * \param   stubTarget          The target Stub address
     * \param   connectStatus       The connection status of Stub
     **/
    StubConnectEvent( const StubAddress & stubTarget, NEService::ServiceConnectionState connectStatus );

    /**
     * \brief   Creates event to trigger request to set Proxy connection.
     *
     * \param   proxyClient         The client Proxy address, which requests to establish connection
     * \param   stubTarget          The target Stub address, which receives request
     * \param   connectStatus       The connection status to notify.
     **/
    StubConnectEvent( const ProxyAddress & proxyClient, const StubAddress & stubTarget, NEService::ServiceConnectionState connectStatus );

    /**
     * \brief   Reads data from streaming object.
     *
     * \param   stream      The instance of streaming object, which contains information.
     **/
    StubConnectEvent( const InStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~StubConnectEvent() = default;


//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns current connection status of client Proxy.
     **/
    inline NEService::ServiceConnectionState connection_status() const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Reads and initializes event data from streaming object.
     *
     * \param   stream      The streaming object to read out event data
     * \return  Returns streaming object to read out data.
     **/
    const InStream & read_stream( const InStream & stream ) override;

    /**
     * \brief   Writes event data to streaming object.
     *
     * \param   stream      The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    OutStream & write_stream( OutStream & stream ) const override;

private:
    /**
     * \brief   The connection status set in event.
     **/
    NEService::ServiceConnectionState   mConnectionStatus;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief
     **/
    StubConnectEvent() = delete;
    AREG_NOCOPY_NOMOVE( StubConnectEvent );
};

//////////////////////////////////////////////////////////////////////////
// StubConnectEvent class inline methods.
//////////////////////////////////////////////////////////////////////////

inline NEService::ServiceConnectionState StubConnectEvent::connection_status() const
{
    return mConnectionStatus;
}

#endif  // AREG_COMPONENT_PRIVATE_STUBCONNECTEVENT_HPP
