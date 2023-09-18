//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/SystemShutdownClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_SYSTEMSHUTDOWNCLIENTBASE_HPP
#define  GENERATED_SRC_SYSTEMSHUTDOWNCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     18.09.2023  09:14:51 GMT+02:00
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 *
 * \file            generated/src/SystemShutdownClientBase.hpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NESystemShutdown.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/SystemShutdownProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// SystemShutdownClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of SystemShutdown Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 *              This service demonstrates the usage of attribute.
 **/
class SystemShutdownClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize SystemShutdown Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service SystemShutdown role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    SystemShutdownClientBase( const String & roleName, const String & ownerThread = String::EmptyString );

    /**
     * \brief   Initialize SystemShutdown Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service SystemShutdown role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    SystemShutdownClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize SystemShutdown Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service SystemShutdown role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    SystemShutdownClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize SystemShutdown Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service SystemShutdown dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    SystemShutdownClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~SystemShutdownClientBase( void );

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
    inline bool isNotificationAssigned( NESystemShutdown::eMessageIDs msgId ) const;

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
// SystemShutdown Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Attribute ServiceState functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of ServiceState attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isServiceStateValid( void ) const;
    /**
     * \brief   Returns the value of ServiceState attribute.
     *          To get valid value, the Update Notification should be enabled.
     *          Attribute ServiceState description:
     *          Describes the system availability state
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isServiceStateValid, notifyServiceStateUpdate, onServiceStateUpdate
     **/
    inline NESystemShutdown::eServiceState getServiceState( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on ServiceState attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getServiceState method will return valid data
     *          Attribute ServiceState description:
     *          Describes the system availability state
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isServiceStateValid, getServiceState, onServiceStateUpdate
     **/
    inline void notifyOnServiceStateUpdate( bool notify = true );
    /**
     * \brief   Triggered, when ServiceState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes ServiceState description:
     *          Describes the system availability state
     * \param   ServiceState    The value of ServiceState attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onServiceStateUpdate( NESystemShutdown::eServiceState ServiceState, NEService::eDataStateType state );

//////////////////////////////////////////////////////////////////////////
// SystemShutdown Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request SystemShutdown
 ************************************************************************/
    /**
     * \brief   Request call.
     *          The request to shutdown the system.
     * \see     Has no response
     **/
    inline void requestSystemShutdown( void );
    /**
     * \brief   Overwrite to handle error of SystemShutdown request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestSystemShutdownFailed( NEService::eResultType FailureReason );

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
// SystemShutdownClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NESystemShutdown::eMessageIDs InvalidReqId );

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
    inline const SystemShutdownProxy * getProxy( void ) const;

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
    SystemShutdownProxy *   mProxy;

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
// SystemShutdownClientBase hidden methods
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
    void notifyOn( NESystemShutdown::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NESystemShutdown::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NESystemShutdown::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of SystemShutdownClientBase object
     **/
    inline SystemShutdownClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SystemShutdownClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SystemShutdownClientBase );
};

//////////////////////////////////////////////////////////////////////////
// SystemShutdownClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline SystemShutdownClientBase & SystemShutdownClientBase::self( void )
{
    return (*this);
}

inline const SequenceNumber & SystemShutdownClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void SystemShutdownClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool SystemShutdownClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool SystemShutdownClientBase::isNotificationAssigned( NESystemShutdown::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & SystemShutdownClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & SystemShutdownClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool SystemShutdownClientBase::isServiceStateValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isServiceStateValid( );
}
inline NESystemShutdown::eServiceState SystemShutdownClientBase::getServiceState( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getServiceState( state );
}

inline void SystemShutdownClientBase::notifyOnServiceStateUpdate( bool notify /* = true */ )
{
    SystemShutdownClientBase::notifyOn( NESystemShutdown::eMessageIDs::MsgId_ServiceState, notify, false );
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline void SystemShutdownClientBase::requestSystemShutdown( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestSystemShutdown(  );
}

inline const SystemShutdownProxy * SystemShutdownClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & SystemShutdownClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_SYSTEMSHUTDOWNCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/SystemShutdownClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
