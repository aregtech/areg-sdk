//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/LocalHelloWorldClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHAREIPCMIX_SRC_LOCALHELLOWORLDCLIENTBASE_HPP
#define  SHAREIPCMIX_SRC_LOCALHELLOWORLDCLIENTBASE_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 * Generated at     04.07.2021  04:21:23 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/LocalHelloWorldClientBase.hpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "shareipcmix/src/NELocalHelloWorld.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "shareipcmix/src/private/LocalHelloWorldProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of LocalHelloWorld Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 *              This interface serves only local components and can be duplicated in other processes.
 *              		
 **/
class LocalHelloWorldClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize LocalHelloWorld Service Interface client object. 
     *          Specifies used service and owner thread.
     * \param   roleName    The role name assigned to LocalHelloWorld servicing component object.
     * \param   ownerThread The name of component owner thread to dispatch messages.
     *                      If NULL, all messages are dispatched in current component thread.
     **/
    LocalHelloWorldClientBase( const char* roleName, const char * ownerThread = static_cast<const char *>(NULL) );

    /**
     * \brief   Initialize LocalHelloWorld Service Interface client object.
     *          Specifies used service and owner thread.
     * \param   roleName    The role name assigned to LocalHelloWorld servicing component object.
     * \param   ownerThread The instance of component owner thread to dispatch messages.
     **/
    LocalHelloWorldClientBase( const char* roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize LocalHelloWorld Service Interface client object.
     *          Specifies used service and owner component.
     * \param   roleName    The role name assigned to LocalHelloWorld servicing component object.
     * \param   owner       The instance of client owner component. The component object should be already initialized.
     * \note    When this constructor is used, it is important that the Component object is already initialized.
     *          and the component thread is set.
     **/
    LocalHelloWorldClientBase( const char* roleName, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~LocalHelloWorldClientBase( void );

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
    inline bool isNotificationAssigned( NELocalHelloWorld::eMessageIDs msgId ) const;

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
// LocalHelloWorld Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attribute ConnectedClients functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of ConnectedClients attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isConnectedClientsValid( void ) const;
    /**
     * \brief   Returns the value of ConnectedClients attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute ConnectedClients description: 
     *          The list of connected clients. Updated each time when new client requests to output Hello World message.
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     * \see     isConnectedClientsValid, notifyConnectedClientsUpdate, onConnectedClientsUpdate
     **/
    inline const NELocalHelloWorld::ConnectionList & getConnectedClients( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on ConnectedClients attribute update.
     *          Once notification is enabled and the data is updated, 
     *          the getConnectedClients method will return valid data 
     *          Attribute ConnectedClients description:
     *          The list of connected clients. Updated each time when new client requests to output Hello World message.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isConnectedClientsValid, getConnectedClients, onConnectedClientsUpdate
     **/
    inline void notifyOnConnectedClientsUpdate( bool notify = true );
    /**
     * \brief   Triggered, when ConnectedClients attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes ConnectedClients description: 
     *          The list of connected clients. Updated each time when new client requests to output Hello World message.
     * \param   ConnectedClients    The value of ConnectedClients attribute.
     * \param   state               The data validation flag.
     **/
    virtual void onConnectedClientsUpdate( const NELocalHelloWorld::ConnectionList & ConnectedClients, NEService::eDataStateType state );

/************************************************************************
 * Attribute RemainOutput functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of RemainOutput attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isRemainOutputValid( void ) const;
    /**
     * \brief   Returns the value of RemainOutput attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute RemainOutput description: 
     *          Remaining number of outputs to print Hello World.
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     * \see     isRemainOutputValid, notifyRemainOutputUpdate, onRemainOutputUpdate
     **/
    inline short getRemainOutput( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on RemainOutput attribute update.
     *          Once notification is enabled and the data is updated, 
     *          the getRemainOutput method will return valid data 
     *          Attribute RemainOutput description:
     *          Remaining number of outputs to print Hello World.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isRemainOutputValid, getRemainOutput, onRemainOutputUpdate
     **/
    inline void notifyOnRemainOutputUpdate( bool notify = true );
    /**
     * \brief   Triggered, when RemainOutput attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes RemainOutput description: 
     *          Remaining number of outputs to print Hello World.
     * \param   RemainOutput    The value of RemainOutput attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onRemainOutputUpdate( short RemainOutput, NEService::eDataStateType state );


//////////////////////////////////////////////////////////////////////////
// LocalHelloWorld Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request HelloWorld
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to print hello world
     * \param   roleName    The role name of client component that requested to print hello world
     * \param   addMessage  Additional message to output. Can be empty.
     *          Has default value: ""
     * \return  The sequence count number of call
     * \see     responseHelloWorld
     **/
    inline unsigned int requestHelloWorld( const String & roleName, const String & addMessage = "" );
    /**
     * \brief   Overwrite to handle error of HelloWorld request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestHelloWorldFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Response HelloWorld
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          The response to hello world request.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientInfo  The client information set by servicing component. If empty or invalid ID, the message output failed.
     * \see     requestHelloWorld
     **/
    virtual void responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo );
    /**
     * \brief   Call to enable or disable receiving notifications on HelloWorld response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseHelloWorld( bool notify = true );

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
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy );

/************************************************************************/
// LocalHelloWorldClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NELocalHelloWorld::eMessageIDs InvalidReqId );
    
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
     *         The function can return NULL if Proxy was not instantiated yet.
     **/
    DispatcherThread * getDispatcherThread( void );
     
    /**
     * \brief   Returns the current sequence number
     **/
    inline unsigned int getCurrentSequenceNr( void ) const;

    /**
     * \brief  Returns instance of proxy object.
     */
    inline const LocalHelloWorldProxy * getProxy( void ) const;
      
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
    LocalHelloWorldProxy *   mProxy;

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
    virtual void processNotificationEvent( NotificationEvent & eventElem );
    
/************************************************************************/
// LocalHelloWorldClientBase hidden methods
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
    void notifyOn( NELocalHelloWorld::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NELocalHelloWorld::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NELocalHelloWorld::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of LocalHelloWorldClientBase object
     **/

    inline LocalHelloWorldClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalHelloWorldClientBase( void );
    LocalHelloWorldClientBase( const LocalHelloWorldClientBase & /* src */ );
    const LocalHelloWorldClientBase & operator = ( const LocalHelloWorldClientBase & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline LocalHelloWorldClientBase & LocalHelloWorldClientBase::self( void )
{
    return (*this);
}

inline unsigned int LocalHelloWorldClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void LocalHelloWorldClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != NULL);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool LocalHelloWorldClientBase::isConnected( void ) const
{
    ASSERT(mProxy != NULL);
    return mIsConnected;
}

inline bool LocalHelloWorldClientBase::isNotificationAssigned( NELocalHelloWorld::eMessageIDs msgId ) const
{
    ASSERT(mProxy != NULL);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & LocalHelloWorldClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != NULL);
    return mProxy->getProxyAddress().getServiceName();
}
    
inline const Version & LocalHelloWorldClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != NULL);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool LocalHelloWorldClientBase::isConnectedClientsValid( void ) const
{
    ASSERT(mProxy != NULL);
   return mProxy->isConnectedClientsValid( );
}
inline const NELocalHelloWorld::ConnectionList & LocalHelloWorldClientBase::getConnectedClients( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != NULL);
    return mProxy->getConnectedClients( state );
}

inline void LocalHelloWorldClientBase::notifyOnConnectedClientsUpdate( bool notify /* = true */ )
{
    notifyOn( NELocalHelloWorld::MSG_ID_ConnectedClients, notify, false );
}

inline bool LocalHelloWorldClientBase::isRemainOutputValid( void ) const
{
    ASSERT(mProxy != NULL);
   return mProxy->isRemainOutputValid( );
}
inline short LocalHelloWorldClientBase::getRemainOutput( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != NULL);
    return mProxy->getRemainOutput( state );
}

inline void LocalHelloWorldClientBase::notifyOnRemainOutputUpdate( bool notify /* = true */ )
{
    notifyOn( NELocalHelloWorld::MSG_ID_RemainOutput, notify, false );
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline unsigned int LocalHelloWorldClientBase::requestHelloWorld( const String & roleName, const String & addMessage/* = "" */ )
{
    ASSERT(mProxy != NULL);
    return mProxy->requestHelloWorld( static_cast<IENotificationEventConsumer &>(self()), roleName, addMessage );
}

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void LocalHelloWorldClientBase::notifyOnResponseHelloWorld( bool notify /* = true */ )
{
    notifyOn(NELocalHelloWorld::MSG_ID_responseHelloWorld, notify, false);
}

inline const LocalHelloWorldProxy * LocalHelloWorldClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & LocalHelloWorldClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != NULL);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // SHAREIPCMIX_SRC_LOCALHELLOWORLDCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/LocalHelloWorldClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
