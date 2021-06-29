#ifndef AREG_COMPONENT_IEPROXYLISTENER_HPP
#define AREG_COMPONENT_IEPROXYLISTENER_HPP
/************************************************************************
 * \file        areg/component/IEProxyListener.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
 * \brief       A base class for all Proxy Notification event listeners.
 *              Implemented in client class to receive notifications
 *              from Proxy object. Clients also receive service
 *              connected message.
 * 
 * \details     When a Service Event is processed and clients needs to be
 *              notified, Proxy object creates internal notification 
 *              and sends to all its clients. The client objects should
 *              be instances of Proxy Listener to receive those 
 *              notification events.
 *
 **/
class AREG_API IEProxyListener  : public IENotificationEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected default constructor.
     **/
    IEProxyListener( void );

public:
    /**
     * \brief   Destructor
     **/
    virtual ~IEProxyListener( void );

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
    IEProxyListener( const IEProxyListener & /*src*/ );
    const IEProxyListener & operator = ( const IEProxyListener & /*src*/ );
};

#endif  // AREG_COMPONENT_IEPROXYLISTENER_HPP
