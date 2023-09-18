//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/DirectConnectionClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_DIRECTCONNECTIONCLIENTBASE_HPP
#define  GENERATED_SRC_DIRECTCONNECTIONCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     17.09.2023  00:34:02 GMT+02:00
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 *
 * \file            generated/src/DirectConnectionClientBase.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEDirectConnection.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/DirectConnectionProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// DirectConnectionClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of DirectConnection Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              Direct Connection service interface. It is used to send connection messages 
 *              to distributed applications. As soon as the distributed applications are connected, 
 *              each application has one instance of implemented service where 
 *              the role name is a nick name of a registered application.
 **/
class DirectConnectionClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize DirectConnection Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service DirectConnection role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    DirectConnectionClientBase( const String & roleName, const String & ownerThread = String::EmptyString );

    /**
     * \brief   Initialize DirectConnection Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service DirectConnection role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    DirectConnectionClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize DirectConnection Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service DirectConnection role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    DirectConnectionClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize DirectConnection Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service DirectConnection dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    DirectConnectionClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~DirectConnectionClientBase( void );

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
    inline bool isNotificationAssigned( NEDirectConnection::eMessageIDs msgId ) const;

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
// DirectConnection Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Attribute InitiatedConnections functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of InitiatedConnections attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isInitiatedConnectionsValid( void ) const;
    /**
     * \brief   Returns the value of InitiatedConnections attribute.
     *          To get valid value, the Update Notification should be enabled.
     *          Attribute InitiatedConnections description:
     *          The map of initiated connections and list of participants.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isInitiatedConnectionsValid, notifyInitiatedConnectionsUpdate, onInitiatedConnectionsUpdate
     **/
    inline const NEDirectConnection::MapParticipants & getInitiatedConnections( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on InitiatedConnections attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getInitiatedConnections method will return valid data
     *          Attribute InitiatedConnections description:
     *          The map of initiated connections and list of participants.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isInitiatedConnectionsValid, getInitiatedConnections, onInitiatedConnectionsUpdate
     **/
    inline void notifyOnInitiatedConnectionsUpdate( bool notify = true );
    /**
     * \brief   Triggered, when InitiatedConnections attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes InitiatedConnections description:
     *          The map of initiated connections and list of participants.
     * \param   InitiatedConnections    The value of InitiatedConnections attribute.
     * \param   state                   The data validation flag.
     **/
    virtual void onInitiatedConnectionsUpdate( const NEDirectConnection::MapParticipants & InitiatedConnections, NEService::eDataStateType state );

//////////////////////////////////////////////////////////////////////////
// DirectConnection Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request ConnectoinSetup
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to setup the direct connection to chat room initiator.
     * \param   initiator           The initiator to setup connection
     * \param   listParticipants    The list of chat-room participants
     * \return  The sequence count number of call
     * \see     responseConnectoinSetup
     **/
    inline SequenceNumber requestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
    /**
     * \brief   Overwrite to handle error of ConnectoinSetup request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestConnectoinSetupFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request AddParticipant
 ************************************************************************/
    /**
     * \brief   Request call.
     *          The request to add a participant in the direct chat-room.
     * \param   initiator           The initiator to add to chat-room
     * \param   listParticipants    List of participants
     * \return  The sequence count number of call
     * \see     responseAddParticipant
     **/
    inline SequenceNumber requestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
    /**
     * \brief   Overwrite to handle error of AddParticipant request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestAddParticipantFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request RemoveParticipant
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to remove initiator from chat-room
     * \param   initiator           The initiator to remove from chat-room
     * \param   listParticipants    List of chat-room participants
     * \return  The sequence count number of call
     * \see     responseRemoveParticipant
     **/
    inline SequenceNumber requestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
    /**
     * \brief   Overwrite to handle error of RemoveParticipant request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestRemoveParticipantFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request CloseConnection
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to close chat-room. There is no response expected.
     * \param   initiator   The initiator to close chat-room.
     * \see     Has no response
     **/
    inline void requestCloseConnection( const NEDirectConnection::sInitiator & initiator );
    /**
     * \brief   Overwrite to handle error of CloseConnection request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestCloseConnectionFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Response ConnectoinSetup
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          The response of connection setup
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   succeeded           Flag, indicating whether the connection setup succeeded or not.
     * \param   target              The targeted participant to include in chat-room
     * \param   initiator           The chat-room initiator
     * \param   listParticipants    List of chat-room participants
     * \see     requestConnectoinSetup
     **/
    virtual void responseConnectoinSetup( bool succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
    /**
     * \brief   Call to enable or disable receiving notifications on ConnectoinSetup response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseConnectoinSetup( bool notify = true );

/************************************************************************
 * Response AddParticipant
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          The response to add initiator to chat room
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   succeeded           Flag, indicating whether operation succeeded.
     * \param   listParticipants    New list of participants
     * \see     requestAddParticipant
     **/
    virtual void responseAddParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants );
    /**
     * \brief   Call to enable or disable receiving notifications on AddParticipant response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseAddParticipant( bool notify = true );

/************************************************************************
 * Response RemoveParticipant
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Response to remove initiator from chat-room
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   succeeded           Flag, indicating whther operation succeeded or not.
     * \param   listParticipants    New lsit of chat-room participants.
     * \see     requestRemoveParticipant
     **/
    virtual void responseRemoveParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants );
    /**
     * \brief   Call to enable or disable receiving notifications on RemoveParticipant response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseRemoveParticipant( bool notify = true );

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
// DirectConnectionClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NEDirectConnection::eMessageIDs InvalidReqId );

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
    inline const DirectConnectionProxy * getProxy( void ) const;

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
    DirectConnectionProxy *   mProxy;

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
// DirectConnectionClientBase hidden methods
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
    void notifyOn( NEDirectConnection::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NEDirectConnection::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NEDirectConnection::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of DirectConnectionClientBase object
     **/
    inline DirectConnectionClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DirectConnectionClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( DirectConnectionClientBase );
};

//////////////////////////////////////////////////////////////////////////
// DirectConnectionClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline DirectConnectionClientBase & DirectConnectionClientBase::self( void )
{
    return (*this);
}

inline const SequenceNumber & DirectConnectionClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void DirectConnectionClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool DirectConnectionClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool DirectConnectionClientBase::isNotificationAssigned( NEDirectConnection::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & DirectConnectionClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & DirectConnectionClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool DirectConnectionClientBase::isInitiatedConnectionsValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isInitiatedConnectionsValid( );
}
inline const NEDirectConnection::MapParticipants & DirectConnectionClientBase::getInitiatedConnections( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getInitiatedConnections( state );
}

inline void DirectConnectionClientBase::notifyOnInitiatedConnectionsUpdate( bool notify /* = true */ )
{
    DirectConnectionClientBase::notifyOn( NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections, notify, false );
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline SequenceNumber DirectConnectionClientBase::requestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestConnectoinSetup( static_cast<IENotificationEventConsumer &>(self()), initiator, listParticipants );
}

inline SequenceNumber DirectConnectionClientBase::requestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestAddParticipant( static_cast<IENotificationEventConsumer &>(self()), initiator, listParticipants );
}

inline SequenceNumber DirectConnectionClientBase::requestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestRemoveParticipant( static_cast<IENotificationEventConsumer &>(self()), initiator, listParticipants );
}

inline void DirectConnectionClientBase::requestCloseConnection( const NEDirectConnection::sInitiator & initiator )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestCloseConnection( initiator );
}

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void DirectConnectionClientBase::notifyOnResponseConnectoinSetup( bool notify /* = true */ )
{
    DirectConnectionClientBase::notifyOn(NEDirectConnection::eMessageIDs::MsgId_responseConnectoinSetup, notify, false);
}

inline void DirectConnectionClientBase::notifyOnResponseAddParticipant( bool notify /* = true */ )
{
    DirectConnectionClientBase::notifyOn(NEDirectConnection::eMessageIDs::MsgId_responseAddParticipant, notify, false);
}

inline void DirectConnectionClientBase::notifyOnResponseRemoveParticipant( bool notify /* = true */ )
{
    DirectConnectionClientBase::notifyOn(NEDirectConnection::eMessageIDs::MsgId_responseRemoveParticipant, notify, false);
}

inline const DirectConnectionProxy * DirectConnectionClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & DirectConnectionClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_DIRECTCONNECTIONCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/DirectConnectionClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
