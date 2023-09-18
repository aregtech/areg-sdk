//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/PubSubStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PUBSUBSTUB_HPP
#define  GENERATED_SRC_PUBSUBSTUB_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     18.09.2023  09:14:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PubSub.
 *
 * \file            generated/src/PubSubStub.hpp
 * \ingroup         PubSub Service Interface
 * \brief           This is an automatic generated code of PubSub
 *                  Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEPubSub.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// PubSubStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       PubSub Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              This is an interface to test the PubSub features (data-driven communication) of the
 *              AREG framework, where service provider is a Publisher and service consumer is a Subscriber that dynamically
 *              subscribes on data update and get notified.
 **/
class PubSubStub   : protected  StubBase
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
    explicit PubSubStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~PubSubStub( void ) = default;

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
// PubSub Interface Attributes
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
     *                      This simple string should send update notification only on value change.
     **/
    virtual void setStringOnChange( const String & newValue );
    /**
     * \brief   Returns the value of StringOnChange attribute.
     *          The StringOnChange attribute description:
     *          This simple string should send update notification only on value change.
     **/
    inline const String & getStringOnChange( void ) const;
    inline String & getStringOnChange( void );

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
     *                      This simple integer should send update notification always when value is set.
     **/
    virtual void setIntegerAlways( const unsigned int & newValue );
    /**
     * \brief   Returns the value of IntegerAlways attribute.
     *          The IntegerAlways attribute description:
     *          This simple integer should send update notification always when value is set.
     **/
    inline const unsigned int & getIntegerAlways( void ) const;
    inline unsigned int & getIntegerAlways( void );

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
    virtual void setServiceProviderState( const NEPubSub::eServiceState & newValue );
    /**
     * \brief   Returns the value of ServiceProviderState attribute.
     *          The ServiceProviderState attribute description:
     *          The state of service provider. The value is always valid, updated on change.
     **/
    inline const NEPubSub::eServiceState & getServiceProviderState( void ) const;
    inline NEPubSub::eServiceState & getServiceProviderState( void );

//////////////////////////////////////////////////////////////////////////
// PubSub Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// PubSub Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// PubSub Interface Broadcasts
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
     * \brief   Returns reference to PubSubStub object
     **/
    inline PubSubStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute StringOnChange
     *          This simple string should send update notification only on value change.
     **/
    String                      mStringOnChange;
    /**
     * \brief   Attribute StringOnChange data validation state.
     **/
    NEService::eDataStateType   mStringOnChangeState;

    /**
     * \brief   Attribute IntegerAlways
     *          This simple integer should send update notification always when value is set.
     **/
    unsigned int                mIntegerAlways;
    /**
     * \brief   Attribute IntegerAlways data validation state.
     **/
    NEService::eDataStateType   mIntegerAlwaysState;

    /**
     * \brief   Attribute ServiceProviderState
     *          The state of service provider. The value is always valid, updated on change.
     **/
    NEPubSub::eServiceState     mServiceProviderState;
    /**
     * \brief   Attribute ServiceProviderState data validation state.
     **/
    NEService::eDataStateType   mServiceProviderStateState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PubSubStub( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PubSubStub );
};

//////////////////////////////////////////////////////////////////////////
// PubSubStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline PubSubStub & PubSubStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   StringOnChange attribute functions
 ************************************************************************/
 
inline bool PubSubStub::isStringOnChangeValid( void ) const
{
    return (mStringOnChangeState  == NEService::eDataStateType::DataIsOK);
}

inline void PubSubStub::invalidateStringOnChange( void )
{
    errorRequest( static_cast<msg_id>(NEPubSub::eMessageIDs::MsgId_StringOnChange), false );
}

inline void PubSubStub::notifyStringOnChangeUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEPubSub::eMessageIDs::MsgId_StringOnChange) );
}

inline const String & PubSubStub::getStringOnChange( void ) const
{
    return mStringOnChange;
}

inline String & PubSubStub::getStringOnChange( void )
{
    return mStringOnChange;
}

/************************************************************************
 * \brief   IntegerAlways attribute functions
 ************************************************************************/
 
inline bool PubSubStub::isIntegerAlwaysValid( void ) const
{
    return (mIntegerAlwaysState  == NEService::eDataStateType::DataIsOK);
}

inline void PubSubStub::invalidateIntegerAlways( void )
{
    errorRequest( static_cast<msg_id>(NEPubSub::eMessageIDs::MsgId_IntegerAlways), false );
}

inline void PubSubStub::notifyIntegerAlwaysUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEPubSub::eMessageIDs::MsgId_IntegerAlways) );
}

inline const unsigned int & PubSubStub::getIntegerAlways( void ) const
{
    return mIntegerAlways;
}

inline unsigned int & PubSubStub::getIntegerAlways( void )
{
    return mIntegerAlways;
}

/************************************************************************
 * \brief   ServiceProviderState attribute functions
 ************************************************************************/
 
inline bool PubSubStub::isServiceProviderStateValid( void ) const
{
    return (mServiceProviderStateState  == NEService::eDataStateType::DataIsOK);
}

inline void PubSubStub::invalidateServiceProviderState( void )
{
    errorRequest( static_cast<msg_id>(NEPubSub::eMessageIDs::MsgId_ServiceProviderState), false );
}

inline void PubSubStub::notifyServiceProviderStateUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEPubSub::eMessageIDs::MsgId_ServiceProviderState) );
}

inline const NEPubSub::eServiceState & PubSubStub::getServiceProviderState( void ) const
{
    return mServiceProviderState;
}

inline NEPubSub::eServiceState & PubSubStub::getServiceProviderState( void )
{
    return mServiceProviderState;
}

//////////////////////////////////////////////////////////////////////////
// PubSubStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_PUBSUBSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/PubSubStub.hpp file
//////////////////////////////////////////////////////////////////////////
