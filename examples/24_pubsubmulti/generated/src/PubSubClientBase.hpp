//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/PubSubClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PUBSUBCLIENTBASE_HPP
#define  GENERATED_SRC_PUBSUBCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     15.11.2023  14:52:23 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PubSub.
 *
 * \file            generated/src/PubSubClientBase.hpp
 * \ingroup         PubSub Service Interface
 * \brief           This is an automatic generated code of PubSub
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEPubSub.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/PubSubProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// PubSubClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of PubSub Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              This is an interface to test the PubSub features (data-driven communication) of the
 *              AREG framework, where service provider is a Publisher and service consumer is a Subscriber that dynamically
 *              subscribes on data update and get notified.
 **/
class PubSubClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize PubSub Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service PubSub role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    PubSubClientBase( const String & roleName, const String & ownerThread = String::EmptyString );

    /**
     * \brief   Initialize PubSub Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service PubSub role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    PubSubClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize PubSub Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service PubSub role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    PubSubClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize PubSub Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service PubSub dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    PubSubClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~PubSubClientBase( void );

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
    inline bool isNotificationAssigned( NEPubSub::eMessageIDs msgId ) const;

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
// PubSub Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Attribute StringOnChange functions
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
     *          This simple string should send update notification only on value change.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isStringOnChangeValid, notifyStringOnChangeUpdate, onStringOnChangeUpdate
     **/
    inline const String & getStringOnChange( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on StringOnChange attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getStringOnChange method will return valid data
     *          Attribute StringOnChange description:
     *          This simple string should send update notification only on value change.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isStringOnChangeValid, getStringOnChange, onStringOnChangeUpdate
     **/
    inline void notifyOnStringOnChangeUpdate( bool notify = true );
    /**
     * \brief   Triggered, when StringOnChange attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes StringOnChange description:
     *          This simple string should send update notification only on value change.
     * \param   StringOnChange  The value of StringOnChange attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onStringOnChangeUpdate( const String & StringOnChange, NEService::eDataStateType state );

/************************************************************************
 * Attribute IntegerAlways functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of IntegerAlways attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isIntegerAlwaysValid( void ) const;
    /**
     * \brief   Returns the value of IntegerAlways attribute.
     *          To get valid value, the Update Notification should be enabled.
     *          Attribute IntegerAlways description:
     *          This simple integer should send update notification always when value is set.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isIntegerAlwaysValid, notifyIntegerAlwaysUpdate, onIntegerAlwaysUpdate
     **/
    inline unsigned int getIntegerAlways( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on IntegerAlways attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getIntegerAlways method will return valid data
     *          Attribute IntegerAlways description:
     *          This simple integer should send update notification always when value is set.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isIntegerAlwaysValid, getIntegerAlways, onIntegerAlwaysUpdate
     **/
    inline void notifyOnIntegerAlwaysUpdate( bool notify = true );
    /**
     * \brief   Triggered, when IntegerAlways attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes IntegerAlways description:
     *          This simple integer should send update notification always when value is set.
     * \param   IntegerAlways   The value of IntegerAlways attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onIntegerAlwaysUpdate( unsigned int IntegerAlways, NEService::eDataStateType state );

/************************************************************************
 * Attribute ServiceProviderState functions
 ************************************************************************/
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
     * \see     isServiceProviderStateValid, notifyServiceProviderStateUpdate, onServiceProviderStateUpdate
     **/
    inline NEPubSub::eServiceState getServiceProviderState( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on ServiceProviderState attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getServiceProviderState method will return valid data
     *          Attribute ServiceProviderState description:
     *          The state of service provider. The value is always valid, updated on change.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isServiceProviderStateValid, getServiceProviderState, onServiceProviderStateUpdate
     **/
    inline void notifyOnServiceProviderStateUpdate( bool notify = true );
    /**
     * \brief   Triggered, when ServiceProviderState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes ServiceProviderState description:
     *          The state of service provider. The value is always valid, updated on change.
     * \param   ServiceProviderState    The value of ServiceProviderState attribute.
     * \param   state                   The data validation flag.
     **/
    virtual void onServiceProviderStateUpdate( NEPubSub::eServiceState ServiceProviderState, NEService::eDataStateType state );

//////////////////////////////////////////////////////////////////////////
// PubSub Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

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
// PubSubClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NEPubSub::eMessageIDs InvalidReqId );

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
    inline const PubSubProxy * getProxy( void ) const;

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
    PubSubProxy *   mProxy;

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
// PubSubClientBase hidden methods
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
    void notifyOn( NEPubSub::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NEPubSub::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NEPubSub::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of PubSubClientBase object
     **/
    inline PubSubClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PubSubClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PubSubClientBase );
};

//////////////////////////////////////////////////////////////////////////
// PubSubClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline PubSubClientBase & PubSubClientBase::self( void )
{
    return (*this);
}

inline const SequenceNumber & PubSubClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void PubSubClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool PubSubClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool PubSubClientBase::isNotificationAssigned( NEPubSub::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & PubSubClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & PubSubClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool PubSubClientBase::isStringOnChangeValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isStringOnChangeValid( );
}
inline const String & PubSubClientBase::getStringOnChange( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getStringOnChange( state );
}

inline void PubSubClientBase::notifyOnStringOnChangeUpdate( bool notify /* = true */ )
{
    PubSubClientBase::notifyOn( NEPubSub::eMessageIDs::MsgId_StringOnChange, notify, false );
}

inline bool PubSubClientBase::isIntegerAlwaysValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isIntegerAlwaysValid( );
}
inline unsigned int PubSubClientBase::getIntegerAlways( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getIntegerAlways( state );
}

inline void PubSubClientBase::notifyOnIntegerAlwaysUpdate( bool notify /* = true */ )
{
    PubSubClientBase::notifyOn( NEPubSub::eMessageIDs::MsgId_IntegerAlways, notify, true );
}

inline bool PubSubClientBase::isServiceProviderStateValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isServiceProviderStateValid( );
}
inline NEPubSub::eServiceState PubSubClientBase::getServiceProviderState( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getServiceProviderState( state );
}

inline void PubSubClientBase::notifyOnServiceProviderStateUpdate( bool notify /* = true */ )
{
    PubSubClientBase::notifyOn( NEPubSub::eMessageIDs::MsgId_ServiceProviderState, notify, false );
}

inline const PubSubProxy * PubSubClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & PubSubClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_PUBSUBCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/PubSubClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
