//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/CETrafficLightClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_CETRAFFICLIGHTCLIENTBASE_HPP
#define  SRC_GEN_CETRAFFICLIGHTCLIENTBASE_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficLight.
 * Generated at     21.04.2021  18:33:59 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/CETrafficLightClientBase.hpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "src/gen/NETrafficLight.hpp"
#include "areg/src/component/CEClientBase.hpp"
#include "areg/src/component/IEProxyListener.hpp"
#include "areg/src/component/CENotificationEvent.hpp"

#include "src/gen/private/CETrafficLightProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEComponent;
class CEDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// CETrafficLightClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of TrafficLight Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              The traffic light to change lights.
 **/
class CETrafficLightClientBase  : public    IEProxyListener, private CEClientBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructs and initialize TrafficLight Service Interface client object
     * \param   roleName    The role name assigned to TrafficLight servicing component object.
     * \param   ownerThread The name of component owner thread to dispatch messages.
     *                      If NULL, all messages are dispatched in current component thread.
     **/
    CETrafficLightClientBase( const char* roleName, const char * ownerThread = static_cast<const char *>(NULL) );

    /**
     * \brief   Constructs and initialize TrafficLight Service Interface client object.
     * \param   roleName    The role name assigned to TrafficLight servicing component object.
     * \param   ownerThread The instance of component owner thread to dispatch messages.
     **/
    CETrafficLightClientBase( const char* roleName, CEDispatcherThread & ownerThread );

    /**
     * \brief   Constructs and initialize TrafficLight Service Interface client object.
     * \param   roleName    The role name assigned to TrafficLight servicing component object.
     * \param   owner       The instance of client owner component. The component object should be already initialized.
     * \note    When this constructor is used, it is important that the CEComponent object is already initialized.
     *          and the component thread is set.
     **/
    CETrafficLightClientBase( const char* roleName, CEComponent & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~CETrafficLightClientBase( void );

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
    bool IsNotificationAssigned( NETrafficLight::eMessageIDs msgId ) const;

    /**
     * \brief   Returns true if client object has got connection with servicing component
     **/
    bool IsConnected( void ) const;

//////////////////////////////////////////////////////////////////////////
// Start Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TrafficLight Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attribute TrafficLight functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of TrafficLight attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    bool IsTrafficLightValid( void ) const;
    /**
     * \brief   Returns the value of TrafficLight attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute TrafficLight description: 
     *          The current color of traffic light
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     * \see     IsTrafficLightValid, NotifyTrafficLightUpdate, OnTrafficLightUpdate
     **/
    const NETrafficLight::eLight & GetTrafficLight( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on TrafficLight attribute update.
     *          Once notification is enabled and the data is updated, 
     *          the GetTrafficLight method will return valid data 
     *          Attribute TrafficLight description:
     *          The current color of traffic light
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     IsTrafficLightValid, GetTrafficLight, OnTrafficLightUpdate
     **/
    void NotifyTrafficLightUpdate( bool notify = true );
    /**
     * \brief   Triggered, when TrafficLight attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes TrafficLight description: 
     *          The current color of traffic light
     * \param   TrafficLight    The value of TrafficLight attribute.
     * \param   state           The data validation flag.
     **/
    virtual void OnTrafficLightUpdate( const NETrafficLight::eLight & TrafficLight, NEService::eDataStateType state );


//////////////////////////////////////////////////////////////////////////
// TrafficLight Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request ChangeLight
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Requested rapidly to change the light.
     * \param   lightColor  DESCRIPTION MISSED
     * \param   holdon      If true, it will holdon the light until next request to change color. 
     *          Otherwise, the timer will trigger next color.
     * \return  The sequence count number of call
     * \see     ResponseChangeLight
     **/
    unsigned int RequestChangeLight( const NETrafficLight::eLight & lightColor, const bool & holdon );
    /**
     * \brief   Overwrite to handle error of ChangeLight request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestChangeLightFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Response ChangeLight
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Resonse to change the light color.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   lightColor  The light of trafic light.
     * \see     RequestChangeLight
     **/
    virtual void ResponseChangeLight( const NETrafficLight::eLight & lightColor );
    /**
     * \brief   Call to enable or disable receiving notifications on ChangeLight response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnResponseChangeLight( bool notify = true );

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
// CETrafficLightClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
    virtual void InvalidResponse( NETrafficLight::eMessageIDs InvalidRespId );

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void InvalidRequest( NETrafficLight::eMessageIDs InvalidReqId );
    
    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestFailed( NETrafficLight::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

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
    CETrafficLightProxy *   mProxy;

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
    void NotifyOn( NETrafficLight::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Returns reference of CETrafficLightClientBase object
     **/
    CETrafficLightClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CETrafficLightClientBase( void );
    CETrafficLightClientBase( const CETrafficLightClientBase & /* src */ );
    const CETrafficLightClientBase & operator = ( const CETrafficLightClientBase & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CETrafficLightClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CETrafficLightClientBase & CETrafficLightClientBase::self( void )
{   return (*this);         }

inline unsigned int CETrafficLightClientBase::GetCurrentSequenceNr( void ) const
{   return mCurrSequenceNr; }

inline bool CETrafficLightClientBase::IsConnected( void ) const
{   return mIsConnected;    }

inline bool CETrafficLightClientBase::IsNotificationAssigned( NETrafficLight::eMessageIDs msgId ) const
{   return mProxy->HasNotificationListener(static_cast<unsigned int>(msgId));   }

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool CETrafficLightClientBase::IsTrafficLightValid( void ) const
{   return mProxy->IsTrafficLightValid( );  }

inline const NETrafficLight::eLight & CETrafficLightClientBase::GetTrafficLight( NEService::eDataStateType & state ) const
{   return mProxy->GetTrafficLight( state ); }

inline void CETrafficLightClientBase::NotifyTrafficLightUpdate( bool notify /* = true */ )
{   NotifyOn( NETrafficLight::MSG_ID_TrafficLight, notify, false );  }

/************************************************************************
 * Request calls
 ************************************************************************/

inline unsigned int CETrafficLightClientBase::RequestChangeLight( const NETrafficLight::eLight & lightColor, const bool & holdon )
{   return mProxy->RequestChangeLight( static_cast<IENotificationEventConsumer &>(self()), lightColor, holdon );  }

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void CETrafficLightClientBase::NotifyOnResponseChangeLight( bool notify /* = true */ )
{   NotifyOn(NETrafficLight::MSG_ID_ResponseChangeLight, notify, false);    }

#endif   // SRC_GEN_CETRAFFICLIGHTCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/CETrafficLightClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
