//////////////////////////////////////////////////////////////////////////
// Begin generate generated/CentralMessagerClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_CENTRALMESSAGERCLIENTBASE_HPP
#define  GENERATED_CENTRALMESSAGERCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:01 GMT+01:00
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 *
 * \file            generated/CentralMessagerClientBase.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/NECentralMessager.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/private/CentralMessagerProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// CentralMessagerClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of CentralMessager Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              This service is used to receive messages from connected edge when typing or 
 *              sending messages. The service receives text message from initiator and
 *              broadcast to all clients, which have subscribed on event.
 *              It as well broadcasts own text message to all connected clients.
 **/
class CentralMessagerClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize CentralMessager Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service CentralMessager role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    CentralMessagerClientBase( const String & roleName, const String & ownerThread = String::getEmptyString() );

    /**
     * \brief   Initialize CentralMessager Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service CentralMessager role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    CentralMessagerClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize CentralMessager Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service CentralMessager role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    CentralMessagerClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize CentralMessager Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service CentralMessager dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    CentralMessagerClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~CentralMessagerClientBase( void );

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
    inline bool isNotificationAssigned( NECentralMessager::eMessageIDs msgId ) const;

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
// CentralMessager Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// CentralMessager Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request SendMessage
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to sends message that all clients can see.
     * \param   nickName    The nick name of initiator
     * \param   cookie      The cookie given by connection manager. Should not be invalid.
     * \param   newMessage  The message sent by connected initiator
     * \param   dateTime    The time-stamp create on local host of initiator
     * \see     Has no response
     **/
    inline void requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime );
    /**
     * \brief   Overwrite to handle error of SendMessage request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestSendMessageFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request KeyTyping
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Sent each time when typing a key
     * \param   nickName    The nick name of initiator
     * \param   cookie      The cookie assigned to initiator
     * \param   newMessage  The message typed.
     * \see     Has no response
     **/
    inline void requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage );
    /**
     * \brief   Overwrite to handle error of KeyTyping request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestKeyTypingFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Broadcast SendMessage
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          The response, sent by connection manager to notify the message typing update
     *          Overwrite, if need to handle Broadcast call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   nickName    The nick name of initiator
     * \param   cookie      The cookie assigned to initiator
     * \param   newMessage  The message sent by initiator
     * \param   dateTime    The local time-stamp of initiator
     **/
    virtual void broadcastSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime );
    /**
     * \brief   Call to enable or disable receiving notifications on SendMessage broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnBroadcastSendMessage( bool notify = true );

/************************************************************************
 * Broadcast KeyTyping
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Sent each time when a client is typing a key
     *          Overwrite, if need to handle Broadcast call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   nickName    The nick name of initiator
     * \param   cookie      Assigned cookie of initiator
     * \param   newMessage  The message typed by initiator
     **/
    virtual void broadcastKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage );
    /**
     * \brief   Call to enable or disable receiving notifications on KeyTyping broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnBroadcastKeyTyping( bool notify = true );

/************************************************************************
 * Broadcast BroadcastMessage
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Server broadcasts a message to all clients.
     *          Overwrite, if need to handle Broadcast call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   serverMessage   The message sent by servicing server
     * \param   dateTime        The time-stamp of servicing component
     **/
    virtual void broadcastBroadcastMessage( const String & serverMessage, const DateTime & dateTime );
    /**
     * \brief   Call to enable or disable receiving notifications on BroadcastMessage broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnBroadcastBroadcastMessage( bool notify = true );

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
// CentralMessagerClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NECentralMessager::eMessageIDs InvalidReqId );

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
    inline const CentralMessagerProxy * getProxy( void ) const;

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
    CentralMessagerProxy *   mProxy;

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
// CentralMessagerClientBase hidden methods
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
    void notifyOn( NECentralMessager::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NECentralMessager::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NECentralMessager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of CentralMessagerClientBase object
     **/
    inline CentralMessagerClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CentralMessagerClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( CentralMessagerClientBase );
};

//////////////////////////////////////////////////////////////////////////
// CentralMessagerClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CentralMessagerClientBase & CentralMessagerClientBase::self( void )
{
    return (*this);
}

inline unsigned int CentralMessagerClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void CentralMessagerClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool CentralMessagerClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool CentralMessagerClientBase::isNotificationAssigned( NECentralMessager::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & CentralMessagerClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & CentralMessagerClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline void CentralMessagerClientBase::requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestSendMessage( nickName, cookie, newMessage, dateTime );
}

inline void CentralMessagerClientBase::requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestKeyTyping( nickName, cookie, newMessage );
}

/************************************************************************
 * Broadcast notifications
 ************************************************************************/

inline void CentralMessagerClientBase::notifyOnBroadcastSendMessage( bool notify /* = true */ )
{
    CentralMessagerClientBase::notifyOn(NECentralMessager::eMessageIDs::MsgId_broadcastSendMessage, notify, false);
}

inline void CentralMessagerClientBase::notifyOnBroadcastKeyTyping( bool notify /* = true */ )
{
    CentralMessagerClientBase::notifyOn(NECentralMessager::eMessageIDs::MsgId_broadcastKeyTyping, notify, false);
}

inline void CentralMessagerClientBase::notifyOnBroadcastBroadcastMessage( bool notify /* = true */ )
{
    CentralMessagerClientBase::notifyOn(NECentralMessager::eMessageIDs::MsgId_broadcastBroadcastMessage, notify, false);
}

inline const CentralMessagerProxy * CentralMessagerClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & CentralMessagerClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_CENTRALMESSAGERCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/CentralMessagerClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
