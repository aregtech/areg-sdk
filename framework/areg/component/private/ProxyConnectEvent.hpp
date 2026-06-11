#ifndef AREG_COMPONENT_PRIVATE_PROXYCONNECTEVENT_HPP
#define AREG_COMPONENT_PRIVATE_PROXYCONNECTEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ProxyConnectEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Proxy Connection event class declaration.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/StubAddress.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ProxyConnectEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event sent to Proxy to notify connection status with Stub.
 *          All data is stored in EventHeader: connection status in result field,
 *          stub information in the provider endpoint.
 **/
class ProxyConnectEvent final  : public ServiceResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class ProxyBase;

//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(ProxyConnectEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes Connection Event indicating to Proxy that connection with Stub is
     *          established or lost. Stores connection status in EventHeader.result and stub
     *          address information in EventHeader.provider.
     *
     * \param   proxy               The address of Proxy to send Connection Event.
     * \param   server              The address of Stub object.
     * \param   connectStatus       Indicates the connection status.
     **/
    ProxyConnectEvent( const ProxyAddress & proxy, const StubAddress & server, areg::ServiceConnectionState connectStatus );

    /**
     * \brief   Clones event data for a different target Proxy address. The full EventHeader
     *          (including result and provider endpoint) is copied; only the consumer endpoint
     *          is updated to the new target.
     *
     * \param   target      The target Proxy address to initialize event.
     * \param   src         The source of data to copy.
     **/
    ProxyConnectEvent(const ProxyAddress & target, const ProxyConnectEvent & src);

    ProxyConnectEvent(const ProxyConnectEvent& /*src*/) = default;

    ProxyConnectEvent(ProxyConnectEvent&& /*src*/) noexcept = default;

    ~ProxyConnectEvent() override = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the target address of Stub object, reconstructed from EventHeader.provider.
     **/
    StubAddress stub_address() const;

    /**
     * \brief   Returns the current connection status, read from EventHeader.result.
     **/
    inline areg::ServiceConnectionState connection_status() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    ProxyConnectEvent() = delete;
};

//////////////////////////////////////////////////////////////////////////
// ProxyConnectEvent class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline areg::ServiceConnectionState ProxyConnectEvent::connection_status() const noexcept
{
    return static_cast<areg::ServiceConnectionState>(result());
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_PROXYCONNECTEVENT_HPP
