//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/CEConnectionManagerClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_CECONNECTIONMANAGERCLIENTBASE_HPP
#define  SHARED_GENERATED_CECONNECTIONMANAGERCLIENTBASE_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 * Generated at     25.04.2021  23:50:44 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/CEConnectionManagerClientBase.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "shared/generated/NEConnectionManager.hpp"
#include "areg/src/component/CEClientBase.hpp"
#include "areg/src/component/IEProxyListener.hpp"
#include "areg/src/component/CENotificationEvent.hpp"

#include "shared/generated/private/CEConnectionManagerProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEComponent;
class CEDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of ConnectionManager Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              The connection manager to register each application with client
 **/
class CEConnectionManagerClientBase  : public    IEProxyListener, private CEClientBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructs and initialize ConnectionManager Service Interface client object
     * \param   roleName    The role name assigned to ConnectionManager servicing component object.
     * \param   ownerThread The name of component owner thread to dispatch messages.
     *                      If NULL, all messages are dispatched in current component thread.
     **/
    CEConnectionManagerClientBase( const char* roleName, const char * ownerThread = static_cast<const char *>(NULL) );

    /**
     * \brief   Constructs and initialize ConnectionManager Service Interface client object.
     * \param   roleName    The role name assigned to ConnectionManager servicing component object.
     * \param   ownerThread The instance of component owner thread to dispatch messages.
     **/
    CEConnectionManagerClientBase( const char* roleName, CEDispatcherThread & ownerThread );

    /**
     * \brief   Constructs and initialize ConnectionManager Service Interface client object.
     * \param   roleName    The role name assigned to ConnectionManager servicing component object.
     * \param   owner       The instance of client owner component. The component object should be already initialized.
     * \note    When this constructor is used, it is important that the CEComponent object is already initialized.
     *          and the component thread is set.
     **/
    CEConnectionManagerClientBase( const char* roleName, CEComponent & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEConnectionManagerClientBase( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Clears all notifications, stops receiving notifications from server
     **/
    void ClearAllNotifications( void );

    /**
     * \brief   Returns true if the specified certain notification is already assigned.
     *          Otherwise returns false.
     * \param   msgId   The ID of message to check.
     **/
    bool IsNotificationAssigned( NEConnectionManager::eMessageIDs msgId ) const;

    /**
     * \brief   Returns true if client object has got connection with servicing component
     **/
    bool IsConnected( void ) const;

//////////////////////////////////////////////////////////////////////////
// Start Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attribute ConnectionList functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of ConnectionList attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    bool IsConnectionListValid( void ) const;
    /**
     * \brief   Returns the value of ConnectionList attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute ConnectionList description: 
     *          The map of all active connections
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     * \see     IsConnectionListValid, NotifyConnectionListUpdate, OnConnectionListUpdate
     **/
    const NEConnectionManager::MapConnection & GetConnectionList( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on ConnectionList attribute update.
     *          Once notification is enabled and the data is updated, 
     *          the GetConnectionList method will return valid data 
     *          Attribute ConnectionList description:
     *          The map of all active connections
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     IsConnectionListValid, GetConnectionList, OnConnectionListUpdate
     **/
    void NotifyConnectionListUpdate( bool notify = true );
    /**
     * \brief   Triggered, when ConnectionList attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes ConnectionList description: 
     *          The map of all active connections
     * \param   ConnectionList  The value of ConnectionList attribute.
     * \param   state           The data validation flag.
     **/
    virtual void OnConnectionListUpdate( const NEConnectionManager::MapConnection & ConnectionList, NEService::eDataStateType state );


//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request Connet
 ************************************************************************/
    /**
     * \brief   Request call.
     *          The request to connect to system
     * \param   nickName    The nick name of connecting client service
     * \param   dateTime    The connecting client connection timestamp in UTC
     * \return  The sequence count number of call
     * \see     ResponseConnect
     **/
    unsigned int RequestConnet( const CEString & nickName, const CEDateTime & dateTime );
    /**
     * \brief   Overwrite to handle error of Connet request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestConnetFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Request RegisterConnection
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to register direct connection service.
     * \param   nickName        The nick name of connection used in service
     * \param   cookie          The cookie value to set
     * \param   connectCookie   The cookie to use for registration. 
     * \param   dateRegister    The connection registration timestamp
     * \return  The sequence count number of call
     * \see     ResponseRegisterConnection
     **/
    unsigned int RequestRegisterConnection( const CEString & nickName, const unsigned int & cookie, const unsigned int & connectCookie, const CEDateTime & dateRegister );
    /**
     * \brief   Overwrite to handle error of RegisterConnection request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestRegisterConnectionFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Request Diconnect
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to disconnect.
     * \param   nickName    The nick name of client connection service
     * \param   cookie      The cookie given by connection manager. If invalid, it will search by nickName parameter
     * \param   dateTime    Disconnect request date-time
     * \see     Has no response
     **/
    inline void RequestDiconnect( const CEString & nickName, const unsigned int & cookie, const CEDateTime & dateTime );
    /**
     * \brief   Overwrite to handle error of Diconnect request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestDiconnectFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Response Connect
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          The result of connection request.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   nickName    Client connection data, which contains nick name and connection date-time
     * \param   cookie      Connection assigned cookie for client service
     * \param   dateTime    The client service connection timestamp in UTC
     * \param   result      The connection operation result
     * \see     RequestConnet
     **/
    virtual void ResponseConnect( const CEString & nickName, const unsigned int & cookie, const CEDateTime & dateTime, const NEConnectionManager::eConnectionResult & result );
    /**
     * \brief   Call to enable or disable receiving notifications on Connect response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnResponseConnect( bool notify = true );

/************************************************************************
 * Response RegisterConnection
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          The result of request to register direct connection service.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   connection      The connection structure after registration.
     * \param   connectionList  The list of all servicing connections.
     * \param   success         Flag, indicating whether the request to register connection succeeded or not.
     * \see     RequestRegisterConnection
     **/
    virtual void ResponseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, const bool & success );
    /**
     * \brief   Call to enable or disable receiving notifications on RegisterConnection response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnResponseRegisterConnection( bool notify = true );

/************************************************************************
 * Broadcast ConnectionUpdated
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Notifies all client components the connection has been updated.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   updatedList The complete list of clients to send.
     **/
    virtual void BroadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList );
    /**
     * \brief   Call to enable or disable receiving notifications on ConnectionUpdated broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnBroadcastConnectionUpdated( bool notify = true );

/************************************************************************
 * Broadcast ClientConnected
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Notifies that a new client has been connected.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientConnected New client data, which contains nick name and connected date-time
     **/
    virtual void BroadcastClientConnected( const NEConnectionManager::sConnection & clientConnected );
    /**
     * \brief   Call to enable or disable receiving notifications on ClientConnected broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnBroadcastClientConnected( bool notify = true );

/************************************************************************
 * Broadcast ClientDisconnected
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Notifies a client has been disconnected.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientLeft  The connection data of disconnected client.
     **/
    virtual void BroadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft );
    /**
     * \brief   Call to enable or disable receiving notifications on ClientDisconnected broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnBroadcastClientDisconnected( bool notify = true );

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
    virtual bool ServiceConnected( bool isConnected, CEProxyBase & proxy );

protected:
/************************************************************************/
// CEConnectionManagerClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
    virtual void InvalidResponse( NEConnectionManager::eMessageIDs InvalidRespId );

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void InvalidRequest( NEConnectionManager::eMessageIDs InvalidReqId );
    
    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestFailed( NEConnectionManager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Returns the current sequence number
     **/
    unsigned int GetCurrentSequenceNr( void ) const;

    /**
     * \brief   Call to recreate Proxy for the client. This call will remove and unregister all existing notifications
     *          and the client will not receive pending update or response notifications. 
     *          The client first will receive disconnect message, then again connect.
     *          Reset update notifications manually in connect if need.          
     *          This function call can be used when the client instance should be registered and run in new thread.
     *
     * \return  Returns true if Proxy was created with success.
     **/
     bool RecreateProxy( void );

     /**
      * \brief  Returns pointer to client dispatcher thread where the messages are dispatched.
      *         The function can return NULL if Proxy was not instantiated yet.
      **/
     CEDispatcherThread * GetDispatcherThread( void );

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
    CEConnectionManagerProxy *   mProxy;

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
    virtual void ProcessNotificationEvent( CENotificationEvent & eventElem );
    
private:
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
    void NotifyOn( NEConnectionManager::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Returns reference of CEConnectionManagerClientBase object
     **/
    CEConnectionManagerClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEConnectionManagerClientBase( void );
    CEConnectionManagerClientBase( const CEConnectionManagerClientBase & /* src */ );
    const CEConnectionManagerClientBase & operator = ( const CEConnectionManagerClientBase & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CEConnectionManagerClientBase & CEConnectionManagerClientBase::self( void )
{   return (*this);         }

inline unsigned int CEConnectionManagerClientBase::GetCurrentSequenceNr( void ) const
{   return mCurrSequenceNr; }

inline bool CEConnectionManagerClientBase::IsConnected( void ) const
{   return mIsConnected;    }

inline bool CEConnectionManagerClientBase::IsNotificationAssigned( NEConnectionManager::eMessageIDs msgId ) const
{   return mProxy->HasNotificationListener(static_cast<unsigned int>(msgId));   }

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool CEConnectionManagerClientBase::IsConnectionListValid( void ) const
{   return mProxy->IsConnectionListValid( );  }

inline const NEConnectionManager::MapConnection & CEConnectionManagerClientBase::GetConnectionList( NEService::eDataStateType & state ) const
{   return mProxy->GetConnectionList( state ); }

inline void CEConnectionManagerClientBase::NotifyConnectionListUpdate( bool notify /* = true */ )
{   NotifyOn( NEConnectionManager::MSG_ID_ConnectionList, notify, false );  }

/************************************************************************
 * Request calls
 ************************************************************************/

inline unsigned int CEConnectionManagerClientBase::RequestConnet( const CEString & nickName, const CEDateTime & dateTime )
{   return mProxy->RequestConnet( static_cast<IENotificationEventConsumer &>(self()), nickName, dateTime );  }

inline unsigned int CEConnectionManagerClientBase::RequestRegisterConnection( const CEString & nickName, const unsigned int & cookie, const unsigned int & connectCookie, const CEDateTime & dateRegister )
{   return mProxy->RequestRegisterConnection( static_cast<IENotificationEventConsumer &>(self()), nickName, cookie, connectCookie, dateRegister );  }

inline void CEConnectionManagerClientBase::RequestDiconnect( const CEString & nickName, const unsigned int & cookie, const CEDateTime & dateTime )
{   mProxy->RequestDiconnect( nickName, cookie, dateTime );   }

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void CEConnectionManagerClientBase::NotifyOnResponseConnect( bool notify /* = true */ )
{   NotifyOn(NEConnectionManager::MSG_ID_ResponseConnect, notify, false);    }

inline void CEConnectionManagerClientBase::NotifyOnResponseRegisterConnection( bool notify /* = true */ )
{   NotifyOn(NEConnectionManager::MSG_ID_ResponseRegisterConnection, notify, false);    }

/************************************************************************
 * Broadcast notifications
 ************************************************************************/

inline void CEConnectionManagerClientBase::NotifyOnBroadcastConnectionUpdated( bool notify /* = true */ )
{   NotifyOn(NEConnectionManager::MSG_ID_BroadcastConnectionUpdated, notify, false);    }

inline void CEConnectionManagerClientBase::NotifyOnBroadcastClientConnected( bool notify /* = true */ )
{   NotifyOn(NEConnectionManager::MSG_ID_BroadcastClientConnected, notify, false);    }

inline void CEConnectionManagerClientBase::NotifyOnBroadcastClientDisconnected( bool notify /* = true */ )
{   NotifyOn(NEConnectionManager::MSG_ID_BroadcastClientDisconnected, notify, false);    }

#endif   // SHARED_GENERATED_CECONNECTIONMANAGERCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/CEConnectionManagerClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
