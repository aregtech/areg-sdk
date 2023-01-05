//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/PowerManagerStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_POWERMANAGERSTUB_HPP
#define  GENERATED_SRC_POWERMANAGERSTUB_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:38 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PowerManager.
 *
 * \file            generated/src/PowerManagerStub.hpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager
 *                  Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEPowerManager.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// PowerManagerStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       PowerManager Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              This is a power manager of Traffic Light Controller. It powers the lights ON and OFF.
 **/
class PowerManagerStub   : protected  StubBase
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
    explicit PowerManagerStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~PowerManagerStub( void ) = default;

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
// PowerManager Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute LightsPowerState functions
 ************************************************************************/

    /**
     * \brief   Returns true if LightsPowerState attribute is valid
     **/
    inline bool isLightsPowerStateValid( void ) const;
    /**
     * \brief   Invalidates LightsPowerState attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateLightsPowerState( void );
    /**
     * \brief   Force to send LightsPowerState attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyLightsPowerStateUpdated( void );
    /**
     * \brief   Set LightsPowerState attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of LightsPowerState attribute to set.
     *                      The LightsPowerState attribute description:
     *                      The power state of the traffic lights.
     **/
    virtual void setLightsPowerState( const NEPowerManager::ePoweredState & newValue );
    /**
     * \brief   Returns the value of LightsPowerState attribute.
     *          The LightsPowerState attribute description:
     *          The power state of the traffic lights.
     **/
    inline const NEPowerManager::ePoweredState & getLightsPowerState( void ) const;
    inline NEPowerManager::ePoweredState & getLightsPowerState( void );

//////////////////////////////////////////////////////////////////////////
// PowerManager Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Called to power ON the traffic lights.
     * \note    Has no response
     **/
    virtual void requestPowerOn( void ) = 0;

    /**
     * \brief   Request call.
     *          Called to power OFF the traffic lights.
     * \note    Has no response
     **/
    virtual void requestPowerOff( void ) = 0;

    /**
     * \brief   Request call.
     *          Triggered to start the traffic light
     * \see     responseStartTrafficLight
     **/
    virtual void requestStartTrafficLight( void ) = 0;

    /**
     * \brief   Request call.
     *          Call to stop the traffic lights and put them in initialization state.
     * \see     responseStopTrafficLight
     **/
    virtual void requestStopTrafficLight( void ) = 0;

//////////////////////////////////////////////////////////////////////////
// PowerManager Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Response call.
     *          Sent as a response to start the traffic light. The traffic light can be started when it is powered ON and in initialization state.
     * \param   Success Flag, indicating whether the operation succeeded or not.
     *          This flag is 'true' if lights are initialization state of if traffic light is already functioning.
     *          This flag is 'false' if lights are OFF.
     * \see     requestStartTrafficLight
     **/
    virtual void responseStartTrafficLight( bool Success );

    /**
     * \brief   Response call.
     *          Response to stop the traffic lights.
     * \param   Success Flag, indicating whether the request was processed with success or not.
     *          This flag is 'true' if traffic light are functioning or lights are in initialization state.
     *          This flag is 'false' if traffic lights are OFF.
     * \see     requestStopTrafficLight
     **/
    virtual void responseStopTrafficLight( bool Success );

//////////////////////////////////////////////////////////////////////////
// PowerManager Interface Broadcasts
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
     * \brief   Returns reference to PowerManagerStub object
     **/
    inline PowerManagerStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute LightsPowerState
     *          The power state of the traffic lights.
     **/
    NEPowerManager::ePoweredState   mLightsPowerState;
    /**
     * \brief   Attribute LightsPowerState data validation state.
     **/
    NEService::eDataStateType       mLightsPowerStateState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PowerManagerStub( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PowerManagerStub );
};

//////////////////////////////////////////////////////////////////////////
// PowerManagerStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline PowerManagerStub & PowerManagerStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   LightsPowerState attribute functions
 ************************************************************************/
 
inline bool PowerManagerStub::isLightsPowerStateValid( void ) const
{
    return (mLightsPowerStateState  == NEService::eDataStateType::DataIsOK);
}

inline void PowerManagerStub::invalidateLightsPowerState( void )
{
    errorRequest( static_cast<msg_id>(NEPowerManager::eMessageIDs::MsgId_LightsPowerState), false );
}

inline void PowerManagerStub::notifyLightsPowerStateUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEPowerManager::eMessageIDs::MsgId_LightsPowerState) );
}

inline const NEPowerManager::ePoweredState & PowerManagerStub::getLightsPowerState( void ) const
{
    return mLightsPowerState;
}

inline NEPowerManager::ePoweredState & PowerManagerStub::getLightsPowerState( void )
{
    return mLightsPowerState;
}

//////////////////////////////////////////////////////////////////////////
// PowerManagerStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_POWERMANAGERSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/PowerManagerStub.hpp file
//////////////////////////////////////////////////////////////////////////
