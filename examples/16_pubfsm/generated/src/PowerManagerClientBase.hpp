//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/PowerManagerClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_POWERMANAGERCLIENTBASE_HPP
#define  GENERATED_SRC_POWERMANAGERCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:38 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PowerManager.
 *
 * \file            generated/src/PowerManagerClientBase.hpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEPowerManager.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/PowerManagerProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// PowerManagerClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of PowerManager Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              This is a power manager of Traffic Light Controller. It powers the lights ON and OFF.
 **/
class PowerManagerClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize PowerManager Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service PowerManager role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    PowerManagerClientBase( const String & roleName, const String & ownerThread = String::getEmptyString() );

    /**
     * \brief   Initialize PowerManager Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service PowerManager role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    PowerManagerClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize PowerManager Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service PowerManager role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    PowerManagerClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize PowerManager Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service PowerManager dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    PowerManagerClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~PowerManagerClientBase( void );

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
    inline bool isNotificationAssigned( NEPowerManager::eMessageIDs msgId ) const;

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
// PowerManager Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Attribute LightsPowerState functions
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
     * \see     isLightsPowerStateValid, notifyLightsPowerStateUpdate, onLightsPowerStateUpdate
     **/
    inline NEPowerManager::ePoweredState getLightsPowerState( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on LightsPowerState attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getLightsPowerState method will return valid data
     *          Attribute LightsPowerState description:
     *          The power state of the traffic lights.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isLightsPowerStateValid, getLightsPowerState, onLightsPowerStateUpdate
     **/
    inline void notifyOnLightsPowerStateUpdate( bool notify = true );
    /**
     * \brief   Triggered, when LightsPowerState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes LightsPowerState description:
     *          The power state of the traffic lights.
     * \param   LightsPowerState    The value of LightsPowerState attribute.
     * \param   state               The data validation flag.
     **/
    virtual void onLightsPowerStateUpdate( NEPowerManager::ePoweredState LightsPowerState, NEService::eDataStateType state );

//////////////////////////////////////////////////////////////////////////
// PowerManager Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request PowerOn
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Called to power ON the traffic lights.
     * \see     Has no response
     **/
    inline void requestPowerOn( void );
    /**
     * \brief   Overwrite to handle error of PowerOn request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestPowerOnFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request PowerOff
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Called to power OFF the traffic lights.
     * \see     Has no response
     **/
    inline void requestPowerOff( void );
    /**
     * \brief   Overwrite to handle error of PowerOff request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestPowerOffFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request StartTrafficLight
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Triggered to start the traffic light
     * \return  The sequence count number of call
     * \see     responseStartTrafficLight
     **/
    inline unsigned int requestStartTrafficLight( void );
    /**
     * \brief   Overwrite to handle error of StartTrafficLight request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestStartTrafficLightFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request StopTrafficLight
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Call to stop the traffic lights and put them in initialization state.
     * \return  The sequence count number of call
     * \see     responseStopTrafficLight
     **/
    inline unsigned int requestStopTrafficLight( void );
    /**
     * \brief   Overwrite to handle error of StopTrafficLight request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestStopTrafficLightFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Response StartTrafficLight
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Sent as a response to start the traffic light. The traffic light can be started when it is powered ON and in initialization state.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   Success Flag, indicating whether the operation succeeded or not.
     *          This flag is 'true' if lights are initialization state of if traffic light is already functioning.
     *          This flag is 'false' if lights are OFF.
     * \see     requestStartTrafficLight
     **/
    virtual void responseStartTrafficLight( bool Success );
    /**
     * \brief   Call to enable or disable receiving notifications on StartTrafficLight response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseStartTrafficLight( bool notify = true );

/************************************************************************
 * Response StopTrafficLight
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Response to stop the traffic lights.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   Success Flag, indicating whether the request was processed with success or not.
     *          This flag is 'true' if traffic light are functioning or lights are in initialization state.
     *          This flag is 'false' if traffic lights are OFF.
     * \see     requestStopTrafficLight
     **/
    virtual void responseStopTrafficLight( bool Success );
    /**
     * \brief   Call to enable or disable receiving notifications on StopTrafficLight response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseStopTrafficLight( bool notify = true );

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
// PowerManagerClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NEPowerManager::eMessageIDs InvalidReqId );

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
    inline const PowerManagerProxy * getProxy( void ) const;

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
    PowerManagerProxy *   mProxy;

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
// PowerManagerClientBase hidden methods
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
    void notifyOn( NEPowerManager::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NEPowerManager::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NEPowerManager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of PowerManagerClientBase object
     **/
    inline PowerManagerClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PowerManagerClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PowerManagerClientBase );
};

//////////////////////////////////////////////////////////////////////////
// PowerManagerClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline PowerManagerClientBase & PowerManagerClientBase::self( void )
{
    return (*this);
}

inline unsigned int PowerManagerClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void PowerManagerClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool PowerManagerClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool PowerManagerClientBase::isNotificationAssigned( NEPowerManager::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & PowerManagerClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & PowerManagerClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool PowerManagerClientBase::isLightsPowerStateValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isLightsPowerStateValid( );
}
inline NEPowerManager::ePoweredState PowerManagerClientBase::getLightsPowerState( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getLightsPowerState( state );
}

inline void PowerManagerClientBase::notifyOnLightsPowerStateUpdate( bool notify /* = true */ )
{
    PowerManagerClientBase::notifyOn( NEPowerManager::eMessageIDs::MsgId_LightsPowerState, notify, false );
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline void PowerManagerClientBase::requestPowerOn( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestPowerOn(  );
}

inline void PowerManagerClientBase::requestPowerOff( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestPowerOff(  );
}

inline unsigned int PowerManagerClientBase::requestStartTrafficLight( void )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestStartTrafficLight( static_cast<IENotificationEventConsumer &>(self()) );
}

inline unsigned int PowerManagerClientBase::requestStopTrafficLight( void )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestStopTrafficLight( static_cast<IENotificationEventConsumer &>(self()) );
}

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void PowerManagerClientBase::notifyOnResponseStartTrafficLight( bool notify /* = true */ )
{
    PowerManagerClientBase::notifyOn(NEPowerManager::eMessageIDs::MsgId_responseStartTrafficLight, notify, false);
}

inline void PowerManagerClientBase::notifyOnResponseStopTrafficLight( bool notify /* = true */ )
{
    PowerManagerClientBase::notifyOn(NEPowerManager::eMessageIDs::MsgId_responseStopTrafficLight, notify, false);
}

inline const PowerManagerProxy * PowerManagerClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & PowerManagerClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_POWERMANAGERCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/PowerManagerClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
