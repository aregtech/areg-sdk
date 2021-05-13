//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/TrafficLightClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_TRAFFICLIGHTCLIENTBASE_HPP
#define  SRC_GEN_TRAFFICLIGHTCLIENTBASE_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficLight.
 * Generated at     12.05.2021  16:41:14 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/TrafficLightClientBase.hpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "src/gen/NETrafficLight.hpp"
#include "areg/component/ClientBase.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "src/gen/private/TrafficLightProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// TrafficLightClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of TrafficLight Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              The traffic light to change lights.
 **/
class TrafficLightClientBase  : public    IEProxyListener, private ClientBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize TrafficLight Service Interface client object. 
     *          Specifies used service and owner thread.
     * \param   roleName    The role name assigned to TrafficLight servicing component object.
     * \param   ownerThread The name of component owner thread to dispatch messages.
     *                      If NULL, all messages are dispatched in current component thread.
     **/
    TrafficLightClientBase( const char* roleName, const char * ownerThread = static_cast<const char *>(NULL) );

    /**
     * \brief   Initialize TrafficLight Service Interface client object.
     *          Specifies used service and owner thread.
     * \param   roleName    The role name assigned to TrafficLight servicing component object.
     * \param   ownerThread The instance of component owner thread to dispatch messages.
     **/
    TrafficLightClientBase( const char* roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize TrafficLight Service Interface client object.
     *          Specifies used service and owner component.
     * \param   roleName    The role name assigned to TrafficLight servicing component object.
     * \param   owner       The instance of client owner component. The component object should be already initialized.
     * \note    When this constructor is used, it is important that the Component object is already initialized.
     *          and the component thread is set.
     **/
    TrafficLightClientBase( const char* roleName, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~TrafficLightClientBase( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Clears all notifications, stops receiving notifications from server
     **/
    void clearAllNotifications( void );

    /**
     * \brief   Returns true if the specified certain notification is already assigned.
     *          Otherwise returns false.
     * \param   msgId   The ID of message to check.
     **/
    bool isNotificationAssigned( NETrafficLight::eMessageIDs msgId ) const;

    /**
     * \brief   Returns true if client object has got connection with servicing component
     **/
    bool isConnected( void ) const;

//////////////////////////////////////////////////////////////////////////
// Start Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TrafficLight Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attribute TrafficLight functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of TrafficLight attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    bool isTrafficLightValid( void ) const;
    /**
     * \brief   Returns the value of TrafficLight attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute TrafficLight description: 
     *          The current color of traffic light
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     * \see     isTrafficLightValid, notifyTrafficLightUpdate, onTrafficLightUpdate
     **/
    NETrafficLight::eLight getTrafficLight( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on TrafficLight attribute update.
     *          Once notification is enabled and the data is updated, 
     *          the getTrafficLight method will return valid data 
     *          Attribute TrafficLight description:
     *          The current color of traffic light
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isTrafficLightValid, getTrafficLight, onTrafficLightUpdate
     **/
    void notifyOnTrafficLightUpdate( bool notify = true );
    /**
     * \brief   Triggered, when TrafficLight attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes TrafficLight description: 
     *          The current color of traffic light
     * \param   TrafficLight    The value of TrafficLight attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onTrafficLightUpdate( NETrafficLight::eLight TrafficLight, NEService::eDataStateType state );


//////////////////////////////////////////////////////////////////////////
// TrafficLight Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request ChangeLight
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Requested rapidly to change the light.
     * \param   lightColor  DESCRIPTION MISSED
     * \param   holdon      If true, it will holdon the light until next request to change color. 
     *          Otherwise, the timer will trigger next color.
     * \return  The sequence count number of call
     * \see     responseChangeLight
     **/
    unsigned int requestChangeLight( NETrafficLight::eLight lightColor, bool holdon );
    /**
     * \brief   Overwrite to handle error of ChangeLight request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestChangeLightFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Response ChangeLight
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Resonse to change the light color.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   lightColor  The light of trafic light.
     * \see     requestChangeLight
     **/
    virtual void responseChangeLight( NETrafficLight::eLight lightColor );
    /**
     * \brief   Call to enable or disable receiving notifications on ChangeLight response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseChangeLight( bool notify = true );

//////////////////////////////////////////////////////////////////////////
// End Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEProxyListener Overrides
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
     **/
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy );

protected:
/************************************************************************/
// TrafficLightClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
    virtual void invalidResponse( NETrafficLight::eMessageIDs InvalidRespId );

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NETrafficLight::eMessageIDs InvalidReqId );
    
    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestFailed( NETrafficLight::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Returns the current sequence number
     **/
    unsigned int getCurrentSequenceNr( void ) const;

    /**
     * \brief   Call to recreate Proxy for the client. This call will remove and unregister all existing notifications
     *          and the client will not receive pending update or response notifications. 
     *          The client first will receive disconnect message, then again connect.
     *          Reset update notifications manually in connect if need.          
     *          This function call can be used when the client instance should be registered and run in new thread.
     *
     * \return  Returns true if Proxy was created with success.
     **/
     bool recreateProxy( void );

     /**
      * \brief  Returns pointer to client dispatcher thread where the messages are dispatched.
      *         The function can return NULL if Proxy was not instantiated yet.
      **/
     DispatcherThread * getDispatcherThread( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Connection flag. If true, the client object is connected to server
     **/
    bool                mIsConnected;
    /**
     * \brief   The counter of sequence number
     **/
    unsigned int        mCurrSequenceNr;
    /**
     * \brief   Pointer of Proxy object providing communication
     **/
    TrafficLightProxy *   mProxy;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// IENotificationEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Is processing notification event calls.
     * \param   eventElem   Notification Event object to process
     **/
    virtual void processNotificationEvent( NotificationEvent & eventElem );
    
private:
    /**
     * \brief   Enables / Disables notification flags on appropriate message call.
     * \param   msgId   The ID of message to enable / disable notification
     * \param   notify  If true, the notification is enabled. 
     *                  If false, the notification is disabled.
     * \param   always  Flag, indicating whether should notify always or not.
     *                  if 'notify' parameter is true and the notification is already
     *                  assigned, then if parameter 'always' is true, it will trigger
     *                  notification immediately after call. 
     **/
    void notifyOn( NETrafficLight::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Returns reference of TrafficLightClientBase object
     **/
    TrafficLightClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TrafficLightClientBase( void );
    TrafficLightClientBase( const TrafficLightClientBase & /* src */ );
    const TrafficLightClientBase & operator = ( const TrafficLightClientBase & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// TrafficLightClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline TrafficLightClientBase & TrafficLightClientBase::self( void )
{
    return (*this);
}

inline unsigned int TrafficLightClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline bool TrafficLightClientBase::isConnected( void ) const
{
    return mIsConnected;
}

inline bool TrafficLightClientBase::isNotificationAssigned( NETrafficLight::eMessageIDs msgId ) const
{
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool TrafficLightClientBase::isTrafficLightValid( void ) const
{
   return mProxy->isTrafficLightValid( );
}
inline NETrafficLight::eLight TrafficLightClientBase::getTrafficLight( NEService::eDataStateType & state ) const
{
    return mProxy->getTrafficLight( state );
}

inline void TrafficLightClientBase::notifyOnTrafficLightUpdate( bool notify /* = true */ )
{
    notifyOn( NETrafficLight::MSG_ID_TrafficLight, notify, false );
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline unsigned int TrafficLightClientBase::requestChangeLight( NETrafficLight::eLight lightColor, bool holdon )
{
    return mProxy->requestChangeLight( static_cast<IENotificationEventConsumer &>(self()), lightColor, holdon );
}

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void TrafficLightClientBase::notifyOnResponseChangeLight( bool notify /* = true */ )
{
    notifyOn(NETrafficLight::MSG_ID_responseChangeLight, notify, false);
}

#endif   // SRC_GEN_TRAFFICLIGHTCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/TrafficLightClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
