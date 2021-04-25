//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/CEMessageOutputClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_CEMESSAGEOUTPUTCLIENTBASE_HPP
#define  SRC_GEN_CEMESSAGEOUTPUTCLIENTBASE_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source MessageOutput.
 * Generated at     21.04.2021  18:33:55 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/CEMessageOutputClientBase.hpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "src/gen/NEMessageOutput.hpp"
#include "areg/src/component/CEClientBase.hpp"
#include "areg/src/component/IEProxyListener.hpp"
#include "areg/src/component/CENotificationEvent.hpp"

#include "src/gen/private/CEMessageOutputProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEComponent;
class CEDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of MessageOutput Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              The message output service to interact with user.
 **/
class CEMessageOutputClientBase  : public    IEProxyListener, private CEClientBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructs and initialize MessageOutput Service Interface client object
     * \param   roleName    The role name assigned to MessageOutput servicing component object.
     * \param   ownerThread The name of component owner thread to dispatch messages.
     *                      If NULL, all messages are dispatched in current component thread.
     **/
    CEMessageOutputClientBase( const char* roleName, const char * ownerThread = static_cast<const char *>(NULL) );

    /**
     * \brief   Constructs and initialize MessageOutput Service Interface client object.
     * \param   roleName    The role name assigned to MessageOutput servicing component object.
     * \param   ownerThread The instance of component owner thread to dispatch messages.
     **/
    CEMessageOutputClientBase( const char* roleName, CEDispatcherThread & ownerThread );

    /**
     * \brief   Constructs and initialize MessageOutput Service Interface client object.
     * \param   roleName    The role name assigned to MessageOutput servicing component object.
     * \param   owner       The instance of client owner component. The component object should be already initialized.
     * \note    When this constructor is used, it is important that the CEComponent object is already initialized.
     *          and the component thread is set.
     **/
    CEMessageOutputClientBase( const char* roleName, CEComponent & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEMessageOutputClientBase( void );

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
    bool IsNotificationAssigned( NEMessageOutput::eMessageIDs msgId ) const;

    /**
     * \brief   Returns true if client object has got connection with servicing component
     **/
    bool IsConnected( void ) const;

//////////////////////////////////////////////////////////////////////////
// Start Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MessageOutput Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attribute LastMessage functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of LastMessage attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    bool IsLastMessageValid( void ) const;
    /**
     * \brief   Returns the value of LastMessage attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute LastMessage description: 
     *          The last message to output on console.
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     * \see     IsLastMessageValid, NotifyLastMessageUpdate, OnLastMessageUpdate
     **/
    const CEString & GetLastMessage( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on LastMessage attribute update.
     *          Once notification is enabled and the data is updated, 
     *          the GetLastMessage method will return valid data 
     *          Attribute LastMessage description:
     *          The last message to output on console.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     IsLastMessageValid, GetLastMessage, OnLastMessageUpdate
     **/
    void NotifyLastMessageUpdate( bool notify = true );
    /**
     * \brief   Triggered, when LastMessage attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes LastMessage description: 
     *          The last message to output on console.
     * \param   LastMessage The value of LastMessage attribute.
     * \param   state       The data validation flag.
     **/
    virtual void OnLastMessageUpdate( const CEString & LastMessage, NEService::eDataStateType state );


//////////////////////////////////////////////////////////////////////////
// MessageOutput Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request OutputMessage
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Sent to service to output message on console.
     * \param   msgOutput   The message requested to output on console.
     * \param   dateTime    The date and the time when it was requested.
     * \see     Has no response
     **/
    inline void RequestOutputMessage( const CEString & msgOutput, const CEDateTime & dateTime );
    /**
     * \brief   Overwrite to handle error of OutputMessage request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestOutputMessageFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Broadcast ActionRequest
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Informs the action requested by a user
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   userAction  The action requested by user.
     **/
    virtual void BroadcastActionRequest( const NEMessageOutput::eRequestedActons & userAction );
    /**
     * \brief   Call to enable or disable receiving notifications on ActionRequest broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnBroadcastActionRequest( bool notify = true );

/************************************************************************
 * Broadcast InputMessage
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          The input message from the user.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   msgInput    The input message from the user.
     **/
    virtual void BroadcastInputMessage( const CEString & msgInput );
    /**
     * \brief   Call to enable or disable receiving notifications on InputMessage broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnBroadcastInputMessage( bool notify = true );

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
// CEMessageOutputClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
    virtual void InvalidResponse( NEMessageOutput::eMessageIDs InvalidRespId );

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void InvalidRequest( NEMessageOutput::eMessageIDs InvalidReqId );
    
    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestFailed( NEMessageOutput::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

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
    CEMessageOutputProxy *   mProxy;

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
    void NotifyOn( NEMessageOutput::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Returns reference of CEMessageOutputClientBase object
     **/
    CEMessageOutputClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEMessageOutputClientBase( void );
    CEMessageOutputClientBase( const CEMessageOutputClientBase & /* src */ );
    const CEMessageOutputClientBase & operator = ( const CEMessageOutputClientBase & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CEMessageOutputClientBase & CEMessageOutputClientBase::self( void )
{   return (*this);         }

inline unsigned int CEMessageOutputClientBase::GetCurrentSequenceNr( void ) const
{   return mCurrSequenceNr; }

inline bool CEMessageOutputClientBase::IsConnected( void ) const
{   return mIsConnected;    }

inline bool CEMessageOutputClientBase::IsNotificationAssigned( NEMessageOutput::eMessageIDs msgId ) const
{   return mProxy->HasNotificationListener(static_cast<unsigned int>(msgId));   }

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool CEMessageOutputClientBase::IsLastMessageValid( void ) const
{   return mProxy->IsLastMessageValid( );  }

inline const CEString & CEMessageOutputClientBase::GetLastMessage( NEService::eDataStateType & state ) const
{   return mProxy->GetLastMessage( state ); }

inline void CEMessageOutputClientBase::NotifyLastMessageUpdate( bool notify /* = true */ )
{   NotifyOn( NEMessageOutput::MSG_ID_LastMessage, notify, false );  }

/************************************************************************
 * Request calls
 ************************************************************************/

inline void CEMessageOutputClientBase::RequestOutputMessage( const CEString & msgOutput, const CEDateTime & dateTime )
{   mProxy->RequestOutputMessage( msgOutput, dateTime );   }

/************************************************************************
 * Broadcast notifications
 ************************************************************************/

inline void CEMessageOutputClientBase::NotifyOnBroadcastActionRequest( bool notify /* = true */ )
{   NotifyOn(NEMessageOutput::MSG_ID_BroadcastActionRequest, notify, false);    }

inline void CEMessageOutputClientBase::NotifyOnBroadcastInputMessage( bool notify /* = true */ )
{   NotifyOn(NEMessageOutput::MSG_ID_BroadcastInputMessage, notify, false);    }

#endif   // SRC_GEN_CEMESSAGEOUTPUTCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/CEMessageOutputClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
