//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/HelloWatchdogClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_HELLOWATCHDOGCLIENTBASE_HPP
#define  GENERATED_SRC_HELLOWATCHDOGCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:31 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWatchdog.
 *
 * \file            generated/src/HelloWatchdogClientBase.hpp
 * \ingroup         HelloWatchdog Service Interface
 * \brief           This is an automatic generated code of HelloWatchdog
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEHelloWatchdog.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/HelloWatchdogProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of HelloWatchdog Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 *              This service will demonstrate how the system watchdog works in case of public service.
 **/
class HelloWatchdogClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize HelloWatchdog Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service HelloWatchdog role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    HelloWatchdogClientBase( const String & roleName, const String & ownerThread = String::getEmptyString() );

    /**
     * \brief   Initialize HelloWatchdog Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service HelloWatchdog role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    HelloWatchdogClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize HelloWatchdog Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service HelloWatchdog role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    HelloWatchdogClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize HelloWatchdog Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service HelloWatchdog dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    HelloWatchdogClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~HelloWatchdogClientBase( void );

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
    inline bool isNotificationAssigned( NEHelloWatchdog::eMessageIDs msgId ) const;

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
// HelloWatchdog Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Attribute ServiceState functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of ServiceState attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isServiceStateValid( void ) const;
    /**
     * \brief   Returns the value of ServiceState attribute.
     *          To get valid value, the Update Notification should be enabled.
     *          Attribute ServiceState description:
     *          The state of the service.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isServiceStateValid, notifyServiceStateUpdate, onServiceStateUpdate
     **/
    inline NEHelloWatchdog::eState getServiceState( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on ServiceState attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getServiceState method will return valid data
     *          Attribute ServiceState description:
     *          The state of the service.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isServiceStateValid, getServiceState, onServiceStateUpdate
     **/
    inline void notifyOnServiceStateUpdate( bool notify = true );
    /**
     * \brief   Triggered, when ServiceState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes ServiceState description:
     *          The state of the service.
     * \param   ServiceState    The value of ServiceState attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onServiceStateUpdate( NEHelloWatchdog::eState ServiceState, NEService::eDataStateType state );

//////////////////////////////////////////////////////////////////////////
// HelloWatchdog Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request StartSleep
 ************************************************************************/
    /**
     * \brief   Request call.
     *          The response triggered when the thread resumed from suspended mode.
     * \param   timeoutSleep    The timeout in milliseconds to suspend the thread.
     * \return  The sequence count number of call
     * \see     responseStartSleep
     **/
    inline unsigned int requestStartSleep( unsigned int timeoutSleep );
    /**
     * \brief   Overwrite to handle error of StartSleep request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestStartSleepFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request StopService
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Called to stop the service.
     * \see     Has no response
     **/
    inline void requestStopService( void );
    /**
     * \brief   Overwrite to handle error of StopService request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestStopServiceFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request ShutdownService
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Called to shutdown service and exit application.
     * \see     Has no response
     **/
    inline void requestShutdownService( void );
    /**
     * \brief   Overwrite to handle error of ShutdownService request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestShutdownServiceFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Response StartSleep
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Triggered to stop the timer.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   timeoutSleep    The timeout in milliseconds while thread was in suspended mode.
     * \see     requestStartSleep
     **/
    virtual void responseStartSleep( unsigned int timeoutSleep );
    /**
     * \brief   Call to enable or disable receiving notifications on StartSleep response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseStartSleep( bool notify = true );

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
// HelloWatchdogClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NEHelloWatchdog::eMessageIDs InvalidReqId );

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
    inline const HelloWatchdogProxy * getProxy( void ) const;

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
    HelloWatchdogProxy *   mProxy;

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
// HelloWatchdogClientBase hidden methods
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
    void notifyOn( NEHelloWatchdog::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NEHelloWatchdog::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NEHelloWatchdog::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of HelloWatchdogClientBase object
     **/
    inline HelloWatchdogClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    HelloWatchdogClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloWatchdogClientBase );
};

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline HelloWatchdogClientBase & HelloWatchdogClientBase::self( void )
{
    return (*this);
}

inline unsigned int HelloWatchdogClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void HelloWatchdogClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool HelloWatchdogClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool HelloWatchdogClientBase::isNotificationAssigned( NEHelloWatchdog::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & HelloWatchdogClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & HelloWatchdogClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool HelloWatchdogClientBase::isServiceStateValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isServiceStateValid( );
}
inline NEHelloWatchdog::eState HelloWatchdogClientBase::getServiceState( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getServiceState( state );
}

inline void HelloWatchdogClientBase::notifyOnServiceStateUpdate( bool notify /* = true */ )
{
    HelloWatchdogClientBase::notifyOn( NEHelloWatchdog::eMessageIDs::MsgId_ServiceState, notify, false );
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline unsigned int HelloWatchdogClientBase::requestStartSleep( unsigned int timeoutSleep )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestStartSleep( static_cast<IENotificationEventConsumer &>(self()), timeoutSleep );
}

inline void HelloWatchdogClientBase::requestStopService( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestStopService(  );
}

inline void HelloWatchdogClientBase::requestShutdownService( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestShutdownService(  );
}

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void HelloWatchdogClientBase::notifyOnResponseStartSleep( bool notify /* = true */ )
{
    HelloWatchdogClientBase::notifyOn(NEHelloWatchdog::eMessageIDs::MsgId_responseStartSleep, notify, false);
}

inline const HelloWatchdogProxy * HelloWatchdogClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & HelloWatchdogClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_HELLOWATCHDOGCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/HelloWatchdogClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
