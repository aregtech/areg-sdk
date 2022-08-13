//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/RemoteRegistryClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_REMOTEREGISTRYCLIENTBASE_HPP
#define  GENERATED_SRC_REMOTEREGISTRYCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     13.08.2022  13:59:48 GMT+02:00
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 *
 * \file            generated/src/RemoteRegistryClientBase.hpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NERemoteRegistry.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/RemoteRegistryProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of RemoteRegistry Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 *              This interface demonstrates how to use multiple remote services in the system.
 *              Each service is unique in the network.
 *              This service is used by each remote servicing component to register clients.
 *              Before using services, the clients should be registered.
 **/
class RemoteRegistryClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize RemoteRegistry Service Interface client object.
     *          Specifies used service and owner thread.
     * \param   roleName    The role name assigned to RemoteRegistry servicing component object.
     * \param   ownerThread The name of component owner thread to dispatch messages.
     *                      If nullptr, all messages are dispatched in current component thread.
     **/
    RemoteRegistryClientBase( const String & roleName, const String & ownerThread = String::EmptyString );

    /**
     * \brief   Initialize RemoteRegistry Service Interface client object.
     *          Specifies used service and owner thread.
     * \param   roleName    The role name assigned to RemoteRegistry servicing component object.
     * \param   ownerThread The instance of component owner thread to dispatch messages.
     **/
    RemoteRegistryClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize RemoteRegistry Service Interface client object.
     *          Specifies used service and owner component.
     * \param   roleName    The role name assigned to RemoteRegistry servicing component object.
     * \param   owner       The instance of client owner component. The component object should be already initialized.
     * \note    When this constructor is used, it is important that the Component object is already initialized.
     *          and the component thread is set.
     **/
    RemoteRegistryClientBase( const String & roleName, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteRegistryClientBase( void );

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
    inline bool isNotificationAssigned( NERemoteRegistry::eMessageIDs msgId ) const;

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
// RemoteRegistry Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Attribute RegistryList functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of RegistryList attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isRegistryListValid( void ) const;
    /**
     * \brief   Returns the value of RegistryList attribute.
     *          To get valid value, the Update Notification should be enabled.
     *          Attribute RegistryList description:
     *          List of registered clients.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isRegistryListValid, notifyRegistryListUpdate, onRegistryListUpdate
     **/
    inline const NERemoteRegistry::ListRegistry & getRegistryList( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on RegistryList attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getRegistryList method will return valid data
     *          Attribute RegistryList description:
     *          List of registered clients.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isRegistryListValid, getRegistryList, onRegistryListUpdate
     **/
    inline void notifyOnRegistryListUpdate( bool notify = true );
    /**
     * \brief   Triggered, when RegistryList attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes RegistryList description:
     *          List of registered clients.
     * \param   RegistryList    The value of RegistryList attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onRegistryListUpdate( const NERemoteRegistry::ListRegistry & RegistryList, NEService::eDataStateType state );

/************************************************************************
 * Attribute RemainOutputs functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of RemainOutputs attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isRemainOutputsValid( void ) const;
    /**
     * \brief   Returns the value of RemainOutputs attribute.
     *          To get valid value, the Update Notification should be enabled.
     *          Attribute RemainOutputs description:
     *          Remaining outputs before reaching maximum.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isRemainOutputsValid, notifyRemainOutputsUpdate, onRemainOutputsUpdate
     **/
    inline unsigned int getRemainOutputs( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on RemainOutputs attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getRemainOutputs method will return valid data
     *          Attribute RemainOutputs description:
     *          Remaining outputs before reaching maximum.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isRemainOutputsValid, getRemainOutputs, onRemainOutputsUpdate
     **/
    inline void notifyOnRemainOutputsUpdate( bool notify = true );
    /**
     * \brief   Triggered, when RemainOutputs attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes RemainOutputs description:
     *          Remaining outputs before reaching maximum.
     * \param   RemainOutputs   The value of RemainOutputs attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onRemainOutputsUpdate( unsigned int RemainOutputs, NEService::eDataStateType state );

//////////////////////////////////////////////////////////////////////////
// RemoteRegistry Interface Requests / Responses / Broadcasts
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
    inline unsigned int requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process );
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
    inline void requestUnregister( const NERemoteRegistry::sClientRegister & client );
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
    inline unsigned int requestHelloWorld( unsigned int clientID );
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
    virtual void responseRegister( const NERemoteRegistry::sClientRegister & client );
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
// RemoteRegistryClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NERemoteRegistry::eMessageIDs InvalidReqId );

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
    inline const RemoteRegistryProxy * getProxy( void ) const;

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
    RemoteRegistryProxy *   mProxy;

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
// RemoteRegistryClientBase hidden methods
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
    void notifyOn( NERemoteRegistry::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NERemoteRegistry::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NERemoteRegistry::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of RemoteRegistryClientBase object
     **/
    inline RemoteRegistryClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RemoteRegistryClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( RemoteRegistryClientBase );
};

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline RemoteRegistryClientBase & RemoteRegistryClientBase::self( void )
{
    return (*this);
}

inline unsigned int RemoteRegistryClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void RemoteRegistryClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool RemoteRegistryClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool RemoteRegistryClientBase::isNotificationAssigned( NERemoteRegistry::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & RemoteRegistryClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & RemoteRegistryClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool RemoteRegistryClientBase::isRegistryListValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isRegistryListValid( );
}
inline const NERemoteRegistry::ListRegistry & RemoteRegistryClientBase::getRegistryList( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getRegistryList( state );
}

inline void RemoteRegistryClientBase::notifyOnRegistryListUpdate( bool notify /* = true */ )
{
    RemoteRegistryClientBase::notifyOn( NERemoteRegistry::eMessageIDs::MsgId_RegistryList, notify, false );
}

inline bool RemoteRegistryClientBase::isRemainOutputsValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isRemainOutputsValid( );
}
inline unsigned int RemoteRegistryClientBase::getRemainOutputs( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getRemainOutputs( state );
}

inline void RemoteRegistryClientBase::notifyOnRemainOutputsUpdate( bool notify /* = true */ )
{
    RemoteRegistryClientBase::notifyOn( NERemoteRegistry::eMessageIDs::MsgId_RemainOutputs, notify, false );
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline unsigned int RemoteRegistryClientBase::requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestRegister( static_cast<IENotificationEventConsumer &>(self()), name, service, thread, process );
}

inline void RemoteRegistryClientBase::requestUnregister( const NERemoteRegistry::sClientRegister & client )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestUnregister( client );
}

inline unsigned int RemoteRegistryClientBase::requestHelloWorld( unsigned int clientID )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestHelloWorld( static_cast<IENotificationEventConsumer &>(self()), clientID );
}

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void RemoteRegistryClientBase::notifyOnResponseRegister( bool notify /* = true */ )
{
    RemoteRegistryClientBase::notifyOn(NERemoteRegistry::eMessageIDs::MsgId_responseRegister, notify, false);
}

inline void RemoteRegistryClientBase::notifyOnResponseHelloWorld( bool notify /* = true */ )
{
    RemoteRegistryClientBase::notifyOn(NERemoteRegistry::eMessageIDs::MsgId_responseHelloWorld, notify, false);
}

inline const RemoteRegistryProxy * RemoteRegistryClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & RemoteRegistryClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_REMOTEREGISTRYCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/RemoteRegistryClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
