//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/HelloUnlockClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_HELLOUNLOCKCLIENTBASE_HPP
#define  GENERATED_SRC_HELLOUNLOCKCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     04.01.2023  00:50:16 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloUnlock.
 *
 * \file            generated/src/HelloUnlockClientBase.hpp
 * \ingroup         HelloUnlock Service Interface
 * \brief           This is an automatic generated code of HelloUnlock
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEHelloUnlock.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/HelloUnlockProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// HelloUnlockClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of HelloUnlock Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 *              In this example the client sends a request each 200 ms and the service replies each 500,
 *              so that when the client sends next request, there is always a pending request on the service.
 *              To process the request and make correct reply, the service manually unblocks the request
 *              and when timeout is expired, sends the respons.
 *              The service is stopped and the application is shutdown when there is no more pending request.
 **/
class HelloUnlockClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize HelloUnlock Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service HelloUnlock role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    HelloUnlockClientBase( const String & roleName, const String & ownerThread = String::EmptyString );

    /**
     * \brief   Initialize HelloUnlock Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service HelloUnlock role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    HelloUnlockClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize HelloUnlock Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service HelloUnlock role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    HelloUnlockClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize HelloUnlock Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service HelloUnlock dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    HelloUnlockClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~HelloUnlockClientBase( void );

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
    inline bool isNotificationAssigned( NEHelloUnlock::eMessageIDs msgId ) const;

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
// HelloUnlock Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Attribute HelloServiceState functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of HelloServiceState attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isHelloServiceStateValid( void ) const;
    /**
     * \brief   Returns the value of HelloServiceState attribute.
     *          To get valid value, the Update Notification should be enabled.
     *          Attribute HelloServiceState description:
     *          The state of the service.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isHelloServiceStateValid, notifyHelloServiceStateUpdate, onHelloServiceStateUpdate
     **/
    inline NEHelloUnlock::eServiceState getHelloServiceState( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on HelloServiceState attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getHelloServiceState method will return valid data
     *          Attribute HelloServiceState description:
     *          The state of the service.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isHelloServiceStateValid, getHelloServiceState, onHelloServiceStateUpdate
     **/
    inline void notifyOnHelloServiceStateUpdate( bool notify = true );
    /**
     * \brief   Triggered, when HelloServiceState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes HelloServiceState description:
     *          The state of the service.
     * \param   HelloServiceState   The value of HelloServiceState attribute.
     * \param   state               The data validation flag.
     **/
    virtual void onHelloServiceStateUpdate( NEHelloUnlock::eServiceState HelloServiceState, NEService::eDataStateType state );

//////////////////////////////////////////////////////////////////////////
// HelloUnlock Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request HelloUblock
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to print hello world
     * \param   clientId    The given ID of the client. Should be 0 if unknown
     * \param   seqNr       The sequence number generated by the client. On each request the client increase the sequence number
     *          and stops sending request when reach the maximum.
     * \return  The sequence count number of call
     * \see     responseHelloUnblock
     **/
    inline unsigned int requestHelloUblock( unsigned int clientId, unsigned int seqNr );
    /**
     * \brief   Overwrite to handle error of HelloUblock request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestHelloUblockFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Response HelloUnblock
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          The response to hello world request.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientId    The ID of the client to send the response. Never is 0.
     * \param   seqNr       The sequence number created by the client. On reply the service sends the sequence number so that
     *          the cilent can check that all sequences exist.
     * \see     requestHelloUblock
     **/
    virtual void responseHelloUnblock( unsigned int clientId, unsigned int seqNr );
    /**
     * \brief   Call to enable or disable receiving notifications on HelloUnblock response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseHelloUnblock( bool notify = true );

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
// HelloUnlockClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NEHelloUnlock::eMessageIDs InvalidReqId );

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
    inline const HelloUnlockProxy * getProxy( void ) const;

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
    HelloUnlockProxy *   mProxy;

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
// HelloUnlockClientBase hidden methods
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
    void notifyOn( NEHelloUnlock::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NEHelloUnlock::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NEHelloUnlock::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of HelloUnlockClientBase object
     **/
    inline HelloUnlockClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    HelloUnlockClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloUnlockClientBase );
};

//////////////////////////////////////////////////////////////////////////
// HelloUnlockClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline HelloUnlockClientBase & HelloUnlockClientBase::self( void )
{
    return (*this);
}

inline unsigned int HelloUnlockClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void HelloUnlockClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool HelloUnlockClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool HelloUnlockClientBase::isNotificationAssigned( NEHelloUnlock::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & HelloUnlockClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & HelloUnlockClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool HelloUnlockClientBase::isHelloServiceStateValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isHelloServiceStateValid( );
}
inline NEHelloUnlock::eServiceState HelloUnlockClientBase::getHelloServiceState( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getHelloServiceState( state );
}

inline void HelloUnlockClientBase::notifyOnHelloServiceStateUpdate( bool notify /* = true */ )
{
    HelloUnlockClientBase::notifyOn( NEHelloUnlock::eMessageIDs::MsgId_HelloServiceState, notify, false );
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline unsigned int HelloUnlockClientBase::requestHelloUblock( unsigned int clientId, unsigned int seqNr )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestHelloUblock( static_cast<IENotificationEventConsumer &>(self()), clientId, seqNr );
}

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void HelloUnlockClientBase::notifyOnResponseHelloUnblock( bool notify /* = true */ )
{
    HelloUnlockClientBase::notifyOn(NEHelloUnlock::eMessageIDs::MsgId_responseHelloUnblock, notify, false);
}

inline const HelloUnlockProxy * HelloUnlockClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & HelloUnlockClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_HELLOUNLOCKCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/HelloUnlockClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
