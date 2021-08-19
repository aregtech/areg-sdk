//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PowerManagerProxy.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PRIVATE_POWERMANAGERPROXY_HPP
#define  GENERATED_SRC_PRIVATE_POWERMANAGERPROXY_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source PowerManager.
 * Generated at     15.08.2021  00:03:03 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/PowerManagerProxy.hpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager Service Interface Proxy class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEPowerManager.hpp"
#include "areg/component/ProxyBase.hpp"
 
/************************************************************************
 * Dependencies
 ************************************************************************/
class PowerManagerResponseEvent;
 
//////////////////////////////////////////////////////////////////////////
// PowerManagerProxy class declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       PowerManager Service Interface proxy class.
 *              Automatically instantiated per thread at client startup.
 *              Used to communicate with stub object and sending notifications
 *              to each client running in the thread instance.
 *
 *              This is a power manager of Traffic Light Controller. It powers the lights ON and OFF.
 **/
class PowerManagerProxy   : public ProxyBase
{

private:
    //////////////////////////////////////////////////////////////////////////
    // PowerManagerProxy::PowerManagerServiceAvailableEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief       The event is used to immediately notify new client
     *              about service availability and escape duplicating 
     *              notification for other, already connected clients.
     *  
     **/
    class PowerManagerServiceAvailableEvent : public ProxyBase::ServiceAvailableEvent
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT(PowerManagerProxy::PowerManagerServiceAvailableEvent)

    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        PowerManagerServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor
         **/
        virtual ~PowerManagerServiceAvailableEvent( void );
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        PowerManagerServiceAvailableEvent( void );
        PowerManagerServiceAvailableEvent( const PowerManagerServiceAvailableEvent & /*src*/ );
        const PowerManagerServiceAvailableEvent & operator = ( const PowerManagerServiceAvailableEvent & /*src*/ );
    };
//////////////////////////////////////////////////////////////////////////
// PowerManagerProxy predefined constants and static members.
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
    static PowerManagerProxy * createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread );

    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The name of thread instance to dispatch messages.
     *                          If NULL, current dispatching thread is used to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static PowerManagerProxy * createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread = static_cast<const char *>(NULL) );

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
    PowerManagerProxy( const char * roleName, DispatcherThread * ownerThread = static_cast<DispatcherThread *>(NULL) );
    /**
     * \brief   Destructor
     **/
    virtual ~PowerManagerProxy( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attributes
 ************************************************************************/
    /**
     * \brief   Returns true if value of LightsPowerState attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isLightsPowerStateValid( void ) const;
    /**
     * \brief   Returns the value of LightsPowerState attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute LightsPowerState description: 
     *          The power state of the traffic lights.
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     **/
    inline NEPowerManager::ePoweredState getLightsPowerState( NEService::eDataStateType & state ) const;

/************************************************************************
 * Parameters
 ************************************************************************/
    /**
     * \brief   Returns value of Success of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter Success description: 
     *          Flag, indicating whether the operation succeeded or not.
     *          This flag is 'true' if lights are initialization state of if traffic light is already functioning.
     *          This flag is 'false' if lights are OFF.
     **/
    inline bool getParamSuccess( void ) const;

/************************************************************************
 * Requests
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Called to power ON the traffic lights.
     * \see     Has no Response.
     **/
    void requestPowerOn( void );

    /**
     * \brief   Request call.
     *          Called to power OFF the traffic lights.
     * \see     Has no Response.
     **/
    void requestPowerOff( void );

    /**
     * \brief   Request call.
     *          Triggered to start the traffic light
     * \param   caller  The reference of caller object to get response.
     * \return  The sequence count number of call
     * \see     responseStartTrafficLight
     **/
    unsigned int requestStartTrafficLight( IENotificationEventConsumer & caller );
    
    /**
     * \brief   Request call.
     *          Call to stop the traffic lights and put them in initialization state.
     * \param   caller  The reference of caller object to get response.
     * \return  The sequence count number of call
     * \see     responseStopTrafficLight
     **/
    unsigned int requestStopTrafficLight( IENotificationEventConsumer & caller );
    
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
    inline void setNotification( NEPowerManager::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways = false );
    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     * \return  
     **/
    inline void clearNotification( NEPowerManager::eMessageIDs notifyId, IENotificationEventConsumer & listener );
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
     * \brief   LightsPowerState attribute value.
     *          The power state of the traffic lights.
     **/
    NEPowerManager::ePoweredState   mLightsPowerState;

/************************************************************************
 * Parameter members
 ************************************************************************/
    /**
     * \brief   Success parameter value.
     *          Flag, indicating whether the operation succeeded or not.
     *          This flag is 'true' if lights are initialization state of if traffic light is already functioning.
     *          This flag is 'false' if lights are OFF.
     **/
    bool                            mParamSuccess;

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
    void updateData( PowerManagerResponseEvent & eventElem, NEPowerManager::eMessageIDs respId );
    /**
     * \brief   Starts processing response message. The received event contains
     *          ID of appropriate message to update and contains result flag.
     *          If request calls failed (busy), the ID in event is a request call ID.
     *          In all other cases, the ID is either response call, info call, 
     *          or attribute update notification call. 
     *          It will also update parameters and states.
     * \param   evenElem    The response object containing message ID, result and data.
     **/
    void processResponse( PowerManagerResponseEvent & evenElem );

    /**
     * \brief   returns reference PowerManagerProxy object
     **/
    inline PowerManagerProxy & self( void );

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
// PowerManagerProxy class declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PowerManagerProxy class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Inline operations
 ************************************************************************/

inline void PowerManagerProxy::setNotification( NEPowerManager::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways /* = false */ )
{
    ProxyBase::setNotification(static_cast<unsigned int>(notifyId), &listener, notifyAlways);
}

inline void PowerManagerProxy::clearNotification( NEPowerManager::eMessageIDs notifyId, IENotificationEventConsumer & listener )
{
    ProxyBase::clearNotification(static_cast<unsigned int>(notifyId), &listener);
}

inline void PowerManagerProxy::clearAllNotifications( IENotificationEventConsumer & listener )
{
    ProxyBase::unregisterListener( &listener );
}

inline PowerManagerProxy & PowerManagerProxy::self( void )
{
    return (*this);
}

/************************************************************************
 * Inline attributes
 ************************************************************************/

inline bool PowerManagerProxy::isLightsPowerStateValid( void ) const
{
    return (getProxyData().getAttributeState(NEPowerManager::MSG_ID_LightsPowerState) == NEService::DATA_OK);
}

inline NEPowerManager::ePoweredState PowerManagerProxy::getLightsPowerState( NEService::eDataStateType & state ) const
{
    state = getProxyData().getAttributeState(NEPowerManager::MSG_ID_LightsPowerState);
    return mLightsPowerState;
}

/************************************************************************
 * Inline parameters
 ************************************************************************/

inline bool PowerManagerProxy::getParamSuccess( void ) const
{
    return mParamSuccess;
}

#endif   // GENERATED_SRC_PRIVATE_POWERMANAGERPROXY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PowerManagerProxy.hpp file
//////////////////////////////////////////////////////////////////////////
