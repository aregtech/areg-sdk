#ifndef AREG_COMPONENT_IEPROXYLISTENER_HPP
#define AREG_COMPONENT_IEPROXYLISTENER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/IEProxyListener.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Proxy Listener interface
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/NotificationEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// IEProxyListener class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A base class for all Proxy Notification event listeners.
 *          Implemented in client class to receive notifications from
 *          Proxy object. Clients also receive service connected message.
 *          When a Service Event is processed and clients needs to be 
 *          notified, the Proxy object creates internal notification
 *          and sends to all its clients. The client objects are instances
 *          of Proxy Listener to receive those notification events.
 **/
class AREG_API IEProxyListener  : public IENotificationEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Protected default constructor.
     **/
    IEProxyListener( void ) = default;

public:
    /**
     * \brief   Destructor
     **/
    virtual ~IEProxyListener( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEProxyListener overrides
/************************************************************************/

    /**
     * \brief   Triggered when receives service provider connected / disconnected event.
     *          When the service provider is connected, the client objects can set the listeners here.
     *          When the service provider is disconnected, the client object should clean the listeners.
     *          Up from connected status, the clients can subscribe and unsubscribe on updates,
     *          responses and broadcasts, can trigger requests. Before connection, the clients cannot
     *          neither trigger requests, nor receive data update messages.
     * \param   status  The service connection status.
     * \param   proxy   The Service Interface Proxy object, which is notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object.
     **/
    virtual bool serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy ) = 0;

    /**
     * \brief   Notification event processing function.
     *          Should be overwritten by every client object.
     *          Function is triggered by dispatcher when notification
     *          event is going to be processed.
     * \param   eventElem   The notification event object to process.
     **/
    virtual void processNotificationEvent( NotificationEvent & eventElem ) override = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEProxyListener );
};

#endif  // AREG_COMPONENT_IEPROXYLISTENER_HPP
