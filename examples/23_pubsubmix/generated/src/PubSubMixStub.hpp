//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/PubSubMixStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PUBSUBMIXSTUB_HPP
#define  GENERATED_SRC_PUBSUBMIXSTUB_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     15.11.2023  14:52:21 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PubSubMix.
 *
 * \file            generated/src/PubSubMixStub.hpp
 * \ingroup         PubSubMix Service Interface
 * \brief           This is an automatic generated code of PubSubMix
 *                  Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEPubSubMix.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// PubSubMixStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       PubSubMix Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              This is an interface to test the PubSub features (data-driven communication) of the
 *              AREG framework, where each service provider is a Publisher and each service consumer is a Subscriber that dynamically
 *              subscribes on data update and get notified. The agenda of this example to demonstrate mixture of Publishers and Subscribers
 *              and testing the correct behavior of notifications when Subscribers run in the same or different processes.
 **/
class PubSubMixStub   : protected  StubBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes Stub by given component object, which should be already instantiated.
     * \param   masterComp  The master component object, which is initializing service Stub.
     * \note    Before constructor is called, the instance of Component must be already initialized.
     **/
    explicit PubSubMixStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~PubSubMixStub( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Unlocks and cancels all requests, excepts those which were manually unlocked.
     *          This call will send cancel error message (NEService::eResultType::RequestCanceled)
     *          to all clients waiting for response.
     **/
    void unlockAllRequests( void );

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
     * \brief   Returns true if StringOnChange attribute is valid
     **/
    inline bool isStringOnChangeValid( void ) const;
    /**
     * \brief   Invalidates StringOnChange attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateStringOnChange( void );
    /**
     * \brief   Force to send StringOnChange attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyStringOnChangeUpdated( void );
    /**
     * \brief   Set StringOnChange attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of StringOnChange attribute to set.
     *                      The StringOnChange attribute description:
     *                      This string should send update notification only on change.
     **/
    virtual void setStringOnChange( const NEPubSubMix::sString & newValue );
    /**
     * \brief   Returns the value of StringOnChange attribute.
     *          The StringOnChange attribute description:
     *          This string should send update notification only on change.
     **/
    inline const NEPubSubMix::sString & getStringOnChange( void ) const;
    inline NEPubSubMix::sString & getStringOnChange( void );

/************************************************************************
 * Attribute IntegerAlways functions
 ************************************************************************/

    /**
     * \brief   Returns true if IntegerAlways attribute is valid
     **/
    inline bool isIntegerAlwaysValid( void ) const;
    /**
     * \brief   Invalidates IntegerAlways attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateIntegerAlways( void );
    /**
     * \brief   Force to send IntegerAlways attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyIntegerAlwaysUpdated( void );
    /**
     * \brief   Set IntegerAlways attribute value and send notification event to all clients.
     *          The notification is sent every time when value set.
     * \param   newValue    New value of IntegerAlways attribute to set.
     *                      The IntegerAlways attribute description:
     *                      This  integer should send update notification always when set.
     **/
    virtual void setIntegerAlways( const NEPubSubMix::sInteger & newValue );
    /**
     * \brief   Returns the value of IntegerAlways attribute.
     *          The IntegerAlways attribute description:
     *          This  integer should send update notification always when set.
     **/
    inline const NEPubSubMix::sInteger & getIntegerAlways( void ) const;
    inline NEPubSubMix::sInteger & getIntegerAlways( void );

/************************************************************************
 * Attribute ServiceProviderState functions
 ************************************************************************/

    /**
     * \brief   Returns true if ServiceProviderState attribute is valid
     **/
    inline bool isServiceProviderStateValid( void ) const;
    /**
     * \brief   Invalidates ServiceProviderState attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateServiceProviderState( void );
    /**
     * \brief   Force to send ServiceProviderState attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyServiceProviderStateUpdated( void );
    /**
     * \brief   Set ServiceProviderState attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of ServiceProviderState attribute to set.
     *                      The ServiceProviderState attribute description:
     *                      The state of service provider. The value is always valid, updated on change.
     **/
    virtual void setServiceProviderState( const NEPubSubMix::eServiceState & newValue );
    /**
     * \brief   Returns the value of ServiceProviderState attribute.
     *          The ServiceProviderState attribute description:
     *          The state of service provider. The value is always valid, updated on change.
     **/
    inline const NEPubSubMix::eServiceState & getServiceProviderState( void ) const;
    inline NEPubSubMix::eServiceState & getServiceProviderState( void );

//////////////////////////////////////////////////////////////////////////
// PubSubMix Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// PubSubMix Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// PubSubMix Interface Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// End Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//  Operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// StubBase overrides. Public methods.
/************************************************************************/

    /**
     * \brief   Sends update notification message to all clients.
     *          This method can be called manually to send update
     *          notification message after updating attribute value.
     *
     * \param   msgId   The attribute message ID to notify clients.
     **/
    virtual void sendNotification( unsigned int msgId ) override;

    /**
     * \brief   Sends error message to clients.
     *          If message ID is a request, it should send result NEService::eResultType::RequestError
     *          or NEService::eResultType::RequestCanceled, depending on msgCancel flag.
     *          If message ID is a response, it should send result NEService::eResultType::RequestInvalid.
     *          If message ID is an attribute, it should send result NEService::eResultType::DataInvalid
     *          and invalidate attribute data value.
     *
     * \param   msgId       The message ID to send error message
     * \param   msgCancel   Indicates whether the request is canceled or should respond with error.
     *                      This parameter has sense only for request IDs.
     *                      It is ignored for response and attributes IDs.
     **/
    virtual void errorRequest( unsigned int msgId, bool msgCancel ) override;

protected:
/************************************************************************/
// StubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void startupServiceInterface( Component & holder ) override;

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void shutdownServiceIntrface ( Component & holder ) override;

/************************************************************************/
// StubBase overrides. Protected methods
/************************************************************************/

    /**
     * \brief   Creates Response event object to pass of client.
     * \param   proxy   The address of proxy object to send response event
     * \param   msgId   The message ID to send to client
     * \param   result  The result of message
     * \param   data    The buffer of data to send to client. Can be Invalid buffer.
     * \return  Returns valid pointer to Response event object
     **/
    virtual ResponseEvent * createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const override;

    /**
     * \brief   Overwrite method to create remote service request event from streaming object for
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns nullptr.
     **/
    virtual RemoteRequestEvent * createRemoteRequestEvent( const IEInStream & stream ) const override;

    /**
     * \brief   Overwrite method to create remote notify request event from streaming object for
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns nullptr.
     **/
    virtual RemoteNotifyRequestEvent * createRemoteNotifyRequestEvent( const IEInStream & stream ) const override;

private:
/************************************************************************/
// IEComponentConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered to process service request event.
     *          Overwrite method to process every service request event.
     * \param   eventElem   Service Request Event object, contains request
     *                      call ID and parameters.
     **/
    virtual void processRequestEvent( ServiceRequestEvent & eventElem ) override;

    /**
     * \brief   Triggered to process attribute update notification event.
     *          Override method to process request to get attribute value and
     *          process notification request of attribute update.
     * \param   eventElem   Service Request Event object, contains attribute ID.
     **/
    virtual void processAttributeEvent( ServiceRequestEvent & eventElem ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to PubSubMixStub object
     **/
    inline PubSubMixStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute StringOnChange
     *          This string should send update notification only on change.
     **/
    NEPubSubMix::sString        mStringOnChange;
    /**
     * \brief   Attribute StringOnChange data validation state.
     **/
    NEService::eDataStateType   mStringOnChangeState;

    /**
     * \brief   Attribute IntegerAlways
     *          This  integer should send update notification always when set.
     **/
    NEPubSubMix::sInteger       mIntegerAlways;
    /**
     * \brief   Attribute IntegerAlways data validation state.
     **/
    NEService::eDataStateType   mIntegerAlwaysState;

    /**
     * \brief   Attribute ServiceProviderState
     *          The state of service provider. The value is always valid, updated on change.
     **/
    NEPubSubMix::eServiceState  mServiceProviderState;
    /**
     * \brief   Attribute ServiceProviderState data validation state.
     **/
    NEService::eDataStateType   mServiceProviderStateState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PubSubMixStub( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PubSubMixStub );
};

//////////////////////////////////////////////////////////////////////////
// PubSubMixStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline PubSubMixStub & PubSubMixStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   StringOnChange attribute functions
 ************************************************************************/
 
inline bool PubSubMixStub::isStringOnChangeValid( void ) const
{
    return (mStringOnChangeState  == NEService::eDataStateType::DataIsOK);
}

inline void PubSubMixStub::invalidateStringOnChange( void )
{
    errorRequest( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_StringOnChange), false );
}

inline void PubSubMixStub::notifyStringOnChangeUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_StringOnChange) );
}

inline const NEPubSubMix::sString & PubSubMixStub::getStringOnChange( void ) const
{
    return mStringOnChange;
}

inline NEPubSubMix::sString & PubSubMixStub::getStringOnChange( void )
{
    return mStringOnChange;
}

/************************************************************************
 * \brief   IntegerAlways attribute functions
 ************************************************************************/
 
inline bool PubSubMixStub::isIntegerAlwaysValid( void ) const
{
    return (mIntegerAlwaysState  == NEService::eDataStateType::DataIsOK);
}

inline void PubSubMixStub::invalidateIntegerAlways( void )
{
    errorRequest( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_IntegerAlways), false );
}

inline void PubSubMixStub::notifyIntegerAlwaysUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_IntegerAlways) );
}

inline const NEPubSubMix::sInteger & PubSubMixStub::getIntegerAlways( void ) const
{
    return mIntegerAlways;
}

inline NEPubSubMix::sInteger & PubSubMixStub::getIntegerAlways( void )
{
    return mIntegerAlways;
}

/************************************************************************
 * \brief   ServiceProviderState attribute functions
 ************************************************************************/
 
inline bool PubSubMixStub::isServiceProviderStateValid( void ) const
{
    return (mServiceProviderStateState  == NEService::eDataStateType::DataIsOK);
}

inline void PubSubMixStub::invalidateServiceProviderState( void )
{
    errorRequest( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState), false );
}

inline void PubSubMixStub::notifyServiceProviderStateUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState) );
}

inline const NEPubSubMix::eServiceState & PubSubMixStub::getServiceProviderState( void ) const
{
    return mServiceProviderState;
}

inline NEPubSubMix::eServiceState & PubSubMixStub::getServiceProviderState( void )
{
    return mServiceProviderState;
}

//////////////////////////////////////////////////////////////////////////
// PubSubMixStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_PUBSUBMIXSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/PubSubMixStub.hpp file
//////////////////////////////////////////////////////////////////////////
