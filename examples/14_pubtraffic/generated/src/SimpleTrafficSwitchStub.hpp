//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/SimpleTrafficSwitchStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_SIMPLETRAFFICSWITCHSTUB_HPP
#define  GENERATED_SRC_SIMPLETRAFFICSWITCHSTUB_HPP

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     13.08.2022  02:45:34 GMT+02:00
 *                  Create by AREG SDK code generator tool from source SimpleTrafficSwitch.
 *
 * \file            generated/src/SimpleTrafficSwitchStub.hpp
 * \ingroup         SimpleTrafficSwitch Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficSwitch
 *                  Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NESimpleTrafficSwitch.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       SimpleTrafficSwitch Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              Simple traffic light switch on/off.
 *              Declared as local service to prevent any other external client to send unexpected signal to switch off the light.
 **/
class SimpleTrafficSwitchStub   : protected  StubBase
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
    explicit SimpleTrafficSwitchStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~SimpleTrafficSwitchStub( void ) = default;

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
// SimpleTrafficSwitch Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute IsSwitchedOn functions
 ************************************************************************/

    /**
     * \brief   Returns true if IsSwitchedOn attribute is valid
     **/
    inline bool isIsSwitchedOnValid( void ) const;
    /**
     * \brief   Invalidates IsSwitchedOn attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateIsSwitchedOn( void );
    /**
     * \brief   Force to send IsSwitchedOn attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyIsSwitchedOnUpdated( void );
    /**
     * \brief   Set IsSwitchedOn attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of IsSwitchedOn attribute to set.
     *                      The IsSwitchedOn attribute description:
     *                      Flag that indicates the light on/off state.
     **/
    virtual void setIsSwitchedOn( const bool & newValue );
    /**
     * \brief   Returns the value of IsSwitchedOn attribute.
     *          The IsSwitchedOn attribute description:
     *          Flag that indicates the light on/off state.
     **/
    inline const bool & getIsSwitchedOn( void ) const;
    inline bool & getIsSwitchedOn( void );

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitch Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Sets the traffic light ON or OFF.
     * \param   switchOn    If true, the traffic light is switched ON. Otherwise, it is OFF.
     * \note    Has no response
     **/
    virtual void requestSwitchLight( bool switchOn ) = 0;

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitch Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitch Interface Broadcasts
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
     * \brief   Returns reference to SimpleTrafficSwitchStub object
     **/
    inline SimpleTrafficSwitchStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute IsSwitchedOn
     *          Flag that indicates the light on/off state.
     **/
    bool                        mIsSwitchedOn;
    /**
     * \brief   Attribute IsSwitchedOn data validation state.
     **/
    NEService::eDataStateType   mIsSwitchedOnState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SimpleTrafficSwitchStub( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SimpleTrafficSwitchStub );
};

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline SimpleTrafficSwitchStub & SimpleTrafficSwitchStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   IsSwitchedOn attribute functions
 ************************************************************************/
 
inline bool SimpleTrafficSwitchStub::isIsSwitchedOnValid( void ) const
{
    return (mIsSwitchedOnState  == NEService::eDataStateType::DataIsOK);
}

inline void SimpleTrafficSwitchStub::invalidateIsSwitchedOn( void )
{
    errorRequest( static_cast<msg_id>(NESimpleTrafficSwitch::eMessageIDs::MsgId_IsSwitchedOn), false );
}

inline void SimpleTrafficSwitchStub::notifyIsSwitchedOnUpdated( void )
{
    sendNotification( static_cast<msg_id>(NESimpleTrafficSwitch::eMessageIDs::MsgId_IsSwitchedOn) );
}

inline const bool & SimpleTrafficSwitchStub::getIsSwitchedOn( void ) const
{
    return mIsSwitchedOn;
}

inline bool & SimpleTrafficSwitchStub::getIsSwitchedOn( void )
{
    return mIsSwitchedOn;
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_SIMPLETRAFFICSWITCHSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/SimpleTrafficSwitchStub.hpp file
//////////////////////////////////////////////////////////////////////////
