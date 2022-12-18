//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SystemShutdownProxy.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PRIVATE_SYSTEMSHUTDOWNPROXY_HPP
#define  GENERATED_SRC_PRIVATE_SYSTEMSHUTDOWNPROXY_HPP

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     18.12.2022  15:17:33 GMT+01:00
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 *
 * \file            generated/src/private/SystemShutdownProxy.hpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown
 *                  Service Interface Proxy class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NESystemShutdown.hpp"
#include "areg/component/ProxyBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class SystemShutdownResponseEvent;

//////////////////////////////////////////////////////////////////////////
// SystemShutdownProxy class declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       SystemShutdown Service Interface proxy class.
 *              Automatically instantiated per thread at client startup.
 *              Used to communicate with stub object and sending notifications
 *              to each client running in the thread instance.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 *              This service demonstrates the usage of attribute.
 **/
class SystemShutdownProxy   : public ProxyBase
{
private:
    //////////////////////////////////////////////////////////////////////////
    // SystemShutdownProxy::SystemShutdownServiceAvailableEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The event is used immediately to notify new client about service
     *          availability and escape duplicating notification for other,
     *          already connected clients.
     **/
    class SystemShutdownServiceAvailableEvent : public ProxyBase::ServiceAvailableEvent
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT(SystemShutdownProxy::SystemShutdownServiceAvailableEvent)

    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        explicit SystemShutdownServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor.
         **/
        virtual ~SystemShutdownServiceAvailableEvent( void ) = default;
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        SystemShutdownServiceAvailableEvent( void ) = delete;
        DECLARE_NOCOPY_NOMOVE( SystemShutdownServiceAvailableEvent );
    };
//////////////////////////////////////////////////////////////////////////
// SystemShutdownProxy predefined constants and static members.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object.
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The instance of thread to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static SystemShutdownProxy * createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread );

    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object.
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The name of thread instance to dispatch messages.
     *                          If nullptr, current dispatching thread is used to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static SystemShutdownProxy * createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread = String::EmptyString );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates Proxy object by given role name.
     *          Do not create proxy object directly, instead use CreateProxy function.
     * \param   roleName    The role name of proxy, i.e. role name of server component object.
     * \param   ownerThread The instance of thread to dispatch messages.
     *                      If nullptr, current dispatching thread is used to dispatch messages.
     **/
    explicit SystemShutdownProxy( const String & roleName, DispatcherThread * ownerThread = nullptr );
    /**
     * \brief   Destructor
     **/
    virtual ~SystemShutdownProxy( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attributes
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
     *          Describes the system availability state
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     **/
    inline NESystemShutdown::eServiceState getServiceState( NEService::eDataStateType & state ) const;

/************************************************************************
 * Parameters
 ************************************************************************/
/************************************************************************
 * Requests
 ************************************************************************/
    /**
     * \brief   Request call.
     *          The request to shutdown the system.
     * \see     Has no Response.
     **/
    void requestSystemShutdown( void );

//////////////////////////////////////////////////////////////////////////
// Operations.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Checks whether there is already listener of Notification Event
     *          exists. If does not exit, adds new listener entry in the listener
     *          list. If need, send appropriate message to Stub to start
     *          sending attribute update messages. If already listener
     *          exists in listener list and notifyAlways flag is true,
     *          sends immediate update notification based on existing update data status.
     * \param   msgId           The Notification Message ID
     * \param   consumer        The pointer of Notification Event consumer
     * \param   notifyAlways    The flag, indicating whether the notification event
     *                          should be send to client immediately if the notification
     *                          already exists. By default it is false.
     **/
    inline void setNotification( NESystemShutdown::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways = false );
    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     **/
    inline void clearNotification( NESystemShutdown::eMessageIDs notifyId, IENotificationEventConsumer & listener );
    /**
     * \brief   Clears all notification for specified listener and unregisters it
     * \param   listener    The notification consumer object, which should be unregistered.
     **/
    inline void clearAllNotifications( IENotificationEventConsumer & listener );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEProxyEventConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Method derived from IEProxyEventConsumer interface.
     *          Triggered when on the request to execute function
     *          on server side, Proxy have got response message.
     * \param   eventElem   The Service Response event object.
     *                      Contains response message and information
     *                      sent by Stub
     **/
    virtual void processResponseEvent( ServiceResponseEvent & eventElem ) override;
    /**
     * \brief   Method derived from IEProxyEventConsumer interface.
     *          Triggered when on server side a certain Attribute
     *          value has been updated.
     * \param   eventElem   The Service Response event object.
     *                      Contains new updated value of Attribute
     *                      and validation flag.
     **/
    virtual void processAttributeEvent( ServiceResponseEvent & eventElem ) override;

/************************************************************************/
// ProxyBase interface overrides.
/************************************************************************/

    /**
     * \brief   Creates notification event to send to client objects.
     *          All Notification Events should be internal events and
     *          should be instances of NotificationEvent class.
     * \param   data    The Notification Event data object containing
     *                  notification information.
     * \return  Returns new created notification event object.
     **/
    virtual NotificationEvent * createNotificationEvent( const NotificationEventData & data ) const override;

    /**
     * \brief   Create Request event to send to Stub object.
     *          Request events are triggering request calls on Stub side.
     * \param   args    The buffer containing serialized arguments of request call.
     * \param   reqId   The ID of request call.
     * \return  Return pointer of valid Request event.
     **/
    virtual ServiceRequestEvent * createRequestEvent( const EventDataStream & args, unsigned int reqId ) override;

    /**
     * \brief   Creates event requesting to receive update notification events.
     *          The caller should be address of current proxy and the target
     *          should address of appropriate sub address.
     * \param   msgId       The message ID to send. Should be either attribute or response (info)
     * \param   reqType     The type of request.
     * \return  Returns valid pointer of created service request event object.
     **/
    virtual ServiceRequestEvent * createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType ) override;

    /**
     * \brief   Overwrite method to create response event from streaming object for
     *          further dispatching by proxy.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Response event object.
     *          Otherwise, it returns nullptr.
     **/
    virtual RemoteResponseEvent * createRemoteResponseEvent( const IEInStream & stream ) const override;

    /**
     * \brief   Overwrite method to create error remote response event.
     *          Function is triggered when remote request sent by proxy failed and did not reach target.
     * \param   addrProxy   Address of Proxy object, which sent request event
     * \param   msgId       Message ID, which was requested to process
     * \param   reason      Failure reason set by system
     * \param   seqNr       The sequence number of processing message.
     **/
    virtual RemoteResponseEvent * createRemoteRequestFailedEvent( const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr ) const override;

    /**
     * \brief   Overwrite this method to create service available event to new instantiated clients.
     * \param   consumer    The instance of consumer, which receives service available event.
     * \return  If succeeds, returns valid pointer to service available event object.
     **/
    virtual ProxyBase::ServiceAvailableEvent * createServiceAvailableEvent( IENotificationEventConsumer & consumer ) override;

    /**
     * \brief   Called to register all servicing listeners. It is called when proxy is instantiated.
     *          Overwrite method to add service event listeners.
     **/
    virtual void registerServiceListeners( void ) override;

    /**
     * \brief   Called to unregister all servicing listeners. It is called when proxy is freed.
     *          Overwrite method to remove service event listeners.
     **/
    virtual void unregisterServiceListeners( void ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attribute members
 ************************************************************************/
    /**
     * \brief   ServiceState attribute value.
     *          Describes the system availability state
     **/
    NESystemShutdown::eServiceState mServiceState;

/************************************************************************
 * Parameter members
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Hidden operations
 ************************************************************************/
    /**
     * \brief   Updates attribute and response call variable data received from Stub
     * \param   eventElem   The event object containing buffer with serialized data
     * \param   respId      The response message ID, which received.
     *                      It is either attribute or response message ID
     **/
    void updateData( SystemShutdownResponseEvent & eventElem, NESystemShutdown::eMessageIDs respId );
    /**
     * \brief   Starts processing response message. The received event contains
     *          ID of appropriate message to update and contains result flag.
     *          If request calls failed (busy), the ID in event is a request call ID.
     *          In all other cases, the ID is either response call, info call,
     *          or attribute update notification call.
     *          It will also update parameters and states.
     * \param   evenElem    The response object containing message ID, result and data.
     **/
    void processResponse( SystemShutdownResponseEvent & evenElem );

    /**
     * \brief   returns reference SystemShutdownProxy object
     **/
    inline SystemShutdownProxy & self( void );

/************************************************************************
 * Hidden static operations
 ************************************************************************/
private:
    /**
     * \brief   Create proxy object.
     * \param   roleName    The server component role name
     * \param   ownerThread The instance of thread to dispatch messages.
     *                      If nullptr, current dispatching thread is used to dispatch messages.
     * \return  Pointer to instantiated proxy object.
     **/
    static ProxyBase * _createProxy( const String & roleName, DispatcherThread * ownerThread = nullptr );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    SystemShutdownProxy( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SystemShutdownProxy );
};

//////////////////////////////////////////////////////////////////////////
// SystemShutdownProxy class declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SystemShutdownProxy class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Inline operations
 ************************************************************************/

inline void SystemShutdownProxy::setNotification( NESystemShutdown::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways /* = false */ )
{
    ProxyBase::setNotification(static_cast<unsigned int>(notifyId), &listener, notifyAlways);
}

inline void SystemShutdownProxy::clearNotification( NESystemShutdown::eMessageIDs notifyId, IENotificationEventConsumer & listener )
{
    ProxyBase::clearNotification(static_cast<unsigned int>(notifyId), &listener);
}

inline void SystemShutdownProxy::clearAllNotifications( IENotificationEventConsumer & listener )
{
    ProxyBase::unregisterListener( &listener );
}

inline SystemShutdownProxy & SystemShutdownProxy::self( void )
{
    return (*this);
}

/************************************************************************
 * Inline attributes
 ************************************************************************/

inline bool SystemShutdownProxy::isServiceStateValid( void ) const
{
    return (getProxyData().getAttributeState( static_cast<msg_id>(NESystemShutdown::eMessageIDs::MsgId_ServiceState) ) == NEService::eDataStateType::DataIsOK);
}

inline NESystemShutdown::eServiceState SystemShutdownProxy::getServiceState( NEService::eDataStateType & state ) const
{
    state = getProxyData().getAttributeState( static_cast<msg_id>(NESystemShutdown::eMessageIDs::MsgId_ServiceState) );
    return mServiceState;
}

#endif   // GENERATED_SRC_PRIVATE_SYSTEMSHUTDOWNPROXY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SystemShutdownProxy.hpp file
//////////////////////////////////////////////////////////////////////////
