//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/SimpleTrafficLightClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_SIMPLETRAFFICLIGHTCLIENTBASE_HPP
#define  GENERATED_SRC_SIMPLETRAFFICLIGHTCLIENTBASE_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SimpleTrafficLight.
 * Generated at     11.08.2021  13:18:20 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/SimpleTrafficLightClientBase.hpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficLight Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NESimpleTrafficLight.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/SimpleTrafficLightProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of SimpleTrafficLight Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              The simple traffic light that triggers timeout and changes traffic light state.
 **/
class SimpleTrafficLightClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize SimpleTrafficLight Service Interface client object. 
     *          Specifies used service and owner thread.
     * \param   roleName    The role name assigned to SimpleTrafficLight servicing component object.
     * \param   ownerThread The name of component owner thread to dispatch messages.
     *                      If NULL, all messages are dispatched in current component thread.
     **/
    SimpleTrafficLightClientBase( const char* roleName, const char * ownerThread = static_cast<const char *>(NULL) );

    /**
     * \brief   Initialize SimpleTrafficLight Service Interface client object.
     *          Specifies used service and owner thread.
     * \param   roleName    The role name assigned to SimpleTrafficLight servicing component object.
     * \param   ownerThread The instance of component owner thread to dispatch messages.
     **/
    SimpleTrafficLightClientBase( const char* roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize SimpleTrafficLight Service Interface client object.
     *          Specifies used service and owner component.
     * \param   roleName    The role name assigned to SimpleTrafficLight servicing component object.
     * \param   owner       The instance of client owner component. The component object should be already initialized.
     * \note    When this constructor is used, it is important that the Component object is already initialized.
     *          and the component thread is set.
     **/
    SimpleTrafficLightClientBase( const char* roleName, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~SimpleTrafficLightClientBase( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Clears all notifications, stops receiving notifications from server
     **/
    inline void clearAllNotifications( void );

    /**
     * \brief   Returns true if the specified certain notification is already assigned.
     *          Otherwise returns false.
     * \param   msgId   The ID of message to check.
     **/
    inline bool isNotificationAssigned( NESimpleTrafficLight::eMessageIDs msgId ) const;

    /**
     * \brief   Returns true if client object has got connection with servicing component
     **/
    inline bool isConnected( void ) const;
    
    /**
     * \brief   Returns the name of used service.
     **/
    inline const String & getServiceName( void ) const;
    
    /**
     * \brief   Returns the version of used service.
     **/
    inline const Version & getServiceVersion( void ) const;

//////////////////////////////////////////////////////////////////////////
// Start Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLight Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attribute SouthNorth functions
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
     * \see     isSouthNorthValid, notifySouthNorthUpdate, onSouthNorthUpdate
     **/
    inline NESimpleTrafficLight::eTrafficLight getSouthNorth( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on SouthNorth attribute update.
     *          Once notification is enabled and the data is updated, 
     *          the getSouthNorth method will return valid data 
     *          Attribute SouthNorth description:
     *          The light state of south-north traffic.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isSouthNorthValid, getSouthNorth, onSouthNorthUpdate
     **/
    inline void notifyOnSouthNorthUpdate( bool notify = true );
    /**
     * \brief   Triggered, when SouthNorth attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes SouthNorth description: 
     *          The light state of south-north traffic.
     * \param   SouthNorth  The value of SouthNorth attribute.
     * \param   state       The data validation flag.
     **/
    virtual void onSouthNorthUpdate( NESimpleTrafficLight::eTrafficLight SouthNorth, NEService::eDataStateType state );

/************************************************************************
 * Attribute EastWest functions
 ************************************************************************/
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
     * \see     isEastWestValid, notifyEastWestUpdate, onEastWestUpdate
     **/
    inline NESimpleTrafficLight::eTrafficLight getEastWest( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on EastWest attribute update.
     *          Once notification is enabled and the data is updated, 
     *          the getEastWest method will return valid data 
     *          Attribute EastWest description:
     *          The light state of east-west traffic.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isEastWestValid, getEastWest, onEastWestUpdate
     **/
    inline void notifyOnEastWestUpdate( bool notify = true );
    /**
     * \brief   Triggered, when EastWest attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes EastWest description: 
     *          The light state of east-west traffic.
     * \param   EastWest    The value of EastWest attribute.
     * \param   state       The data validation flag.
     **/
    virtual void onEastWestUpdate( NESimpleTrafficLight::eTrafficLight EastWest, NEService::eDataStateType state );


//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLight Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Broadcast LightChanged
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Triggered when the light state of traffic is changed.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   SouthNorth  The light state of south-north traffic.
     * \param   EastWest    The light state of east-west traffic.
     **/
    virtual void broadcastLightChanged( NESimpleTrafficLight::eTrafficLight SouthNorth, NESimpleTrafficLight::eTrafficLight EastWest );
    /**
     * \brief   Call to enable or disable receiving notifications on LightChanged broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnBroadcastLightChanged( bool notify = true );

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

/************************************************************************/
// SimpleTrafficLightClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NESimpleTrafficLight::eMessageIDs InvalidReqId );
    
//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

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
     
    /**
     * \brief   Returns the current sequence number
     **/
    inline unsigned int getCurrentSequenceNr( void ) const;

    /**
     * \brief  Returns instance of proxy object.
     */
    inline const SimpleTrafficLightProxy * getProxy( void ) const;
      
    /**
     * \brief Returns target service component role name.
     **/
    inline const String & getServiceRole( void ) const;

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
    SimpleTrafficLightProxy *   mProxy;

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
    
/************************************************************************/
// SimpleTrafficLightClientBase hidden methods
/************************************************************************/

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
    void notifyOn( NESimpleTrafficLight::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NESimpleTrafficLight::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NESimpleTrafficLight::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of SimpleTrafficLightClientBase object
     **/

    inline SimpleTrafficLightClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SimpleTrafficLightClientBase( void );
    SimpleTrafficLightClientBase( const SimpleTrafficLightClientBase & /* src */ );
    const SimpleTrafficLightClientBase & operator = ( const SimpleTrafficLightClientBase & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline SimpleTrafficLightClientBase & SimpleTrafficLightClientBase::self( void )
{
    return (*this);
}

inline unsigned int SimpleTrafficLightClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void SimpleTrafficLightClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != NULL);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool SimpleTrafficLightClientBase::isConnected( void ) const
{
    ASSERT(mProxy != NULL);
    return mIsConnected;
}

inline bool SimpleTrafficLightClientBase::isNotificationAssigned( NESimpleTrafficLight::eMessageIDs msgId ) const
{
    ASSERT(mProxy != NULL);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & SimpleTrafficLightClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != NULL);
    return mProxy->getProxyAddress().getServiceName();
}
    
inline const Version & SimpleTrafficLightClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != NULL);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool SimpleTrafficLightClientBase::isSouthNorthValid( void ) const
{
    ASSERT(mProxy != NULL);
   return mProxy->isSouthNorthValid( );
}
inline NESimpleTrafficLight::eTrafficLight SimpleTrafficLightClientBase::getSouthNorth( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != NULL);
    return mProxy->getSouthNorth( state );
}

inline void SimpleTrafficLightClientBase::notifyOnSouthNorthUpdate( bool notify /* = true */ )
{
    SimpleTrafficLightClientBase::notifyOn( NESimpleTrafficLight::MSG_ID_SouthNorth, notify, false );
}

inline bool SimpleTrafficLightClientBase::isEastWestValid( void ) const
{
    ASSERT(mProxy != NULL);
   return mProxy->isEastWestValid( );
}
inline NESimpleTrafficLight::eTrafficLight SimpleTrafficLightClientBase::getEastWest( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != NULL);
    return mProxy->getEastWest( state );
}

inline void SimpleTrafficLightClientBase::notifyOnEastWestUpdate( bool notify /* = true */ )
{
    SimpleTrafficLightClientBase::notifyOn( NESimpleTrafficLight::MSG_ID_EastWest, notify, false );
}

/************************************************************************
 * Broadcast notifications
 ************************************************************************/

inline void SimpleTrafficLightClientBase::notifyOnBroadcastLightChanged( bool notify /* = true */ )
{
    SimpleTrafficLightClientBase::notifyOn(NESimpleTrafficLight::MSG_ID_broadcastLightChanged, notify, false);
}

inline const SimpleTrafficLightProxy * SimpleTrafficLightClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & SimpleTrafficLightClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != NULL);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_SIMPLETRAFFICLIGHTCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/SimpleTrafficLightClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
