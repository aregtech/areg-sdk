//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/TrafficControllerClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_TRAFFICCONTROLLERCLIENTBASE_HPP
#define  GENERATED_SRC_TRAFFICCONTROLLERCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:40 GMT+01:00
 *                  Create by AREG SDK code generator tool from source TrafficController.
 *
 * \file            generated/src/TrafficControllerClientBase.hpp
 * \ingroup         TrafficController Service Interface
 * \brief           This is an automatic generated code of TrafficController
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NETrafficController.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/TrafficControllerProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// TrafficControllerClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of TrafficController Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              This is a traffic light controlle service interface.
 **/
class TrafficControllerClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize TrafficController Service client object by specifying the
     *          dependency of service by role name and the client owner thread name.
     * \param   roleName    The dependent service TrafficController role name.
     * \param   ownerThread The name of client owner thread. If empty, assign current thread.
     **/
    TrafficControllerClientBase( const String & roleName, const String & ownerThread = String::getEmptyString() );

    /**
     * \brief   Initialize TrafficController Service client object by specifying the
     *          dependency of service by role name and the client owner thread.
     * \param   roleName    The dependent service TrafficController role name.
     * \param   ownerThread The instance of client owner thread.
     **/
    TrafficControllerClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize TrafficController Service client object by specifying the
     *          dependency of service by role name and the client owner component.
     * \param   roleName    The dependent service TrafficController role name.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    TrafficControllerClientBase( const String & roleName, Component & owner );
    
    /**
     * \brief   Initialize TrafficController Service client object by specifying the
     *          dependency of service by registered dependency entry and the owner component object.
     * \param   dependency  The instance of registered service TrafficController dependency entry.
     * \param   owner       The instance of client owner component.
     * \note    It is important that the Component object is already initialized.
     **/
    TrafficControllerClientBase( const NERegistry::DependencyEntry & dependency, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~TrafficControllerClientBase( void );

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
    inline bool isNotificationAssigned( NETrafficController::eMessageIDs msgId ) const;

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
// TrafficController Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Attribute TrafficSouthNorth functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of TrafficSouthNorth attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isTrafficSouthNorthValid( void ) const;
    /**
     * \brief   Returns the value of TrafficSouthNorth attribute.
     *          To get valid value, the Update Notification should be enabled.
     *          Attribute TrafficSouthNorth description:
     *          The south-north traffic.
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isTrafficSouthNorthValid, notifyTrafficSouthNorthUpdate, onTrafficSouthNorthUpdate
     **/
    inline const NETrafficController::sTrafficLight & getTrafficSouthNorth( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on TrafficSouthNorth attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getTrafficSouthNorth method will return valid data
     *          Attribute TrafficSouthNorth description:
     *          The south-north traffic.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isTrafficSouthNorthValid, getTrafficSouthNorth, onTrafficSouthNorthUpdate
     **/
    inline void notifyOnTrafficSouthNorthUpdate( bool notify = true );
    /**
     * \brief   Triggered, when TrafficSouthNorth attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes TrafficSouthNorth description:
     *          The south-north traffic.
     * \param   TrafficSouthNorth   The value of TrafficSouthNorth attribute.
     * \param   state               The data validation flag.
     **/
    virtual void onTrafficSouthNorthUpdate( const NETrafficController::sTrafficLight & TrafficSouthNorth, NEService::eDataStateType state );

/************************************************************************
 * Attribute TrafficEastWest functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of TrafficEastWest attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isTrafficEastWestValid( void ) const;
    /**
     * \brief   Returns the value of TrafficEastWest attribute.
     *          To get valid value, the Update Notification should be enabled.
     *          Attribute TrafficEastWest description:
     *          The east-west traffic
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isTrafficEastWestValid, notifyTrafficEastWestUpdate, onTrafficEastWestUpdate
     **/
    inline const NETrafficController::sTrafficLight & getTrafficEastWest( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on TrafficEastWest attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getTrafficEastWest method will return valid data
     *          Attribute TrafficEastWest description:
     *          The east-west traffic
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isTrafficEastWestValid, getTrafficEastWest, onTrafficEastWestUpdate
     **/
    inline void notifyOnTrafficEastWestUpdate( bool notify = true );
    /**
     * \brief   Triggered, when TrafficEastWest attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes TrafficEastWest description:
     *          The east-west traffic
     * \param   TrafficEastWest The value of TrafficEastWest attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onTrafficEastWestUpdate( const NETrafficController::sTrafficLight & TrafficEastWest, NEService::eDataStateType state );

//////////////////////////////////////////////////////////////////////////
// TrafficController Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Broadcast SouthNorth
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Broadcasts the light states of south-north direction.
     *          Overwrite, if need to handle Broadcast call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   LightVehicle    Light state for vehicles
     * \param   LightPedestrian Light state for pedestrians.
     **/
    virtual void broadcastSouthNorth( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian );
    /**
     * \brief   Call to enable or disable receiving notifications on SouthNorth broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnBroadcastSouthNorth( bool notify = true );

/************************************************************************
 * Broadcast EastWest
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Broadcasts the light states of east-west direction.
     *          Overwrite, if need to handle Broadcast call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   LightVehicle    Light state for vehicles
     * \param   LightPedestrian Light state for pedestrians.
     **/
    virtual void broadcastEastWest( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian );
    /**
     * \brief   Call to enable or disable receiving notifications on EastWest broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnBroadcastEastWest( bool notify = true );

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
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy ) override;

/************************************************************************/
// TrafficControllerClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NETrafficController::eMessageIDs InvalidReqId );

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
    inline unsigned int getCurrentSequenceNr( void ) const;

    /**
     * \brief  Returns instance of proxy object.
     */
    inline const TrafficControllerProxy * getProxy( void ) const;

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
    unsigned int        mCurrSequenceNr;
    /**
     * \brief   Pointer of Proxy object providing communication
     **/
    TrafficControllerProxy *   mProxy;

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
// TrafficControllerClientBase hidden methods
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
    void notifyOn( NETrafficController::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NETrafficController::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NETrafficController::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of TrafficControllerClientBase object
     **/
    inline TrafficControllerClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TrafficControllerClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( TrafficControllerClientBase );
};

//////////////////////////////////////////////////////////////////////////
// TrafficControllerClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline TrafficControllerClientBase & TrafficControllerClientBase::self( void )
{
    return (*this);
}

inline unsigned int TrafficControllerClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void TrafficControllerClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool TrafficControllerClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool TrafficControllerClientBase::isNotificationAssigned( NETrafficController::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & TrafficControllerClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & TrafficControllerClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool TrafficControllerClientBase::isTrafficSouthNorthValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isTrafficSouthNorthValid( );
}
inline const NETrafficController::sTrafficLight & TrafficControllerClientBase::getTrafficSouthNorth( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getTrafficSouthNorth( state );
}

inline void TrafficControllerClientBase::notifyOnTrafficSouthNorthUpdate( bool notify /* = true */ )
{
    TrafficControllerClientBase::notifyOn( NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth, notify, false );
}

inline bool TrafficControllerClientBase::isTrafficEastWestValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isTrafficEastWestValid( );
}
inline const NETrafficController::sTrafficLight & TrafficControllerClientBase::getTrafficEastWest( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getTrafficEastWest( state );
}

inline void TrafficControllerClientBase::notifyOnTrafficEastWestUpdate( bool notify /* = true */ )
{
    TrafficControllerClientBase::notifyOn( NETrafficController::eMessageIDs::MsgId_TrafficEastWest, notify, false );
}

/************************************************************************
 * Broadcast notifications
 ************************************************************************/

inline void TrafficControllerClientBase::notifyOnBroadcastSouthNorth( bool notify /* = true */ )
{
    TrafficControllerClientBase::notifyOn(NETrafficController::eMessageIDs::MsgId_broadcastSouthNorth, notify, false);
}

inline void TrafficControllerClientBase::notifyOnBroadcastEastWest( bool notify /* = true */ )
{
    TrafficControllerClientBase::notifyOn(NETrafficController::eMessageIDs::MsgId_broadcastEastWest, notify, false);
}

inline const TrafficControllerProxy * TrafficControllerClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & TrafficControllerClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_TRAFFICCONTROLLERCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/TrafficControllerClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
