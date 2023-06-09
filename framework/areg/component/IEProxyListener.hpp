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
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
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
     * \brief   Triggered by Proxy, when gets service connected event.
     *          Make client initializations in this function. No request
     *          will be processed until this function is not called for
     *          client object. Also set listeners here if client is interested
     *          to receive update notifications.
     * \param   isConnected     The flag, indicating whether service is connected
     *                          or disconnected. Make cleanups and stop sending
     *                          requests or assigning for notification if
     *                          this flag is false. No event to Stub target will
     *                          be sent, until no service connected event is
     *                          received.
     * \param   proxy           The Service Interface Proxy object, which is
     *                          notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object,
     *          i.e. if passed Proxy address is equal to the Proxy object that client has.
     *          If Proxy objects are not equal, it should return false;
     *
     * \example serviceConnected calls
     * 
     *          The code implementation of Service Client Base should look like this:
     *
     *          bool result = false;
     *          if (mProxy == &proxy)
     *          {
     *              mIsConnected= isConnected;
     *              result      = true;
     *          }
     *          return result;
     **/
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy ) = 0;

    /**
     * \brief   Notification event processing function.
     *          Should be overwritten by every client object.
     *          Function is triggered by dispatcher when notification
     *          event is going to be processed.
     * \param   eventElem   The notification event object to process.
     **/
    virtual void processNotificationEvent( NotificationEvent & eventElem ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEProxyListener );
};

#endif  // AREG_COMPONENT_IEPROXYLISTENER_HPP
