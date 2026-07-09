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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/StubConnectEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Stub Connection event class declaration.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/ServiceRequestEvent.hpp"
namespace areg {

/**
 * \brief   Event sent by Service Manager to Stub to notify client Proxy connection status.
 *          All data lives in EventHeader; no extra member variables.
 *
 *          EventHeader layout:
 *            - provider endpoint : StubAddress  (target stub)
 *            - consumer endpoint : ProxyAddress (connecting proxy; invalid for ServiceConnection)
 *            - messageId         : FuncIdRange::ResponseServiceProviderConnection
 *            - result            : ServiceConnectionState
 *
 *          ServiceConnection vs ClientConnection is distinguished by consumer (proxy) validity:
 *            - consumer invalid: ServiceConnection (stub registered with service manager)
 *            - consumer valid  : ClientConnection  (proxy client connecting/disconnecting)
 **/
class StubConnectEvent final  : public    ServiceRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(StubConnectEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates event to notify stub about its own registration status.
     *
     * \param   stubTarget          The target Stub address
     * \param   connectStatus       The connection status of Stub
     **/
    StubConnectEvent( const StubAddress & stubTarget, areg::ServiceConnectionState connectStatus );

    /**
     * \brief   Creates event to notify stub that a proxy client is connecting or disconnecting.
     *
     * \param   proxyClient         The client Proxy address
     * \param   stubTarget          The target Stub address
     * \param   connectStatus       The connection status to notify.
     **/
    StubConnectEvent( const ProxyAddress & proxyClient, const StubAddress & stubTarget, areg::ServiceConnectionState connectStatus );

    StubConnectEvent(const StubConnectEvent& /*src*/) = default;

    StubConnectEvent(StubConnectEvent&& /*src*/) noexcept = default;

    ~StubConnectEvent() override = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns current connection status, read from EventHeader result field.
     **/
    inline areg::ServiceConnectionState connection_status() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    StubConnectEvent() = delete;
};

//////////////////////////////////////////////////////////////////////////
// StubConnectEvent class inline methods.
//////////////////////////////////////////////////////////////////////////

inline areg::ServiceConnectionState StubConnectEvent::connection_status() const noexcept
{
    return static_cast<areg::ServiceConnectionState>(result());
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_STUBCONNECTEVENT_HPP
