//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/CEDirectConnectionClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_CEDIRECTCONNECTIONCLIENTBASE_HPP
#define  SHARED_GENERATED_CEDIRECTCONNECTIONCLIENTBASE_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 * Generated at     25.04.2021  23:50:45 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/CEDirectConnectionClientBase.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "shared/generated/NEDirectConnection.hpp"
#include "areg/component/CEClientBase.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/CENotificationEvent.hpp"

#include "shared/generated/private/CEDirectConnectionProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEComponent;
class CEDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionClientBase class declaration.
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
class CEDirectConnectionClientBase  : public    IEProxyListener, private CEClientBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructs and initialize DirectConnection Service Interface client object
     * \param   roleName    The role name assigned to DirectConnection servicing component object.
     * \param   ownerThread The name of component owner thread to dispatch messages.
     *                      If NULL, all messages are dispatched in current component thread.
     **/
    CEDirectConnectionClientBase( const char* roleName, const char * ownerThread = static_cast<const char *>(NULL) );

    /**
     * \brief   Constructs and initialize DirectConnection Service Interface client object.
     * \param   roleName    The role name assigned to DirectConnection servicing component object.
     * \param   ownerThread The instance of component owner thread to dispatch messages.
     **/
    CEDirectConnectionClientBase( const char* roleName, CEDispatcherThread & ownerThread );

    /**
     * \brief   Constructs and initialize DirectConnection Service Interface client object.
     * \param   roleName    The role name assigned to DirectConnection servicing component object.
     * \param   owner       The instance of client owner component. The component object should be already initialized.
     * \note    When this constructor is used, it is important that the CEComponent object is already initialized.
     *          and the component thread is set.
     **/
    CEDirectConnectionClientBase( const char* roleName, CEComponent & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEDirectConnectionClientBase( void );

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
    bool IsNotificationAssigned( NEDirectConnection::eMessageIDs msgId ) const;

    /**
     * \brief   Returns true if client object has got connection with servicing component
     **/
    bool IsConnected( void ) const;

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
    bool IsInitiatedConnectionsValid( void ) const;
    /**
     * \brief   Returns the value of InitiatedConnections attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute InitiatedConnections description: 
     *          The map of initiated connections and list of participants.
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     * \see     IsInitiatedConnectionsValid, NotifyInitiatedConnectionsUpdate, OnInitiatedConnectionsUpdate
     **/
    const NEDirectConnection::MapParticipants & GetInitiatedConnections( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on InitiatedConnections attribute update.
     *          Once notification is enabled and the data is updated, 
     *          the GetInitiatedConnections method will return valid data 
     *          Attribute InitiatedConnections description:
     *          The map of initiated connections and list of participants.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     IsInitiatedConnectionsValid, GetInitiatedConnections, OnInitiatedConnectionsUpdate
     **/
    void NotifyInitiatedConnectionsUpdate( bool notify = true );
    /**
     * \brief   Triggered, when InitiatedConnections attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes InitiatedConnections description: 
     *          The map of initiated connections and list of participants.
     * \param   InitiatedConnections    The value of InitiatedConnections attribute.
     * \param   state                   The data validation flag.
     **/
    virtual void OnInitiatedConnectionsUpdate( const NEDirectConnection::MapParticipants & InitiatedConnections, NEService::eDataStateType state );


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
     * \see     ResponseConnectoinSetup
     **/
    unsigned int RequestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
    /**
     * \brief   Overwrite to handle error of ConnectoinSetup request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestConnectoinSetupFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Request AddParticipant
 ************************************************************************/
    /**
     * \brief   Request call.
     *          The request to add a participant in the direct chat-room.
     * \param   initiator           The initiator to add to chat-room
     * \param   listParticipants    List of participants
     * \return  The sequence count number of call
     * \see     ResponseAddParticipant
     **/
    unsigned int RequestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
    /**
     * \brief   Overwrite to handle error of AddParticipant request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestAddParticipantFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Request RemoveParticipant
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to remove initiator from chat-room
     * \param   initiator           The initiator to remove from chat-room
     * \param   listParticipants    List of chat-room participants
     * \return  The sequence count number of call
     * \see     ResponseRemoveParticipant
     **/
    unsigned int RequestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
    /**
     * \brief   Overwrite to handle error of RemoveParticipant request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestRemoveParticipantFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Request CloseConnection
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to close chat-room. There is no response expected.
     * \param   initiator   The initiator to close chat-room.
     * \see     Has no response
     **/
    inline void RequestCloseConnection( const NEDirectConnection::sInitiator & initiator );
    /**
     * \brief   Overwrite to handle error of CloseConnection request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestCloseConnectionFailed( NEService::eResultType FailureReason );
    
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
     * \see     RequestConnectoinSetup
     **/
    virtual void ResponseConnectoinSetup( const bool & succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
    /**
     * \brief   Call to enable or disable receiving notifications on ConnectoinSetup response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnResponseConnectoinSetup( bool notify = true );

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
     * \see     RequestAddParticipant
     **/
    virtual void ResponseAddParticipant( const bool & succeeded, const NEDirectConnection::ListParticipants & listParticipants );
    /**
     * \brief   Call to enable or disable receiving notifications on AddParticipant response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnResponseAddParticipant( bool notify = true );

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
     * \see     RequestRemoveParticipant
     **/
    virtual void ResponseRemoveParticipant( const bool & succeeded, const NEDirectConnection::ListParticipants & listParticipants );
    /**
     * \brief   Call to enable or disable receiving notifications on RemoveParticipant response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnResponseRemoveParticipant( bool notify = true );

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
// CEDirectConnectionClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
    virtual void InvalidResponse( NEDirectConnection::eMessageIDs InvalidRespId );

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void InvalidRequest( NEDirectConnection::eMessageIDs InvalidReqId );
    
    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestFailed( NEDirectConnection::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

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
    CEDirectConnectionProxy *   mProxy;

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
    void NotifyOn( NEDirectConnection::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Returns reference of CEDirectConnectionClientBase object
     **/
    CEDirectConnectionClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEDirectConnectionClientBase( void );
    CEDirectConnectionClientBase( const CEDirectConnectionClientBase & /* src */ );
    const CEDirectConnectionClientBase & operator = ( const CEDirectConnectionClientBase & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CEDirectConnectionClientBase & CEDirectConnectionClientBase::self( void )
{   return (*this);         }

inline unsigned int CEDirectConnectionClientBase::GetCurrentSequenceNr( void ) const
{   return mCurrSequenceNr; }

inline bool CEDirectConnectionClientBase::IsConnected( void ) const
{   return mIsConnected;    }

inline bool CEDirectConnectionClientBase::IsNotificationAssigned( NEDirectConnection::eMessageIDs msgId ) const
{   return mProxy->HasNotificationListener(static_cast<unsigned int>(msgId));   }

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool CEDirectConnectionClientBase::IsInitiatedConnectionsValid( void ) const
{   return mProxy->IsInitiatedConnectionsValid( );  }

inline const NEDirectConnection::MapParticipants & CEDirectConnectionClientBase::GetInitiatedConnections( NEService::eDataStateType & state ) const
{   return mProxy->GetInitiatedConnections( state ); }

inline void CEDirectConnectionClientBase::NotifyInitiatedConnectionsUpdate( bool notify /* = true */ )
{   NotifyOn( NEDirectConnection::MSG_ID_InitiatedConnections, notify, false );  }

/************************************************************************
 * Request calls
 ************************************************************************/

inline unsigned int CEDirectConnectionClientBase::RequestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{   return mProxy->RequestConnectoinSetup( static_cast<IENotificationEventConsumer &>(self()), initiator, listParticipants );  }

inline unsigned int CEDirectConnectionClientBase::RequestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{   return mProxy->RequestAddParticipant( static_cast<IENotificationEventConsumer &>(self()), initiator, listParticipants );  }

inline unsigned int CEDirectConnectionClientBase::RequestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{   return mProxy->RequestRemoveParticipant( static_cast<IENotificationEventConsumer &>(self()), initiator, listParticipants );  }

inline void CEDirectConnectionClientBase::RequestCloseConnection( const NEDirectConnection::sInitiator & initiator )
{   mProxy->RequestCloseConnection( initiator );   }

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void CEDirectConnectionClientBase::NotifyOnResponseConnectoinSetup( bool notify /* = true */ )
{   NotifyOn(NEDirectConnection::MSG_ID_ResponseConnectoinSetup, notify, false);    }

inline void CEDirectConnectionClientBase::NotifyOnResponseAddParticipant( bool notify /* = true */ )
{   NotifyOn(NEDirectConnection::MSG_ID_ResponseAddParticipant, notify, false);    }

inline void CEDirectConnectionClientBase::NotifyOnResponseRemoveParticipant( bool notify /* = true */ )
{   NotifyOn(NEDirectConnection::MSG_ID_ResponseRemoveParticipant, notify, false);    }

#endif   // SHARED_GENERATED_CEDIRECTCONNECTIONCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/CEDirectConnectionClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
