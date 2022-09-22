//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/OptionsProxy.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PRIVATE_OPTIONSPROXY_HPP
#define  GENERATED_SRC_PRIVATE_OPTIONSPROXY_HPP

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.09.2022  23:32:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source Options.
 *
 * \file            generated/src/private/OptionsProxy.hpp
 * \ingroup         Options Service Interface
 * \brief           This is an automatic generated code of Options
 *                  Service Interface Proxy class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEOptions.hpp"
#include "areg/component/ProxyBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class OptionsResponseEvent;

//////////////////////////////////////////////////////////////////////////
// OptionsProxy class declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Options Service Interface proxy class.
 *              Automatically instantiated per thread at client startup.
 *              Used to communicate with stub object and sending notifications
 *              to each client running in the thread instance.
 *
 *              This SI sets options to generate data
 **/
class OptionsProxy   : public ProxyBase
{
private:
    //////////////////////////////////////////////////////////////////////////
    // OptionsProxy::OptionsServiceAvailableEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The event is used immediately to notify new client about service
     *          availability and escape duplicating notification for other,
     *          already connected clients.
     **/
    class OptionsServiceAvailableEvent : public ProxyBase::ServiceAvailableEvent
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT(OptionsProxy::OptionsServiceAvailableEvent)

    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        explicit OptionsServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor.
         **/
        virtual ~OptionsServiceAvailableEvent( void ) = default;
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        OptionsServiceAvailableEvent( void ) = delete;
        DECLARE_NOCOPY_NOMOVE( OptionsServiceAvailableEvent );
    };
//////////////////////////////////////////////////////////////////////////
// OptionsProxy predefined constants and static members.
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
    static OptionsProxy * createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread );

    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object.
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The name of thread instance to dispatch messages.
     *                          If nullptr, current dispatching thread is used to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static OptionsProxy * createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread = String::EmptyString );

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
    explicit OptionsProxy( const String & roleName, DispatcherThread * ownerThread = nullptr );
    /**
     * \brief   Destructor
     **/
    virtual ~OptionsProxy( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attributes
 ************************************************************************/
    /**
     * \brief   Returns true if value of OptionData attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isOptionDataValid( void ) const;
    /**
     * \brief   Returns the value of OptionData attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute OptionData description:
     *          The options
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     **/
    inline const NEOptions::sOptionData & getOptionData( NEService::eDataStateType & state ) const;

/************************************************************************
 * Parameters
 ************************************************************************/
    /**
     * \brief   Returns value of sentData of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter sentData description:
     *          Computed rate of sent data per second.
     **/
    inline const NEOptions::sDataRate & getParamsentData( void ) const;

    /**
     * \brief   Returns value of missedData of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter missedData description:
     *          Computed rate of missed data. The data is missed if failed to copy within certain timeshlot.
     *          TODO: remove if useless.
     **/
    inline const NEOptions::sDataRate & getParammissedData( void ) const;

    /**
     * \brief   Returns value of genPackets of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter genPackets description:
     *          Number of generate packet per second
     **/
    inline unsigned int getParamgenPackets( void ) const;

    /**
     * \brief   Returns value of avSize of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter avSize description:
     *          The average size of a packet.
     **/
    inline const NEOptions::sDataRate & getParamavSize( void ) const;

/************************************************************************
 * Requests
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to get data rate values
     * \param   caller  The reference of caller object to get response.
     * \return  The sequence count number of call
     * \see     responseDataRate
     **/
    unsigned int requestDataRate( IENotificationEventConsumer & caller );

    /**
     * \brief   Request call.
     *          Request to get packetrate, i.e. the numberof generated image blocks per second
     * \param   caller  The reference of caller object to get response.
     * \return  The sequence count number of call
     * \see     responsePacketRate
     **/
    unsigned int requestPacketRate( IENotificationEventConsumer & caller );

    /**
     * \brief   Request call.
     *          Request to change the options to generate images.
     * \param   Options DESCRIPTION MISSED
     * \see     Has no Response.
     **/
    void requestChangeOptions( const NEOptions::sOptionData & Options );

    /**
     * \brief   Request call.
     *          DESCRIPTION MISSED
     * \see     Has no Response.
     **/
    void requestQuitApplication( void );

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
    inline void setNotification( NEOptions::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways = false );
    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     **/
    inline void clearNotification( NEOptions::eMessageIDs notifyId, IENotificationEventConsumer & listener );
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
     * \brief   OptionData attribute value.
     *          The options
     **/
    NEOptions::sOptionData  mOptionData;

/************************************************************************
 * Parameter members
 ************************************************************************/
    /**
     * \brief   sentData parameter value.
     *          Computed rate of sent data per second.
     **/
    NEOptions::sDataRate    mParamsentData;

    /**
     * \brief   missedData parameter value.
     *          Computed rate of missed data. The data is missed if failed to copy within certain timeshlot.
     *          TODO: remove if useless.
     **/
    NEOptions::sDataRate    mParammissedData;

    /**
     * \brief   genPackets parameter value.
     *          Number of generate packet per second
     **/
    unsigned int            mParamgenPackets;

    /**
     * \brief   avSize parameter value.
     *          The average size of a packet.
     **/
    NEOptions::sDataRate    mParamavSize;

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
    void updateData( OptionsResponseEvent & eventElem, NEOptions::eMessageIDs respId );
    /**
     * \brief   Starts processing response message. The received event contains
     *          ID of appropriate message to update and contains result flag.
     *          If request calls failed (busy), the ID in event is a request call ID.
     *          In all other cases, the ID is either response call, info call,
     *          or attribute update notification call.
     *          It will also update parameters and states.
     * \param   evenElem    The response object containing message ID, result and data.
     **/
    void processResponse( OptionsResponseEvent & evenElem );

    /**
     * \brief   returns reference OptionsProxy object
     **/
    inline OptionsProxy & self( void );

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
    OptionsProxy( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( OptionsProxy );
};

//////////////////////////////////////////////////////////////////////////
// OptionsProxy class declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// OptionsProxy class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Inline operations
 ************************************************************************/

inline void OptionsProxy::setNotification( NEOptions::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways /* = false */ )
{
    ProxyBase::setNotification(static_cast<unsigned int>(notifyId), &listener, notifyAlways);
}

inline void OptionsProxy::clearNotification( NEOptions::eMessageIDs notifyId, IENotificationEventConsumer & listener )
{
    ProxyBase::clearNotification(static_cast<unsigned int>(notifyId), &listener);
}

inline void OptionsProxy::clearAllNotifications( IENotificationEventConsumer & listener )
{
    ProxyBase::unregisterListener( &listener );
}

inline OptionsProxy & OptionsProxy::self( void )
{
    return (*this);
}

/************************************************************************
 * Inline attributes
 ************************************************************************/

inline bool OptionsProxy::isOptionDataValid( void ) const
{
    return (getProxyData().getAttributeState( static_cast<msg_id>(NEOptions::eMessageIDs::MsgId_OptionData) ) == NEService::eDataStateType::DataIsOK);
}

inline const NEOptions::sOptionData & OptionsProxy::getOptionData( NEService::eDataStateType & state ) const
{
    state = getProxyData().getAttributeState( static_cast<msg_id>(NEOptions::eMessageIDs::MsgId_OptionData) );
    return mOptionData;
}

/************************************************************************
 * Inline parameters
 ************************************************************************/

inline const NEOptions::sDataRate & OptionsProxy::getParamsentData( void ) const
{
    return mParamsentData;
}

inline const NEOptions::sDataRate & OptionsProxy::getParammissedData( void ) const
{
    return mParammissedData;
}

inline unsigned int OptionsProxy::getParamgenPackets( void ) const
{
    return mParamgenPackets;
}

inline const NEOptions::sDataRate & OptionsProxy::getParamavSize( void ) const
{
    return mParamavSize;
}

#endif   // GENERATED_SRC_PRIVATE_OPTIONSPROXY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/OptionsProxy.hpp file
//////////////////////////////////////////////////////////////////////////
