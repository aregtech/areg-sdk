//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PubSubMixProxy.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PRIVATE_PUBSUBMIXPROXY_HPP
#define  GENERATED_SRC_PRIVATE_PUBSUBMIXPROXY_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     17.09.2023  00:34:02 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PubSubMix.
 *
 * \file            generated/src/private/PubSubMixProxy.hpp
 * \ingroup         PubSubMix Service Interface
 * \brief           This is an automatic generated code of PubSubMix
 *                  Service Interface Proxy class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEPubSubMix.hpp"
#include "areg/component/ProxyBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class PubSubMixResponseEvent;

//////////////////////////////////////////////////////////////////////////
// PubSubMixProxy class declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       PubSubMix Service Interface proxy class.
 *              Automatically instantiated per thread at client startup.
 *              Used to communicate with stub object and sending notifications
 *              to each client running in the thread instance.
 *
 *              This is an interface to test the PubSub features (data-driven communication) of the
 *              AREG framework, where each service provider is a Publisher and each service consumer is a Subscriber that dynamically
 *              subscribes on data update and get notified. The agenda of this example to demonstrate mixture of Publishers and Subscribers
 *              and testing the correct behavior of notifications when Subscribers run in the same or different processes.
 **/
class PubSubMixProxy   : public ProxyBase
{
    friend class PubSubMixClientBase;

private:
    //////////////////////////////////////////////////////////////////////////
    // PubSubMixProxy::PubSubMixServiceAvailableEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The event is used immediately to notify new client about service
     *          availability and escape duplicating notification for other,
     *          already connected clients.
     **/
    class PubSubMixServiceAvailableEvent : public ProxyBase::ServiceAvailableEvent
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT(PubSubMixProxy::PubSubMixServiceAvailableEvent)

    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        explicit PubSubMixServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor.
         **/
        virtual ~PubSubMixServiceAvailableEvent( void ) = default;
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        PubSubMixServiceAvailableEvent( void ) = delete;
        DECLARE_NOCOPY_NOMOVE( PubSubMixServiceAvailableEvent );
    };
//////////////////////////////////////////////////////////////////////////
// PubSubMixProxy predefined constants and static members.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object.
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The instance of thread to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static PubSubMixProxy * createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread );

    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object.
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The name of thread instance to dispatch messages.
     *                          If nullptr, current dispatching thread is used to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static PubSubMixProxy * createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread = String::EmptyString );

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
    explicit PubSubMixProxy( const String & roleName, DispatcherThread * ownerThread = nullptr );
    /**
     * \brief   Destructor
     **/
    virtual ~PubSubMixProxy( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attributes
 ************************************************************************/
    /**
     * \brief   Returns true if value of StringOnChange attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isStringOnChangeValid( void ) const;
    /**
     * \brief   Returns the value of StringOnChange attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute StringOnChange description:
     *          This string should send update notification only on change.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     **/
    inline const NEPubSubMix::sString & getStringOnChange( NEService::eDataStateType & state ) const;

    /**
     * \brief   Returns true if value of IntegerAlways attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isIntegerAlwaysValid( void ) const;
    /**
     * \brief   Returns the value of IntegerAlways attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute IntegerAlways description:
     *          This  integer should send update notification always when set.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     **/
    inline const NEPubSubMix::sInteger & getIntegerAlways( NEService::eDataStateType & state ) const;

    /**
     * \brief   Returns true if value of ServiceProviderState attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isServiceProviderStateValid( void ) const;
    /**
     * \brief   Returns the value of ServiceProviderState attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute ServiceProviderState description:
     *          The state of service provider. The value is always valid, updated on change.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     **/
    inline NEPubSubMix::eServiceState getServiceProviderState( NEService::eDataStateType & state ) const;

/************************************************************************
 * Parameters
 ************************************************************************/
/************************************************************************
 * Requests.
 ************************************************************************/
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
    inline void setNotification( NEPubSubMix::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways = false );
    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     **/
    inline void clearNotification( NEPubSubMix::eMessageIDs notifyId, IENotificationEventConsumer & listener );
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
    virtual RemoteResponseEvent * createRemoteRequestFailedEvent( const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, SequenceNumber seqNr ) const override;

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
     * \brief   StringOnChange attribute value.
     *          This string should send update notification only on change.
     **/
    NEPubSubMix::sString        mStringOnChange;

    /**
     * \brief   IntegerAlways attribute value.
     *          This  integer should send update notification always when set.
     **/
    NEPubSubMix::sInteger       mIntegerAlways;

    /**
     * \brief   ServiceProviderState attribute value.
     *          The state of service provider. The value is always valid, updated on change.
     **/
    NEPubSubMix::eServiceState  mServiceProviderState;

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
    void updateData( PubSubMixResponseEvent & eventElem, NEPubSubMix::eMessageIDs respId );
    /**
     * \brief   Starts processing response message. The received event contains
     *          ID of appropriate message to update and contains result flag.
     *          If request calls failed (busy), the ID in event is a request call ID.
     *          In all other cases, the ID is either response call, info call,
     *          or attribute update notification call.
     *          It will also update parameters and states.
     * \param   evenElem    The response object containing message ID, result and data.
     **/
    void processResponse( PubSubMixResponseEvent & evenElem );

    /**
     * \brief   returns reference PubSubMixProxy object
     **/
    inline PubSubMixProxy & self( void );

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
    PubSubMixProxy( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PubSubMixProxy );
};

//////////////////////////////////////////////////////////////////////////
// PubSubMixProxy class declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PubSubMixProxy class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Inline operations
 ************************************************************************/

inline void PubSubMixProxy::setNotification( NEPubSubMix::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways /* = false */ )
{
    ProxyBase::setNotification(static_cast<unsigned int>(notifyId), &listener, notifyAlways);
}

inline void PubSubMixProxy::clearNotification( NEPubSubMix::eMessageIDs notifyId, IENotificationEventConsumer & listener )
{
    ProxyBase::clearNotification(static_cast<unsigned int>(notifyId), &listener);
}

inline void PubSubMixProxy::clearAllNotifications( IENotificationEventConsumer & listener )
{
    ProxyBase::unregisterListener( &listener );
}

inline PubSubMixProxy & PubSubMixProxy::self( void )
{
    return (*this);
}

/************************************************************************
 * Inline attributes
 ************************************************************************/

inline bool PubSubMixProxy::isStringOnChangeValid( void ) const
{
    return (getProxyData().getAttributeState( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_StringOnChange) ) == NEService::eDataStateType::DataIsOK);
}

inline const NEPubSubMix::sString & PubSubMixProxy::getStringOnChange( NEService::eDataStateType & state ) const
{
    state = getProxyData().getAttributeState( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_StringOnChange) );
    return mStringOnChange;
}

inline bool PubSubMixProxy::isIntegerAlwaysValid( void ) const
{
    return (getProxyData().getAttributeState( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_IntegerAlways) ) == NEService::eDataStateType::DataIsOK);
}

inline const NEPubSubMix::sInteger & PubSubMixProxy::getIntegerAlways( NEService::eDataStateType & state ) const
{
    state = getProxyData().getAttributeState( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_IntegerAlways) );
    return mIntegerAlways;
}

inline bool PubSubMixProxy::isServiceProviderStateValid( void ) const
{
    return (getProxyData().getAttributeState( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState) ) == NEService::eDataStateType::DataIsOK);
}

inline NEPubSubMix::eServiceState PubSubMixProxy::getServiceProviderState( NEService::eDataStateType & state ) const
{
    state = getProxyData().getAttributeState( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState) );
    return mServiceProviderState;
}

#endif   // GENERATED_SRC_PRIVATE_PUBSUBMIXPROXY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PubSubMixProxy.hpp file
//////////////////////////////////////////////////////////////////////////
