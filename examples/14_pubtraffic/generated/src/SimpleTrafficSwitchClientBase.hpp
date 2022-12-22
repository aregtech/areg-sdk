//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/SimpleTrafficSwitchClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_SIMPLETRAFFICSWITCHCLIENTBASE_HPP
#define  GENERATED_SRC_SIMPLETRAFFICSWITCHCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.12.2022  16:19:34 GMT+01:00
 *                  Create by AREG SDK code generator tool from source SimpleTrafficSwitch.
 *
 * \file            generated/src/SimpleTrafficSwitchClientBase.hpp
 * \ingroup         SimpleTrafficSwitch Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficSwitch
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NESimpleTrafficSwitch.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/SimpleTrafficSwitchProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of SimpleTrafficSwitch Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              Simple traffic light switch on/off.
 *              Declared as local service to prevent any other external client to send unexpected signal to switch off the light.
 **/
class SimpleTrafficSwitchClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize SimpleTrafficSwitch Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service SimpleTrafficSwitch role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    SimpleTrafficSwitchClientBase( const String & roleName, const String & ownerThread = String::EmptyString );

    /**
     * \brief   Initialize SimpleTrafficSwitch Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service SimpleTrafficSwitch role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    SimpleTrafficSwitchClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize SimpleTrafficSwitch Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service SimpleTrafficSwitch role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    SimpleTrafficSwitchClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize SimpleTrafficSwitch Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service SimpleTrafficSwitch dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    SimpleTrafficSwitchClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~SimpleTrafficSwitchClientBase( void );

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
    inline bool isNotificationAssigned( NESimpleTrafficSwitch::eMessageIDs msgId ) const;

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
// SimpleTrafficSwitch Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Attribute IsSwitchedOn functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of IsSwitchedOn attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isIsSwitchedOnValid( void ) const;
    /**
     * \brief   Returns the value of IsSwitchedOn attribute.
     *          To get valid value, the Update Notification should be enabled.
     *          Attribute IsSwitchedOn description:
     *          Flag that indicates the light on/off state.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isIsSwitchedOnValid, notifyIsSwitchedOnUpdate, onIsSwitchedOnUpdate
     **/
    inline bool getIsSwitchedOn( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on IsSwitchedOn attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getIsSwitchedOn method will return valid data
     *          Attribute IsSwitchedOn description:
     *          Flag that indicates the light on/off state.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isIsSwitchedOnValid, getIsSwitchedOn, onIsSwitchedOnUpdate
     **/
    inline void notifyOnIsSwitchedOnUpdate( bool notify = true );
    /**
     * \brief   Triggered, when IsSwitchedOn attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes IsSwitchedOn description:
     *          Flag that indicates the light on/off state.
     * \param   IsSwitchedOn    The value of IsSwitchedOn attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onIsSwitchedOnUpdate( bool IsSwitchedOn, NEService::eDataStateType state );

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitch Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request SwitchLight
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Sets the traffic light ON or OFF.
     * \param   switchOn    If true, the traffic light is switched ON. Otherwise, it is OFF.
     * \see     Has no response
     **/
    inline void requestSwitchLight( bool switchOn );
    /**
     * \brief   Overwrite to handle error of SwitchLight request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestSwitchLightFailed( NEService::eResultType FailureReason );

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
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy ) override;

/************************************************************************/
// SimpleTrafficSwitchClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NESimpleTrafficSwitch::eMessageIDs InvalidReqId );

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
     *         The function can return nullptr if Proxy was not instantiated yet.
     **/
    DispatcherThread * getDispatcherThread( void );

    /**
     * \brief   Returns the current sequence number
     **/
    inline unsigned int getCurrentSequenceNr( void ) const;

    /**
     * \brief  Returns instance of proxy object.
     */
    inline const SimpleTrafficSwitchProxy * getProxy( void ) const;

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
    SimpleTrafficSwitchProxy *   mProxy;

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
    virtual void processNotificationEvent( NotificationEvent & eventElem ) override;

/************************************************************************/
// SimpleTrafficSwitchClientBase hidden methods
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
    void notifyOn( NESimpleTrafficSwitch::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NESimpleTrafficSwitch::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NESimpleTrafficSwitch::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of SimpleTrafficSwitchClientBase object
     **/
    inline SimpleTrafficSwitchClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SimpleTrafficSwitchClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SimpleTrafficSwitchClientBase );
};

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline SimpleTrafficSwitchClientBase & SimpleTrafficSwitchClientBase::self( void )
{
    return (*this);
}

inline unsigned int SimpleTrafficSwitchClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void SimpleTrafficSwitchClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool SimpleTrafficSwitchClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool SimpleTrafficSwitchClientBase::isNotificationAssigned( NESimpleTrafficSwitch::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & SimpleTrafficSwitchClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & SimpleTrafficSwitchClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool SimpleTrafficSwitchClientBase::isIsSwitchedOnValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isIsSwitchedOnValid( );
}
inline bool SimpleTrafficSwitchClientBase::getIsSwitchedOn( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getIsSwitchedOn( state );
}

inline void SimpleTrafficSwitchClientBase::notifyOnIsSwitchedOnUpdate( bool notify /* = true */ )
{
    SimpleTrafficSwitchClientBase::notifyOn( NESimpleTrafficSwitch::eMessageIDs::MsgId_IsSwitchedOn, notify, false );
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline void SimpleTrafficSwitchClientBase::requestSwitchLight( bool switchOn )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestSwitchLight( switchOn );
}

inline const SimpleTrafficSwitchProxy * SimpleTrafficSwitchClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & SimpleTrafficSwitchClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_SIMPLETRAFFICSWITCHCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/SimpleTrafficSwitchClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
