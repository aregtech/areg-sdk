#ifndef AREG_COMPONENT_PROXYLISTENER_HPP
#define AREG_COMPONENT_PROXYLISTENER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ProxyListener.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Proxy Listener interface
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/NotificationEvent.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ProxyListener class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for client objects to receive notifications from a proxy when the service
 *          connects/disconnects or when attributes, broadcasts, and responses are received.
 **/
class AREG_API ProxyListener  : public NotificationConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ProxyListener() = default;

public:
    virtual ~ProxyListener() = default;

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// ProxyListener overrides
/************************************************************************/

    /**
     * \brief   Triggered when the service provider connects or disconnects. Returns true if the
     *          notification is relevant to the client.
     *
     * \param   status      The service connection status (connected or disconnected).
     * \param   proxy       The service interface proxy object notifying of the connection state.
     * \return  Return true if this notification is relevant to the client; false otherwise.
     * \note    When connected, clients can subscribe to notifications and trigger requests. When
     *          disconnected, clients should clean up listeners.
     **/
    virtual bool service_connected( areg::ServiceConnectionState status, ProxyBase & proxy ) = 0;

    /**
     * \brief   Processes a notification event from the proxy. Overwrite to handle attribute
     *          updates, broadcasts, and response notifications.
     *
     * \param   eventElem       The notification event object to process.
     **/
    virtual void process_notification_event( NotificationEvent & eventElem ) override = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( ProxyListener );
};

} // namespace areg
#endif  // AREG_COMPONENT_PROXYLISTENER_HPP
