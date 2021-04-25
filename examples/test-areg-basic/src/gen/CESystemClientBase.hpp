//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/CESystemClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_CESYSTEMCLIENTBASE_HPP
#define  SRC_GEN_CESYSTEMCLIENTBASE_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source System.
 * Generated at     21.04.2021  18:33:57 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/CESystemClientBase.hpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "src/gen/NESystem.hpp"
#include "areg/src/component/CEClientBase.hpp"
#include "areg/src/component/IEProxyListener.hpp"
#include "areg/src/component/CENotificationEvent.hpp"

#include "src/gen/private/CESystemProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEComponent;
class CEDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// CESystemClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of System Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              The system state.
 **/
class CESystemClientBase  : public    IEProxyListener, private CEClientBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructs and initialize System Service Interface client object
     * \param   roleName    The role name assigned to System servicing component object.
     * \param   ownerThread The name of component owner thread to dispatch messages.
     *                      If NULL, all messages are dispatched in current component thread.
     **/
    CESystemClientBase( const char* roleName, const char * ownerThread = static_cast<const char *>(NULL) );

    /**
     * \brief   Constructs and initialize System Service Interface client object.
     * \param   roleName    The role name assigned to System servicing component object.
     * \param   ownerThread The instance of component owner thread to dispatch messages.
     **/
    CESystemClientBase( const char* roleName, CEDispatcherThread & ownerThread );

    /**
     * \brief   Constructs and initialize System Service Interface client object.
     * \param   roleName    The role name assigned to System servicing component object.
     * \param   owner       The instance of client owner component. The component object should be already initialized.
     * \note    When this constructor is used, it is important that the CEComponent object is already initialized.
     *          and the component thread is set.
     **/
    CESystemClientBase( const char* roleName, CEComponent & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~CESystemClientBase( void );

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
    bool IsNotificationAssigned( NESystem::eMessageIDs msgId ) const;

    /**
     * \brief   Returns true if client object has got connection with servicing component
     **/
    bool IsConnected( void ) const;

//////////////////////////////////////////////////////////////////////////
// Start Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// System Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attribute SystemState functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of SystemState attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    bool IsSystemStateValid( void ) const;
    /**
     * \brief   Returns the value of SystemState attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute SystemState description: 
     *          The system state.
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     * \see     IsSystemStateValid, NotifySystemStateUpdate, OnSystemStateUpdate
     **/
    const NESystem::eSystemState & GetSystemState( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on SystemState attribute update.
     *          Once notification is enabled and the data is updated, 
     *          the GetSystemState method will return valid data 
     *          Attribute SystemState description:
     *          The system state.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     IsSystemStateValid, GetSystemState, OnSystemStateUpdate
     **/
    void NotifySystemStateUpdate( bool notify = true );
    /**
     * \brief   Triggered, when SystemState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes SystemState description: 
     *          The system state.
     * \param   SystemState The value of SystemState attribute.
     * \param   state       The data validation flag.
     **/
    virtual void OnSystemStateUpdate( const NESystem::eSystemState & SystemState, NEService::eDataStateType state );


//////////////////////////////////////////////////////////////////////////
// System Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request PowerOff
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Requested to power off.
     * \see     Has no response
     **/
    inline void RequestPowerOff( void );
    /**
     * \brief   Overwrite to handle error of PowerOff request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestPowerOffFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Request PowerOn
 ************************************************************************/
    /**
     * \brief   Request call.
     *          requested to power on
     * \see     Has no response
     **/
    inline void RequestPowerOn( void );
    /**
     * \brief   Overwrite to handle error of PowerOn request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestPowerOnFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Request Shutdown
 ************************************************************************/
    /**
     * \brief   Request call.
     *          requested to shutdown
     * \see     Has no response
     **/
    inline void RequestShutdown( void );
    /**
     * \brief   Overwrite to handle error of Shutdown request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestShutdownFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Request Start
 ************************************************************************/
    /**
     * \brief   Request call.
     *          requested to start.
     * \see     Has no response
     **/
    inline void RequestStart( void );
    /**
     * \brief   Overwrite to handle error of Start request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestStartFailed( NEService::eResultType FailureReason );
    
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
// CESystemClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
    virtual void InvalidResponse( NESystem::eMessageIDs InvalidRespId );

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void InvalidRequest( NESystem::eMessageIDs InvalidReqId );
    
    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestFailed( NESystem::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

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
    CESystemProxy *   mProxy;

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
    void NotifyOn( NESystem::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Returns reference of CESystemClientBase object
     **/
    CESystemClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CESystemClientBase( void );
    CESystemClientBase( const CESystemClientBase & /* src */ );
    const CESystemClientBase & operator = ( const CESystemClientBase & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CESystemClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CESystemClientBase & CESystemClientBase::self( void )
{   return (*this);         }

inline unsigned int CESystemClientBase::GetCurrentSequenceNr( void ) const
{   return mCurrSequenceNr; }

inline bool CESystemClientBase::IsConnected( void ) const
{   return mIsConnected;    }

inline bool CESystemClientBase::IsNotificationAssigned( NESystem::eMessageIDs msgId ) const
{   return mProxy->HasNotificationListener(static_cast<unsigned int>(msgId));   }

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool CESystemClientBase::IsSystemStateValid( void ) const
{   return mProxy->IsSystemStateValid( );  }

inline const NESystem::eSystemState & CESystemClientBase::GetSystemState( NEService::eDataStateType & state ) const
{   return mProxy->GetSystemState( state ); }

inline void CESystemClientBase::NotifySystemStateUpdate( bool notify /* = true */ )
{   NotifyOn( NESystem::MSG_ID_SystemState, notify, false );  }

/************************************************************************
 * Request calls
 ************************************************************************/

inline void CESystemClientBase::RequestPowerOff( void )
{   mProxy->RequestPowerOff(  );   }

inline void CESystemClientBase::RequestPowerOn( void )
{   mProxy->RequestPowerOn(  );   }

inline void CESystemClientBase::RequestShutdown( void )
{   mProxy->RequestShutdown(  );   }

inline void CESystemClientBase::RequestStart( void )
{   mProxy->RequestStart(  );   }

#endif   // SRC_GEN_CESYSTEMCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/CESystemClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
