//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/PubSubMixClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PUBSUBMIXCLIENTBASE_HPP
#define  GENERATED_SRC_PUBSUBMIXCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     18.09.2023  09:14:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PubSubMix.
 *
 * \file            generated/src/PubSubMixClientBase.hpp
 * \ingroup         PubSubMix Service Interface
 * \brief           This is an automatic generated code of PubSubMix
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEPubSubMix.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/PubSubMixProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// PubSubMixClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of PubSubMix Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              This is an interface to test the PubSub features (data-driven communication) of the
 *              AREG framework, where each service provider is a Publisher and each service consumer is a Subscriber that dynamically
 *              subscribes on data update and get notified. The agenda of this example to demonstrate mixture of Publishers and Subscribers
 *              and testing the correct behavior of notifications when Subscribers run in the same or different processes.
 **/
class PubSubMixClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize PubSubMix Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service PubSubMix role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    PubSubMixClientBase( const String & roleName, const String & ownerThread = String::EmptyString );

    /**
     * \brief   Initialize PubSubMix Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service PubSubMix role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    PubSubMixClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize PubSubMix Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service PubSubMix role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    PubSubMixClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize PubSubMix Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service PubSubMix dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    PubSubMixClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~PubSubMixClientBase( void );

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
    inline bool isNotificationAssigned( NEPubSubMix::eMessageIDs msgId ) const;

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
// PubSubMix Interface Attributes
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
     *          This string should send update notification only on change.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isStringOnChangeValid, notifyStringOnChangeUpdate, onStringOnChangeUpdate
     **/
    inline const NEPubSubMix::sString & getStringOnChange( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on StringOnChange attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getStringOnChange method will return valid data
     *          Attribute StringOnChange description:
     *          This string should send update notification only on change.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isStringOnChangeValid, getStringOnChange, onStringOnChangeUpdate
     **/
    inline void notifyOnStringOnChangeUpdate( bool notify = true );
    /**
     * \brief   Triggered, when StringOnChange attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes StringOnChange description:
     *          This string should send update notification only on change.
     * \param   StringOnChange  The value of StringOnChange attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onStringOnChangeUpdate( const NEPubSubMix::sString & StringOnChange, NEService::eDataStateType state );

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
     *          This  integer should send update notification always when set.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isIntegerAlwaysValid, notifyIntegerAlwaysUpdate, onIntegerAlwaysUpdate
     **/
    inline const NEPubSubMix::sInteger & getIntegerAlways( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on IntegerAlways attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getIntegerAlways method will return valid data
     *          Attribute IntegerAlways description:
     *          This  integer should send update notification always when set.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isIntegerAlwaysValid, getIntegerAlways, onIntegerAlwaysUpdate
     **/
    inline void notifyOnIntegerAlwaysUpdate( bool notify = true );
    /**
     * \brief   Triggered, when IntegerAlways attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes IntegerAlways description:
     *          This  integer should send update notification always when set.
     * \param   IntegerAlways   The value of IntegerAlways attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onIntegerAlwaysUpdate( const NEPubSubMix::sInteger & IntegerAlways, NEService::eDataStateType state );

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
    inline NEPubSubMix::eServiceState getServiceProviderState( NEService::eDataStateType & state ) const;
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
    virtual void onServiceProviderStateUpdate( NEPubSubMix::eServiceState ServiceProviderState, NEService::eDataStateType state );

//////////////////////////////////////////////////////////////////////////
// PubSubMix Interface Requests / Responses / Broadcasts
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
// PubSubMixClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NEPubSubMix::eMessageIDs InvalidReqId );

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
    inline const PubSubMixProxy * getProxy( void ) const;

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
    PubSubMixProxy *   mProxy;

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
// PubSubMixClientBase hidden methods
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
    void notifyOn( NEPubSubMix::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NEPubSubMix::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NEPubSubMix::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of PubSubMixClientBase object
     **/
    inline PubSubMixClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PubSubMixClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PubSubMixClientBase );
};

//////////////////////////////////////////////////////////////////////////
// PubSubMixClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline PubSubMixClientBase & PubSubMixClientBase::self( void )
{
    return (*this);
}

inline const SequenceNumber & PubSubMixClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void PubSubMixClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool PubSubMixClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool PubSubMixClientBase::isNotificationAssigned( NEPubSubMix::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & PubSubMixClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & PubSubMixClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool PubSubMixClientBase::isStringOnChangeValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isStringOnChangeValid( );
}
inline const NEPubSubMix::sString & PubSubMixClientBase::getStringOnChange( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getStringOnChange( state );
}

inline void PubSubMixClientBase::notifyOnStringOnChangeUpdate( bool notify /* = true */ )
{
    PubSubMixClientBase::notifyOn( NEPubSubMix::eMessageIDs::MsgId_StringOnChange, notify, false );
}

inline bool PubSubMixClientBase::isIntegerAlwaysValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isIntegerAlwaysValid( );
}
inline const NEPubSubMix::sInteger & PubSubMixClientBase::getIntegerAlways( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getIntegerAlways( state );
}

inline void PubSubMixClientBase::notifyOnIntegerAlwaysUpdate( bool notify /* = true */ )
{
    PubSubMixClientBase::notifyOn( NEPubSubMix::eMessageIDs::MsgId_IntegerAlways, notify, true );
}

inline bool PubSubMixClientBase::isServiceProviderStateValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isServiceProviderStateValid( );
}
inline NEPubSubMix::eServiceState PubSubMixClientBase::getServiceProviderState( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getServiceProviderState( state );
}

inline void PubSubMixClientBase::notifyOnServiceProviderStateUpdate( bool notify /* = true */ )
{
    PubSubMixClientBase::notifyOn( NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState, notify, false );
}

inline const PubSubMixProxy * PubSubMixClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & PubSubMixClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_PUBSUBMIXCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/PubSubMixClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
