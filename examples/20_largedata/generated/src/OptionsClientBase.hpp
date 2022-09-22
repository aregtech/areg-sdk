//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/OptionsClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_OPTIONSCLIENTBASE_HPP
#define  GENERATED_SRC_OPTIONSCLIENTBASE_HPP

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.09.2022  23:32:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source Options.
 *
 * \file            generated/src/OptionsClientBase.hpp
 * \ingroup         Options Service Interface
 * \brief           This is an automatic generated code of Options
 *                  Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEOptions.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/NotificationEvent.hpp"

#include "generated/src/private/OptionsProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// OptionsClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of Options Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              This SI sets options to generate data
 **/
class OptionsClientBase  : public IEProxyListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialize Options Service Interface client object.
     *          Specifies used service and owner thread.
     * \param   roleName    The role name assigned to Options servicing component object.
     * \param   ownerThread The name of component owner thread to dispatch messages.
     *                      If nullptr, all messages are dispatched in current component thread.
     **/
    OptionsClientBase( const String & roleName, const String & ownerThread = String::EmptyString );

    /**
     * \brief   Initialize Options Service Interface client object.
     *          Specifies used service and owner thread.
     * \param   roleName    The role name assigned to Options servicing component object.
     * \param   ownerThread The instance of component owner thread to dispatch messages.
     **/
    OptionsClientBase( const String & roleName, DispatcherThread & ownerThread );

    /**
     * \brief   Initialize Options Service Interface client object.
     *          Specifies used service and owner component.
     * \param   roleName    The role name assigned to Options servicing component object.
     * \param   owner       The instance of client owner component. The component object should be already initialized.
     * \note    When this constructor is used, it is important that the Component object is already initialized.
     *          and the component thread is set.
     **/
    OptionsClientBase( const String & roleName, Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~OptionsClientBase( void );

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
    inline bool isNotificationAssigned( NEOptions::eMessageIDs msgId ) const;

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
// Options Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Attribute OptionData functions
 ************************************************************************/
    /**
     * \brief   Returns true if value of OptionData attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isOptionDataValid( void ) const;
    /**
     * \brief   Returns the value of OptionData attribute.
     *          To get valid value, the Update Notification should be enabled.
     *          Attribute OptionData description:
     *          The options
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     * \see     isOptionDataValid, notifyOptionDataUpdate, onOptionDataUpdate
     **/
    inline const NEOptions::sOptionData & getOptionData( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on OptionData attribute update.
     *          Once notification is enabled and the data is updated,
     *          the getOptionData method will return valid data
     *          Attribute OptionData description:
     *          The options
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     isOptionDataValid, getOptionData, onOptionDataUpdate
     **/
    inline void notifyOnOptionDataUpdate( bool notify = true );
    /**
     * \brief   Triggered, when OptionData attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes OptionData description:
     *          The options
     * \param   OptionData  The value of OptionData attribute.
     * \param   state       The data validation flag.
     **/
    virtual void onOptionDataUpdate( const NEOptions::sOptionData & OptionData, NEService::eDataStateType state );

//////////////////////////////////////////////////////////////////////////
// Options Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request DataRate
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to get data rate values
     * \return  The sequence count number of call
     * \see     responseDataRate
     **/
    inline unsigned int requestDataRate( void );
    /**
     * \brief   Overwrite to handle error of DataRate request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestDataRateFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request PacketRate
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to get packetrate, i.e. the numberof generated image blocks per second
     * \return  The sequence count number of call
     * \see     responsePacketRate
     **/
    inline unsigned int requestPacketRate( void );
    /**
     * \brief   Overwrite to handle error of PacketRate request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestPacketRateFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request ChangeOptions
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to change the options to generate images.
     * \param   Options DESCRIPTION MISSED
     * \see     Has no response
     **/
    inline void requestChangeOptions( const NEOptions::sOptionData & Options );
    /**
     * \brief   Overwrite to handle error of ChangeOptions request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestChangeOptionsFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Request QuitApplication
 ************************************************************************/
    /**
     * \brief   Request call.
     *          DESCRIPTION MISSED
     * \see     Has no response
     **/
    inline void requestQuitApplication( void );
    /**
     * \brief   Overwrite to handle error of QuitApplication request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestQuitApplicationFailed( NEService::eResultType FailureReason );

/************************************************************************
 * Response DataRate
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Response of data rate
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   sentData    Computed rate of sent data per second.
     * \param   missedData  Computed rate of missed data. The data is missed if failed to copy within certain timeshlot.
     *          TODO: remove if useless.
     * \see     requestDataRate
     **/
    virtual void responseDataRate( const NEOptions::sDataRate & sentData, const NEOptions::sDataRate & missedData );
    /**
     * \brief   Call to enable or disable receiving notifications on DataRate response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponseDataRate( bool notify = true );

/************************************************************************
 * Response PacketRate
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Response of packet rate.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   genPackets  Number of generate packet per second
     * \param   avSize      The average size of a packet.
     * \see     requestPacketRate
     **/
    virtual void responsePacketRate( unsigned int genPackets, const NEOptions::sDataRate & avSize );
    /**
     * \brief   Call to enable or disable receiving notifications on PacketRate response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void notifyOnResponsePacketRate( bool notify = true );

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
// OptionsClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void invalidRequest( NEOptions::eMessageIDs InvalidReqId );

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
    inline const OptionsProxy * getProxy( void ) const;

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
    OptionsProxy *   mProxy;

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
// OptionsClientBase hidden methods
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
    void notifyOn( NEOptions::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
     void invalidResponse( NEOptions::eMessageIDs InvalidRespId );

    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestFailed( NEOptions::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

    /**
     * \brief   Returns reference of OptionsClientBase object
     **/
    inline OptionsClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    OptionsClientBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( OptionsClientBase );
};

//////////////////////////////////////////////////////////////////////////
// OptionsClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline OptionsClientBase & OptionsClientBase::self( void )
{
    return (*this);
}

inline unsigned int OptionsClientBase::getCurrentSequenceNr( void ) const
{
    return mCurrSequenceNr;
}

inline void OptionsClientBase::clearAllNotifications( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

inline bool OptionsClientBase::isConnected( void ) const
{
    ASSERT(mProxy != nullptr);
    return mIsConnected;
}

inline bool OptionsClientBase::isNotificationAssigned( NEOptions::eMessageIDs msgId ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->hasNotificationListener(static_cast<unsigned int>(msgId));
}

inline const String & OptionsClientBase::getServiceName( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceName();
}

inline const Version & OptionsClientBase::getServiceVersion( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getServiceVersion();
}

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool OptionsClientBase::isOptionDataValid( void ) const
{
    ASSERT(mProxy != nullptr);
   return mProxy->isOptionDataValid( );
}
inline const NEOptions::sOptionData & OptionsClientBase::getOptionData( NEService::eDataStateType & state ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getOptionData( state );
}

inline void OptionsClientBase::notifyOnOptionDataUpdate( bool notify /* = true */ )
{
    OptionsClientBase::notifyOn( NEOptions::eMessageIDs::MsgId_OptionData, notify, false );
}

/************************************************************************
 * Request calls
 ************************************************************************/

inline unsigned int OptionsClientBase::requestDataRate( void )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestDataRate( static_cast<IENotificationEventConsumer &>(self()) );
}

inline unsigned int OptionsClientBase::requestPacketRate( void )
{
    ASSERT(mProxy != nullptr);
    return mProxy->requestPacketRate( static_cast<IENotificationEventConsumer &>(self()) );
}

inline void OptionsClientBase::requestChangeOptions( const NEOptions::sOptionData & Options )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestChangeOptions( Options );
}

inline void OptionsClientBase::requestQuitApplication( void )
{
    ASSERT(mProxy != nullptr);
    mProxy->requestQuitApplication(  );
}

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void OptionsClientBase::notifyOnResponseDataRate( bool notify /* = true */ )
{
    OptionsClientBase::notifyOn(NEOptions::eMessageIDs::MsgId_responseDataRate, notify, false);
}

inline void OptionsClientBase::notifyOnResponsePacketRate( bool notify /* = true */ )
{
    OptionsClientBase::notifyOn(NEOptions::eMessageIDs::MsgId_responsePacketRate, notify, false);
}

inline const OptionsProxy * OptionsClientBase::getProxy( void ) const
{
    return mProxy;
}

inline const String & OptionsClientBase::getServiceRole( void ) const
{
    ASSERT(mProxy != nullptr);
    return mProxy->getProxyAddress().getRoleName();
}

#endif   // GENERATED_SRC_OPTIONSCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/OptionsClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
