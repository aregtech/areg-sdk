//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/LocalHelloWorldClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_LOCALHELLOWORLDCLIENTBASE_HPP
#define  GENERATED_SRC_LOCALHELLOWORLDCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     15.11.2023  14:51:44 GMT+01:00
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 *
 * \file            generated/src/LocalHelloWorldClientBase.hpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NELocalHelloWorld.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/LocalHelloWorldProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of LocalHelloWorld Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              A Local Service Interface to demonstrate working features of AREG SDK.
 **/
class LocalHelloWorldClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize LocalHelloWorld Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service LocalHelloWorld role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    LocalHelloWorldClientBase( const String & roleName, const String & ownerThread = String::EmptyString );

    /**
     * \brief   Initialize LocalHelloWorld Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service LocalHelloWorld role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    LocalHelloWorldClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize LocalHelloWorld Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service LocalHelloWorld role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    LocalHelloWorldClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize LocalHelloWorld Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service LocalHelloWorld dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    LocalHelloWorldClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~LocalHelloWorldClientBase( void );

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
    inline bool isNotificationAssigned( NELocalHelloWorld::eMessageIDs msgId ) const;

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
// LocalHelloWorld Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorld Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request HelloWorld
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to print hello world
     * \param   roleName    The role name of client component that requested to print hello world
     * \see     Has no response
     **/
    inline void requestHelloWorld( const String & roleName );
    /**
     * \brief   Overwrite to handle error of HelloWorld request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestHelloWorldFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Response HelloWorld
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          The response to hello world request.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientInfo  The client information set by servicing component. If empty or invalid ID, the message output failed.
     * \note    No request is associated.
     **/
    virtual void responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo );
    /**
     * \brief   Call to enable or disable receiving notifications on HelloWorld response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseHelloWorld( bool notify = true );

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
     * \brief   Triggered when receives service provider connected / disconnected event.
     *          When the service provider is connected, the client objects can set the listeners here.
     *          When the service provider is disconnected, the client object should clean the listeners.
     *          Up from connected status, the clients can subscribe and unsubscribe on updates,
     *          responses and broadcasts, can trigger requests. Before connection, the clients cannot
     *          neither trigger requests, nor receive data update messages.
     * \param   status  The service connection status.
     * \param   proxy   The Service Interface Proxy object, which is notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object.
     **/
    virtual bool serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy ) override;

/************************************************************************/
// LocalHelloWorldClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NELocalHelloWorld::eMessageIDs InvalidReqId );

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
    inline const SequenceNumber & getCurrentSequenceNr( void ) const;

    /**
     * \brief  Returns instance of proxy object.
     */
    inline const LocalHelloWorldProxy * getProxy( void ) const;

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
    SequenceNumber      mCurrSequenceNr;
    /**
     * \brief   Pointer of Proxy object providing communication
     **/
    LocalHelloWorldProxy *   mProxy;

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
// LocalHelloWorldClientBase hidden methods
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
    void notifyOn( NELocalHelloWorld::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NELocalHelloWorld::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NELocalHelloWorld::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of LocalHelloWorldClientBase object
     **/
    inline LocalHelloWorldClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalHelloWorldClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LocalHelloWorldClientBase );
};

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline LocalHelloWorldClientBase & LocalHelloWorldClientBase::self( void )
{
    return (*this);
}

inline const SequenceNumber & LocalHelloWorldClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void LocalHelloWorldClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool LocalHelloWorldClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool LocalHelloWorldClientBase::isNotificationAssigned( NELocalHelloWorld::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & LocalHelloWorldClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & LocalHelloWorldClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline void LocalHelloWorldClientBase::requestHelloWorld( const String & roleName )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestHelloWorld( roleName );
}

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void LocalHelloWorldClientBase::notifyOnResponseHelloWorld( bool notify /* = true */ )
{
    LocalHelloWorldClientBase::notifyOn(NELocalHelloWorld::eMessageIDs::MsgId_responseHelloWorld, notify, false);
}

inline const LocalHelloWorldProxy * LocalHelloWorldClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & LocalHelloWorldClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_LOCALHELLOWORLDCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/LocalHelloWorldClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
