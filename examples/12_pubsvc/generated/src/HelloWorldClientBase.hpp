//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/HelloWorldClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_HELLOWORLDCLIENTBASE_HPP
#define  GENERATED_SRC_HELLOWORLDCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:08:39 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 *
 * \file            generated/src/HelloWorldClientBase.hpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEHelloWorld.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/HelloWorldProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// HelloWorldClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of HelloWorld Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 **/
class HelloWorldClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize HelloWorld Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service HelloWorld role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    HelloWorldClientBase( const String & roleName, const String & ownerThread = String::getEmptyString() );

    /**
     * \brief   Initialize HelloWorld Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service HelloWorld role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    HelloWorldClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize HelloWorld Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service HelloWorld role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    HelloWorldClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize HelloWorld Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service HelloWorld dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    HelloWorldClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~HelloWorldClientBase( void );

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
    inline bool isNotificationAssigned( NEHelloWorld::eMessageIDs msgId ) const;

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
// HelloWorld Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// HelloWorld Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request HelloWorld
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to print hello world
     * \param   roleName    The role name of client component that requested to print hello world
     * \return  The sequence count number of call
     * \see     responseHelloWorld
     **/
    inline unsigned int requestHelloWorld( const String & roleName );
    /**
     * \brief   Overwrite to handle error of HelloWorld request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestHelloWorldFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request ShutdownService
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Sent by client to request to shutdown service and the application. This request has no response.
     * \param   clientID    The ID of client that requests to shutdown. The ID is given by service when first time client requests to output message.
     * \param   roleName    Service client component role name
     * \see     Has no response
     **/
    inline void requestShutdownService( unsigned int clientID, const String & roleName );
    /**
     * \brief   Overwrite to handle error of ShutdownService request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestShutdownServiceFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Response HelloWorld
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          The response to hello world request.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientInfo  The client information set by servicing component. If empty or invalid ID, the message output failed.
     * \see     requestHelloWorld
     **/
    virtual void responseHelloWorld( const NEHelloWorld::sConnectedClient & clientInfo );
    /**
     * \brief   Call to enable or disable receiving notifications on HelloWorld response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseHelloWorld( bool notify = true );

/************************************************************************
 * Broadcast ReachedMaximum
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Triggered to notify that  reached the maximum number of requests.
     *          Overwrite, if need to handle Broadcast call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   maxNumber   The maximum number of reqeusts.
     **/
    virtual void broadcastReachedMaximum( int maxNumber );
    /**
     * \brief   Call to enable or disable receiving notifications on ReachedMaximum broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnBroadcastReachedMaximum( bool notify = true );

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
// HelloWorldClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NEHelloWorld::eMessageIDs InvalidReqId );

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
    inline const HelloWorldProxy * getProxy( void ) const;

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
    HelloWorldProxy *   mProxy;

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
// HelloWorldClientBase hidden methods
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
    void notifyOn( NEHelloWorld::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NEHelloWorld::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NEHelloWorld::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of HelloWorldClientBase object
     **/
    inline HelloWorldClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    HelloWorldClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloWorldClientBase );
};

//////////////////////////////////////////////////////////////////////////
// HelloWorldClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline HelloWorldClientBase & HelloWorldClientBase::self( void )
{
    return (*this);
}

inline unsigned int HelloWorldClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void HelloWorldClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool HelloWorldClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool HelloWorldClientBase::isNotificationAssigned( NEHelloWorld::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & HelloWorldClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & HelloWorldClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline unsigned int HelloWorldClientBase::requestHelloWorld( const String & roleName )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestHelloWorld( static_cast<IENotificationEventConsumer &>(self()), roleName );
}

inline void HelloWorldClientBase::requestShutdownService( unsigned int clientID, const String & roleName )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestShutdownService( clientID, roleName );
}

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void HelloWorldClientBase::notifyOnResponseHelloWorld( bool notify /* = true */ )
{
    HelloWorldClientBase::notifyOn(NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld, notify, false);
}

/************************************************************************
 * Broadcast notifications
 ************************************************************************/

inline void HelloWorldClientBase::notifyOnBroadcastReachedMaximum( bool notify /* = true */ )
{
    HelloWorldClientBase::notifyOn(NEHelloWorld::eMessageIDs::MsgId_broadcastReachedMaximum, notify, false);
}

inline const HelloWorldProxy * HelloWorldClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & HelloWorldClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_HELLOWORLDCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/HelloWorldClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
