//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/TrafficControllerStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_TRAFFICCONTROLLERSTUB_HPP
#define  GENERATED_SRC_TRAFFICCONTROLLERSTUB_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:40 GMT+01:00
 *                  Create by AREG SDK code generator tool from source TrafficController.
 *
 * \file            generated/src/TrafficControllerStub.hpp
 * \ingroup         TrafficController Service Interface
 * \brief           This is an automatic generated code of TrafficController
 *                  Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NETrafficController.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// TrafficControllerStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       TrafficController Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              This is a traffic light controlle service interface.
 **/
class TrafficControllerStub   : protected  StubBase
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
    explicit TrafficControllerStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~TrafficControllerStub( void ) = default;

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
// TrafficController Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute TrafficSouthNorth functions
 ************************************************************************/

    /**
     * \brief   Returns true if TrafficSouthNorth attribute is valid
     **/
    inline bool isTrafficSouthNorthValid( void ) const;
    /**
     * \brief   Invalidates TrafficSouthNorth attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateTrafficSouthNorth( void );
    /**
     * \brief   Force to send TrafficSouthNorth attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyTrafficSouthNorthUpdated( void );
    /**
     * \brief   Set TrafficSouthNorth attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of TrafficSouthNorth attribute to set.
     *                      The TrafficSouthNorth attribute description:
     *                      The south-north traffic.
     **/
    virtual void setTrafficSouthNorth( const NETrafficController::sTrafficLight & newValue );
    /**
     * \brief   Returns the value of TrafficSouthNorth attribute.
     *          The TrafficSouthNorth attribute description:
     *          The south-north traffic.
     **/
    inline const NETrafficController::sTrafficLight & getTrafficSouthNorth( void ) const;
    inline NETrafficController::sTrafficLight & getTrafficSouthNorth( void );

/************************************************************************
 * Attribute TrafficEastWest functions
 ************************************************************************/

    /**
     * \brief   Returns true if TrafficEastWest attribute is valid
     **/
    inline bool isTrafficEastWestValid( void ) const;
    /**
     * \brief   Invalidates TrafficEastWest attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateTrafficEastWest( void );
    /**
     * \brief   Force to send TrafficEastWest attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyTrafficEastWestUpdated( void );
    /**
     * \brief   Set TrafficEastWest attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of TrafficEastWest attribute to set.
     *                      The TrafficEastWest attribute description:
     *                      The east-west traffic
     **/
    virtual void setTrafficEastWest( const NETrafficController::sTrafficLight & newValue );
    /**
     * \brief   Returns the value of TrafficEastWest attribute.
     *          The TrafficEastWest attribute description:
     *          The east-west traffic
     **/
    inline const NETrafficController::sTrafficLight & getTrafficEastWest( void ) const;
    inline NETrafficController::sTrafficLight & getTrafficEastWest( void );

//////////////////////////////////////////////////////////////////////////
// TrafficController Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// TrafficController Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// TrafficController Interface Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Broadcast call.
     *          Broadcasts the light states of south-north direction.
     * \param   LightVehicle    Light state for vehicles
     * \param   LightPedestrian Light state for pedestrians.
     **/
    virtual void broadcastSouthNorth( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian );

    /**
     * \brief   Broadcast call.
     *          Broadcasts the light states of east-west direction.
     * \param   LightVehicle    Light state for vehicles
     * \param   LightPedestrian Light state for pedestrians.
     **/
    virtual void broadcastEastWest( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian );

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
     * \brief   Returns reference to TrafficControllerStub object
     **/
    inline TrafficControllerStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute TrafficSouthNorth
     *          The south-north traffic.
     **/
    NETrafficController::sTrafficLight  mTrafficSouthNorth;
    /**
     * \brief   Attribute TrafficSouthNorth data validation state.
     **/
    NEService::eDataStateType           mTrafficSouthNorthState;

    /**
     * \brief   Attribute TrafficEastWest
     *          The east-west traffic
     **/
    NETrafficController::sTrafficLight  mTrafficEastWest;
    /**
     * \brief   Attribute TrafficEastWest data validation state.
     **/
    NEService::eDataStateType           mTrafficEastWestState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TrafficControllerStub( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( TrafficControllerStub );
};

//////////////////////////////////////////////////////////////////////////
// TrafficControllerStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline TrafficControllerStub & TrafficControllerStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   TrafficSouthNorth attribute functions
 ************************************************************************/
 
inline bool TrafficControllerStub::isTrafficSouthNorthValid( void ) const
{
    return (mTrafficSouthNorthState  == NEService::eDataStateType::DataIsOK);
}

inline void TrafficControllerStub::invalidateTrafficSouthNorth( void )
{
    errorRequest( static_cast<msg_id>(NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth), false );
}

inline void TrafficControllerStub::notifyTrafficSouthNorthUpdated( void )
{
    sendNotification( static_cast<msg_id>(NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth) );
}

inline const NETrafficController::sTrafficLight & TrafficControllerStub::getTrafficSouthNorth( void ) const
{
    return mTrafficSouthNorth;
}

inline NETrafficController::sTrafficLight & TrafficControllerStub::getTrafficSouthNorth( void )
{
    return mTrafficSouthNorth;
}

/************************************************************************
 * \brief   TrafficEastWest attribute functions
 ************************************************************************/
 
inline bool TrafficControllerStub::isTrafficEastWestValid( void ) const
{
    return (mTrafficEastWestState  == NEService::eDataStateType::DataIsOK);
}

inline void TrafficControllerStub::invalidateTrafficEastWest( void )
{
    errorRequest( static_cast<msg_id>(NETrafficController::eMessageIDs::MsgId_TrafficEastWest), false );
}

inline void TrafficControllerStub::notifyTrafficEastWestUpdated( void )
{
    sendNotification( static_cast<msg_id>(NETrafficController::eMessageIDs::MsgId_TrafficEastWest) );
}

inline const NETrafficController::sTrafficLight & TrafficControllerStub::getTrafficEastWest( void ) const
{
    return mTrafficEastWest;
}

inline NETrafficController::sTrafficLight & TrafficControllerStub::getTrafficEastWest( void )
{
    return mTrafficEastWest;
}

//////////////////////////////////////////////////////////////////////////
// TrafficControllerStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_TRAFFICCONTROLLERSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/TrafficControllerStub.hpp file
//////////////////////////////////////////////////////////////////////////
