//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SimpleTrafficLightProxy.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PRIVATE_SIMPLETRAFFICLIGHTPROXY_HPP
#define  GENERATED_SRC_PRIVATE_SIMPLETRAFFICLIGHTPROXY_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SimpleTrafficLight.
 * Generated at     11.08.2021  13:18:20 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/SimpleTrafficLightProxy.hpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficLight Service Interface Proxy class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NESimpleTrafficLight.hpp"
#include "areg/component/ProxyBase.hpp"
 
/************************************************************************
 * Dependencies
 ************************************************************************/
class SimpleTrafficLightResponseEvent;
 
//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightProxy class declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       SimpleTrafficLight Service Interface proxy class.
 *              Automatically instantiated per thread at client startup.
 *              Used to communicate with stub object and sending notifications
 *              to each client running in the thread instance.
 *
 *              The simple traffic light that triggers timeout and changes traffic light state.
 **/
class SimpleTrafficLightProxy   : public ProxyBase
{

private:
    //////////////////////////////////////////////////////////////////////////
    // SimpleTrafficLightProxy::SimpleTrafficLightServiceAvailableEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief       The event is used to immediately notify new client
     *              about service availability and escape duplicating 
     *              notification for other, already connected clients.
     *  
     **/
    class SimpleTrafficLightServiceAvailableEvent : public ProxyBase::ServiceAvailableEvent
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT(SimpleTrafficLightProxy::SimpleTrafficLightServiceAvailableEvent)

    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        SimpleTrafficLightServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor
         **/
        virtual ~SimpleTrafficLightServiceAvailableEvent( void );
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        SimpleTrafficLightServiceAvailableEvent( void );
        SimpleTrafficLightServiceAvailableEvent( const SimpleTrafficLightServiceAvailableEvent & /*src*/ );
        const SimpleTrafficLightServiceAvailableEvent & operator = ( const SimpleTrafficLightServiceAvailableEvent & /*src*/ );
    };
//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightProxy predefined constants and static members.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The instance of thread to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static SimpleTrafficLightProxy * createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread );

    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The name of thread instance to dispatch messages.
     *                          If NULL, current dispatching thread is used to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static SimpleTrafficLightProxy * createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread = static_cast<const char *>(NULL) );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructor. Creates Proxy object by given role name. 
     *          Do not create proxy object directly, instead use CreateProxy function.
     * \param   roleName    The role name of proxy, i.e. role name of server component object.
     * \param   ownerThread The instance of thread to dispatch messages.
     *                      If NULL, current dispatching thread is used to dispatch messages.
     **/
    SimpleTrafficLightProxy( const char * roleName, DispatcherThread * ownerThread = static_cast<DispatcherThread *>(NULL) );
    /**
     * \brief   Destructor
     **/
    virtual ~SimpleTrafficLightProxy( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attributes
 ************************************************************************/
    /**
     * \brief   Returns true if value of SouthNorth attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isSouthNorthValid( void ) const;
    /**
     * \brief   Returns the value of SouthNorth attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute SouthNorth description: 
     *          The light state of south-north traffic.
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     **/
    inline NESimpleTrafficLight::eTrafficLight getSouthNorth( NEService::eDataStateType & state ) const;

    /**
     * \brief   Returns true if value of EastWest attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isEastWestValid( void ) const;
    /**
     * \brief   Returns the value of EastWest attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute EastWest description: 
     *          The light state of east-west traffic.
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     **/
    inline NESimpleTrafficLight::eTrafficLight getEastWest( NEService::eDataStateType & state ) const;

/************************************************************************
 * Parameters
 ************************************************************************/
    /**
     * \brief   Returns value of SouthNorth of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter SouthNorth description: 
     *          The light state of south-north traffic.
     **/
    inline NESimpleTrafficLight::eTrafficLight getParamSouthNorth( void ) const;

    /**
     * \brief   Returns value of EastWest of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter EastWest description: 
     *          The light state of east-west traffic.
     **/
    inline NESimpleTrafficLight::eTrafficLight getParamEastWest( void ) const;

/************************************************************************
 * Requests
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// Operations.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Checks whether there is already listener of Notification Event
     *          exists. If does not exit, adds new listener entry in the listener
     *          list. If need, send appropriate message to Stub to start 
     *          sending attribute update messages. If already listener
     *          exists in listener list and notifyAlways flag is true, 
     *          sends immediate update notification based on existing update data status.
     * \param   msgId           The Notification Message ID
     * \param   consumer        The pointer of Notification Event consumer
     * \param   notifyAlways    The flag, indicating whether the notification event
     *                          should be send to client immediately if the notification
     *                          already exists. By default it is false. 
     **/
    inline void setNotification( NESimpleTrafficLight::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways = false );
    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     * \return  
     **/
    inline void clearNotification( NESimpleTrafficLight::eMessageIDs notifyId, IENotificationEventConsumer & listener );
    /**
     * \brief   Clears all notification for specified listener and unregisters it
     * \param   listener    The notification consumer object, which should be unregistered.
     **/
    inline void clearAllNotifications( IENotificationEventConsumer & listener );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEProxyEventConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Method derived from IEProxyEventConsumer interface.
     *          Triggered when on the request to execute function
     *          on server side, Proxy have got response message.
     * \param   eventElem   The Service Response event object.
     *                      Contains response message and information
     *                      sent by Stub
     **/
    virtual void processResponseEvent( ServiceResponseEvent & eventElem );
    /**
     * \brief   Method derived from IEProxyEventConsumer interface.
     *          Triggered when on server side a certain Attribute 
     *          value has been updated.
     * \param   eventElem   The Service Response event object.
     *                      Contains new updated value of Attribute
     *                      and validation flag.
     **/
    virtual void processAttributeEvent( ServiceResponseEvent & eventElem );

/************************************************************************/
// ProxyBase interface overrides.
/************************************************************************/

    /**
     * \brief   Creates notification event to send to client objects. 
     *          All Notification Events should be internal events and 
     *          should be instances of NotificationEvent class.
     * \param   data    The Notification Event data object containing
     *                  notification information.
     * \return  Returns new created notification event object.
     **/
    virtual NotificationEvent * createNotificationEvent( const NotificationEventData & data ) const;

    /**
     * \brief   Create Request event to send to Stub object. 
     *          Request events are triggering request calls on Stub side.
     * \param   args    The buffer containing serialized arguments of request call.
     * \param   reqId   The ID of request call.
     * \return  Return pointer of valid Request event.
     **/
    virtual ServiceRequestEvent * createRequestEvent( const EventDataStream & args, unsigned int reqId );

    /**
     * \brief   Creates event requesting to receive update notification events.
     *          The caller should be address of current proxy and the target 
     *          should address of appropriate sub address.
     * \param   msgId       The message ID to send. Should be either attribute or response (info)
     * \param   reqType     The type of request.
     * \return  Returns valid pointer of created service request event object.
     **/
    virtual ServiceRequestEvent * createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType );

    /**
     * \brief   Overwrite method to create response event from streaming object for 
     *          further dispatching by proxy.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Response event object.
     *          Otherwise, it returns NULL.
     **/
    virtual RemoteResponseEvent * createRemoteResponseEvent( const IEInStream & stream ) const;

    /**
     * \brief   Overwrite method to create error remote response event.
     *          Function is triggered when remote request sent by proxy failed and did not reach target.
     * \param   addrProxy   Address of Proxy object, which sent request event
     * \param   msgId       Message ID, which was requested to process
     * \param   reason      Failure reason set by system
     * \param   seqNr       The sequence number of processing message.
     **/
    virtual RemoteResponseEvent * createRemoteRequestFailedEvent( const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr ) const;

    /**
     * \brief   Overwrite this method to create service available event to new instantiated clients.
     * \param   consumer    The instance of consumer, which receives service available event.
     * \return  If succeeds, returns valid pointer to service available event object.
     **/
    virtual ProxyBase::ServiceAvailableEvent * createServiceAvailableEvent( IENotificationEventConsumer & consumer );

    /**
     * \brief   Called to register all servicing listeners. It is called when proxy is instantiated.
     *          Overwrite method to add service event listeners.
     **/
    virtual void registerServiceListeners( void );

    /**
     * \brief   Called to unregister all servicing listeners. It is called when proxy is freed.
     *          Overwrite method to remove service event listeners.
     **/
    virtual void unregisterServiceListeners( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attribute members
 ************************************************************************/
    /**
     * \brief   SouthNorth attribute value.
     *          The light state of south-north traffic.
     **/
    NESimpleTrafficLight::eTrafficLight mSouthNorth;

    /**
     * \brief   EastWest attribute value.
     *          The light state of east-west traffic.
     **/
    NESimpleTrafficLight::eTrafficLight mEastWest;

/************************************************************************
 * Parameter members
 ************************************************************************/
    /**
     * \brief   SouthNorth parameter value.
     *          The light state of south-north traffic.
     **/
    NESimpleTrafficLight::eTrafficLight mParamSouthNorth;

    /**
     * \brief   EastWest parameter value.
     *          The light state of east-west traffic.
     **/
    NESimpleTrafficLight::eTrafficLight mParamEastWest;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Hidden operations
 ************************************************************************/
    /**
     * \brief   Updates attribute and response call variable data received from Stub
     * \param   eventElem   The event object containing buffer with serialized data
     * \param   respId      The response message ID, which received. 
     *                      It is either attribute or response message ID
     **/
    void updateData( SimpleTrafficLightResponseEvent & eventElem, NESimpleTrafficLight::eMessageIDs respId );
    /**
     * \brief   Starts processing response message. The received event contains
     *          ID of appropriate message to update and contains result flag.
     *          If request calls failed (busy), the ID in event is a request call ID.
     *          In all other cases, the ID is either response call, info call, 
     *          or attribute update notification call. 
     *          It will also update parameters and states.
     * \param   evenElem    The response object containing message ID, result and data.
     **/
    void processResponse( SimpleTrafficLightResponseEvent & evenElem );

    /**
     * \brief   returns reference SimpleTrafficLightProxy object
     **/
    inline SimpleTrafficLightProxy & self( void );

/************************************************************************
 * Hidden static operations
 ************************************************************************/
private:
    /**
     * \brief   Create proxy object.
     * \param   roleName    The server component role name
     * \param   ownerThread The instance of thread to dispatch messages.
     *                      If NULL, current dispatching thread is used to dispatch messages.
     * \return  Pointer to instantiated proxy object.
     **/
    static ProxyBase * _createProxy( const char * roleName, DispatcherThread * ownerThread = static_cast<DispatcherThread *>(NULL) );
};

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightProxy class declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightProxy class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Inline operations
 ************************************************************************/

inline void SimpleTrafficLightProxy::setNotification( NESimpleTrafficLight::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways /* = false */ )
{
    ProxyBase::setNotification(static_cast<unsigned int>(notifyId), &listener, notifyAlways);
}

inline void SimpleTrafficLightProxy::clearNotification( NESimpleTrafficLight::eMessageIDs notifyId, IENotificationEventConsumer & listener )
{
    ProxyBase::clearNotification(static_cast<unsigned int>(notifyId), &listener);
}

inline void SimpleTrafficLightProxy::clearAllNotifications( IENotificationEventConsumer & listener )
{
    ProxyBase::unregisterListener( &listener );
}

inline SimpleTrafficLightProxy & SimpleTrafficLightProxy::self( void )
{
    return (*this);
}

/************************************************************************
 * Inline attributes
 ************************************************************************/

inline bool SimpleTrafficLightProxy::isSouthNorthValid( void ) const
{
    return (getProxyData().getAttributeState(NESimpleTrafficLight::MSG_ID_SouthNorth) == NEService::DATA_OK);
}

inline NESimpleTrafficLight::eTrafficLight SimpleTrafficLightProxy::getSouthNorth( NEService::eDataStateType & state ) const
{
    state = getProxyData().getAttributeState(NESimpleTrafficLight::MSG_ID_SouthNorth);
    return mSouthNorth;
}

inline bool SimpleTrafficLightProxy::isEastWestValid( void ) const
{
    return (getProxyData().getAttributeState(NESimpleTrafficLight::MSG_ID_EastWest) == NEService::DATA_OK);
}

inline NESimpleTrafficLight::eTrafficLight SimpleTrafficLightProxy::getEastWest( NEService::eDataStateType & state ) const
{
    state = getProxyData().getAttributeState(NESimpleTrafficLight::MSG_ID_EastWest);
    return mEastWest;
}

/************************************************************************
 * Inline parameters
 ************************************************************************/

inline NESimpleTrafficLight::eTrafficLight SimpleTrafficLightProxy::getParamSouthNorth( void ) const
{
    return mParamSouthNorth;
}

inline NESimpleTrafficLight::eTrafficLight SimpleTrafficLightProxy::getParamEastWest( void ) const
{
    return mParamEastWest;
}

#endif   // GENERATED_SRC_PRIVATE_SIMPLETRAFFICLIGHTPROXY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SimpleTrafficLightProxy.hpp file
//////////////////////////////////////////////////////////////////////////
