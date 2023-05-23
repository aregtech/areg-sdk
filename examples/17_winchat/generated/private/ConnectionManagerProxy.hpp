//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/ConnectionManagerProxy.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_PRIVATE_CONNECTIONMANAGERPROXY_HPP
#define  GENERATED_PRIVATE_CONNECTIONMANAGERPROXY_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:02 GMT+01:00
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 *
 * \file            generated/private/ConnectionManagerProxy.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager
 *                  Service Interface Proxy class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/NEConnectionManager.hpp"
#include "areg/component/ProxyBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class ConnectionManagerResponseEvent;

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerProxy class declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       ConnectionManager Service Interface proxy class.
 *              Automatically instantiated per thread at client startup.
 *              Used to communicate with stub object and sending notifications
 *              to each client running in the thread instance.
 *
 *              The connection manager to register each application with client
 **/
class ConnectionManagerProxy   : public ProxyBase
{
    friend class ConnectionManagerClientBase;
    
private:
    //////////////////////////////////////////////////////////////////////////
    // ConnectionManagerProxy::ConnectionManagerServiceAvailableEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The event is used immediately to notify new client about service
     *          availability and escape duplicating notification for other,
     *          already connected clients.
     **/
    class ConnectionManagerServiceAvailableEvent : public ProxyBase::ServiceAvailableEvent
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT(ConnectionManagerProxy::ConnectionManagerServiceAvailableEvent)

    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        explicit ConnectionManagerServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor.
         **/
        virtual ~ConnectionManagerServiceAvailableEvent( void ) = default;
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        ConnectionManagerServiceAvailableEvent( void ) = delete;
        DECLARE_NOCOPY_NOMOVE( ConnectionManagerServiceAvailableEvent );
    };
//////////////////////////////////////////////////////////////////////////
// ConnectionManagerProxy predefined constants and static members.
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
    static ConnectionManagerProxy * createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread );

    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object.
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The name of thread instance to dispatch messages.
     *                          If nullptr, current dispatching thread is used to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static ConnectionManagerProxy * createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread = String::getEmptyString() );

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
    explicit ConnectionManagerProxy( const String & roleName, DispatcherThread * ownerThread = nullptr );
    /**
     * \brief   Destructor
     **/
    virtual ~ConnectionManagerProxy( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attributes
 ************************************************************************/
    /**
     * \brief   Returns true if value of ConnectionList attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isConnectionListValid( void ) const;
    /**
     * \brief   Returns the value of ConnectionList attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute ConnectionList description:
     *          The map of all active connections
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     **/
    inline const NEConnectionManager::MapConnection & getConnectionList( NEService::eDataStateType & state ) const;

/************************************************************************
 * Parameters
 ************************************************************************/
    /**
     * \brief   Returns value of nickName of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter nickName description:
     *          Client connection data, which contains nick name and connection date-time
     **/
    inline const String & getParamnickName( void ) const;

    /**
     * \brief   Returns value of cookie of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter cookie description:
     *          Connection assigned cookie for client service
     **/
    inline unsigned int getParamcookie( void ) const;

    /**
     * \brief   Returns value of dateTime of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter dateTime description:
     *          The client service connection time-stamp in UTC
     **/
    inline const DateTime & getParamdateTime( void ) const;

    /**
     * \brief   Returns value of result of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter result description:
     *          The connection operation result
     **/
    inline NEConnectionManager::eConnectionResult getParamresult( void ) const;

    /**
     * \brief   Returns value of connection of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter connection description:
     *          The connection structure after registration.
     **/
    inline const NEConnectionManager::sConnection & getParamconnection( void ) const;

    /**
     * \brief   Returns value of connectionList of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter connectionList description:
     *          The list of all servicing connections.
     **/
    inline const NEConnectionManager::ListConnection & getParamconnectionList( void ) const;

    /**
     * \brief   Returns value of success of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter success description:
     *          Flag, indicating whether the request to register connection succeeded or not.
     **/
    inline bool getParamsuccess( void ) const;

    /**
     * \brief   Returns value of updatedList of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter updatedList description:
     *          The complete list of clients to send.
     **/
    inline const NEConnectionManager::MapConnection & getParamupdatedList( void ) const;

    /**
     * \brief   Returns value of clientConnected of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter clientConnected description:
     *          New client data, which contains nick name and connected date-time
     **/
    inline const NEConnectionManager::sConnection & getParamclientConnected( void ) const;

    /**
     * \brief   Returns value of clientLeft of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter clientLeft description:
     *          The connection data of disconnected client.
     **/
    inline const NEConnectionManager::sConnection & getParamclientLeft( void ) const;

/************************************************************************
 * Requests
 ************************************************************************/
    /**
     * \brief   Request call.
     *          The request to connect to system
     * \param   caller      The reference of caller object to get response.
     * \param   nickName    The nick name of connecting client service
     * \param   dateTime    The connecting client connection time-stamp in UTC
     * \return  The sequence count number of call
     * \see     responseConnect
     **/
    unsigned int requestConnect( IENotificationEventConsumer & caller, const String & nickName, const DateTime & dateTime );

    /**
     * \brief   Request call.
     *          Request to register direct connection service.
     * \param   caller          The reference of caller object to get response.
     * \param   nickName        The nick name of connection used in service
     * \param   cookie          The cookie value to set
     * \param   connectCookie   The cookie to use for registration. 
     * \param   dateRegister    The connection registration time-stamp
     * \return  The sequence count number of call
     * \see     responseRegisterConnection
     **/
    unsigned int requestRegisterConnection( IENotificationEventConsumer & caller, const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister );

    /**
     * \brief   Request call.
     *          Request to disconnect.
     * \param   nickName    The nick name of client connection service
     * \param   cookie      The cookie given by connection manager. If invalid, it will search by nickName parameter
     * \param   dateTime    Disconnect request date-time
     * \see     Has no Response.
     **/
    void requestDisconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime );

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
    inline void setNotification( NEConnectionManager::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways = false );
    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     **/
    inline void clearNotification( NEConnectionManager::eMessageIDs notifyId, IENotificationEventConsumer & listener );
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
     * \brief   ConnectionList attribute value.
     *          The map of all active connections
     **/
    NEConnectionManager::MapConnection      mConnectionList;

/************************************************************************
 * Parameter members
 ************************************************************************/
    /**
     * \brief   nickName parameter value.
     *          Client connection data, which contains nick name and connection date-time
     **/
    String                                  mParamnickName;

    /**
     * \brief   cookie parameter value.
     *          Connection assigned cookie for client service
     **/
    unsigned int                            mParamcookie;

    /**
     * \brief   dateTime parameter value.
     *          The client service connection time-stamp in UTC
     **/
    DateTime                                mParamdateTime;

    /**
     * \brief   result parameter value.
     *          The connection operation result
     **/
    NEConnectionManager::eConnectionResult  mParamresult;

    /**
     * \brief   connection parameter value.
     *          The connection structure after registration.
     **/
    NEConnectionManager::sConnection        mParamconnection;

    /**
     * \brief   connectionList parameter value.
     *          The list of all servicing connections.
     **/
    NEConnectionManager::ListConnection     mParamconnectionList;

    /**
     * \brief   success parameter value.
     *          Flag, indicating whether the request to register connection succeeded or not.
     **/
    bool                                    mParamsuccess;

    /**
     * \brief   updatedList parameter value.
     *          The complete list of clients to send.
     **/
    NEConnectionManager::MapConnection      mParamupdatedList;

    /**
     * \brief   clientConnected parameter value.
     *          New client data, which contains nick name and connected date-time
     **/
    NEConnectionManager::sConnection        mParamclientConnected;

    /**
     * \brief   clientLeft parameter value.
     *          The connection data of disconnected client.
     **/
    NEConnectionManager::sConnection        mParamclientLeft;

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
    void updateData( ConnectionManagerResponseEvent & eventElem, NEConnectionManager::eMessageIDs respId );
    /**
     * \brief   Starts processing response message. The received event contains
     *          ID of appropriate message to update and contains result flag.
     *          If request calls failed (busy), the ID in event is a request call ID.
     *          In all other cases, the ID is either response call, info call,
     *          or attribute update notification call.
     *          It will also update parameters and states.
     * \param   evenElem    The response object containing message ID, result and data.
     **/
    void processResponse( ConnectionManagerResponseEvent & evenElem );

    /**
     * \brief   returns reference ConnectionManagerProxy object
     **/
    inline ConnectionManagerProxy & self( void );

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
    ConnectionManagerProxy( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ConnectionManagerProxy );
};

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerProxy class declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerProxy class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Inline operations
 ************************************************************************/

inline void ConnectionManagerProxy::setNotification( NEConnectionManager::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways /* = false */ )
{
    ProxyBase::setNotification(static_cast<unsigned int>(notifyId), &listener, notifyAlways);
}

inline void ConnectionManagerProxy::clearNotification( NEConnectionManager::eMessageIDs notifyId, IENotificationEventConsumer & listener )
{
    ProxyBase::clearNotification(static_cast<unsigned int>(notifyId), &listener);
}

inline void ConnectionManagerProxy::clearAllNotifications( IENotificationEventConsumer & listener )
{
    ProxyBase::unregisterListener( &listener );
}

inline ConnectionManagerProxy & ConnectionManagerProxy::self( void )
{
    return (*this);
}

/************************************************************************
 * Inline attributes
 ************************************************************************/

inline bool ConnectionManagerProxy::isConnectionListValid( void ) const
{
    return (getProxyData().getAttributeState( static_cast<msg_id>(NEConnectionManager::eMessageIDs::MsgId_ConnectionList) ) == NEService::eDataStateType::DataIsOK);
}

inline const NEConnectionManager::MapConnection & ConnectionManagerProxy::getConnectionList( NEService::eDataStateType & state ) const
{
    state = getProxyData().getAttributeState( static_cast<msg_id>(NEConnectionManager::eMessageIDs::MsgId_ConnectionList) );
    return mConnectionList;
}

/************************************************************************
 * Inline parameters
 ************************************************************************/

inline const String & ConnectionManagerProxy::getParamnickName( void ) const
{
    return mParamnickName;
}

inline unsigned int ConnectionManagerProxy::getParamcookie( void ) const
{
    return mParamcookie;
}

inline const DateTime & ConnectionManagerProxy::getParamdateTime( void ) const
{
    return mParamdateTime;
}

inline NEConnectionManager::eConnectionResult ConnectionManagerProxy::getParamresult( void ) const
{
    return mParamresult;
}

inline const NEConnectionManager::sConnection & ConnectionManagerProxy::getParamconnection( void ) const
{
    return mParamconnection;
}

inline const NEConnectionManager::ListConnection & ConnectionManagerProxy::getParamconnectionList( void ) const
{
    return mParamconnectionList;
}

inline bool ConnectionManagerProxy::getParamsuccess( void ) const
{
    return mParamsuccess;
}

inline const NEConnectionManager::MapConnection & ConnectionManagerProxy::getParamupdatedList( void ) const
{
    return mParamupdatedList;
}

inline const NEConnectionManager::sConnection & ConnectionManagerProxy::getParamclientConnected( void ) const
{
    return mParamclientConnected;
}

inline const NEConnectionManager::sConnection & ConnectionManagerProxy::getParamclientLeft( void ) const
{
    return mParamclientLeft;
}

#endif   // GENERATED_PRIVATE_CONNECTIONMANAGERPROXY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/ConnectionManagerProxy.hpp file
//////////////////////////////////////////////////////////////////////////
