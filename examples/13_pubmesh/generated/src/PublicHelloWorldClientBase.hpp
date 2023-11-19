//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/PublicHelloWorldClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PUBLICHELLOWORLDCLIENTBASE_HPP
#define  GENERATED_SRC_PUBLICHELLOWORLDCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     15.11.2023  14:51:46 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PublicHelloWorld.
 *
 * \file            generated/src/PublicHelloWorldClientBase.hpp
 * \ingroup         PublicHelloWorld Service Interface
 * \brief           This is an automatic generated code of PublicHelloWorld
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEPublicHelloWorld.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/PublicHelloWorldProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of PublicHelloWorld Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 *              This interface demonstrates how to use multiple remote services in the system.
 *              Each service is unique in the network.
 *              This service is used by each remote servicing component to register clients.
 *              Before using services, the clients should be registered.
 **/
class PublicHelloWorldClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize PublicHelloWorld Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service PublicHelloWorld role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    PublicHelloWorldClientBase( const String & roleName, const String & ownerThread = String::EmptyString );

    /**
     * \brief   Initialize PublicHelloWorld Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service PublicHelloWorld role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    PublicHelloWorldClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize PublicHelloWorld Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service PublicHelloWorld role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    PublicHelloWorldClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize PublicHelloWorld Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service PublicHelloWorld dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    PublicHelloWorldClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~PublicHelloWorldClientBase( void );

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
    inline bool isNotificationAssigned( NEPublicHelloWorld::eMessageIDs msgId ) const;

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
// PublicHelloWorld Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorld Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request Register
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Call to register client. Each client should be registered before starting communication.
     * \param   name    The name of the client.
     * \param   service The service address of the client.
     * \param   thread  The thread name where client is running. Required to provide uniqueness.
     * \param   process The name of process. Optional parameter, used to make output in logs.
     * \return  The sequence count number of call
     * \see     responseRegister
     **/
    inline const SequenceNumber &  requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process );
    /**
     * \brief   Overwrite to handle error of Register request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestRegisterFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request Unregister
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Sent to unregister connected client.
     * \param   client  The client registration object indicating the unregistered client.
     * \see     Has no response
     **/
    inline void requestUnregister( const NEPublicHelloWorld::sClientRegister & client );
    /**
     * \brief   Overwrite to handle error of Unregister request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestUnregisterFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request HelloWorld
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Outputs message on console.
     * \param   clientID    The ID of registered client to make message output.
     * \return  The sequence count number of call
     * \see     responseHelloWorld
     **/
    inline const SequenceNumber &  requestHelloWorld( unsigned int clientID );
    /**
     * \brief   Overwrite to handle error of HelloWorld request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestHelloWorldFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Response Register
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Sent when the client is registered. Each registered client has unique ID.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   client  The client registration object that contains unique ID.
     * \see     requestRegister
     **/
    virtual void responseRegister( const NEPublicHelloWorld::sClientRegister & client );
    /**
     * \brief   Call to enable or disable receiving notifications on Register response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseRegister( bool notify = true );

/************************************************************************
 * Response HelloWorld
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Called as reply to the request to output message.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientID    Indicates the ID client that made message output
     * \see     requestHelloWorld
     **/
    virtual void responseHelloWorld( unsigned int clientID );
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
// PublicHelloWorldClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NEPublicHelloWorld::eMessageIDs InvalidReqId );

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
    inline const PublicHelloWorldProxy * getProxy( void ) const;

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
    PublicHelloWorldProxy *   mProxy;

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
// PublicHelloWorldClientBase hidden methods
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
    void notifyOn( NEPublicHelloWorld::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NEPublicHelloWorld::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NEPublicHelloWorld::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of PublicHelloWorldClientBase object
     **/
    inline PublicHelloWorldClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PublicHelloWorldClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PublicHelloWorldClientBase );
};

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline PublicHelloWorldClientBase & PublicHelloWorldClientBase::self( void )
{
    return (*this);
}

inline const SequenceNumber & PublicHelloWorldClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void PublicHelloWorldClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool PublicHelloWorldClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool PublicHelloWorldClientBase::isNotificationAssigned( NEPublicHelloWorld::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & PublicHelloWorldClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & PublicHelloWorldClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline const SequenceNumber &  PublicHelloWorldClientBase::requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestRegister( static_cast<IENotificationEventConsumer &>(self()), name, service, thread, process );
}

inline void PublicHelloWorldClientBase::requestUnregister( const NEPublicHelloWorld::sClientRegister & client )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestUnregister( client );
}

inline const SequenceNumber &  PublicHelloWorldClientBase::requestHelloWorld( unsigned int clientID )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestHelloWorld( static_cast<IENotificationEventConsumer &>(self()), clientID );
}

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void PublicHelloWorldClientBase::notifyOnResponseRegister( bool notify /* = true */ )
{
    PublicHelloWorldClientBase::notifyOn(NEPublicHelloWorld::eMessageIDs::MsgId_responseRegister, notify, false);
}

inline void PublicHelloWorldClientBase::notifyOnResponseHelloWorld( bool notify /* = true */ )
{
    PublicHelloWorldClientBase::notifyOn(NEPublicHelloWorld::eMessageIDs::MsgId_responseHelloWorld, notify, false);
}

inline const PublicHelloWorldProxy * PublicHelloWorldClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & PublicHelloWorldClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_PUBLICHELLOWORLDCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/PublicHelloWorldClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
