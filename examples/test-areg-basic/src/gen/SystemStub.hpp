//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/SystemStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_SYSTEMSTUB_HPP
#define  SRC_GEN_SYSTEMSTUB_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source System.
 * Generated at     12.05.2021  16:41:13 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/SystemStub.hpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "src/gen/NESystem.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// SystemStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       System Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              The system state.
 **/
class SystemStub   : protected  StubBase
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
    SystemStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~SystemStub( void );
    
//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Unlocks and cancels all requests, excepts those which were manually unlocked.
     *          This call will send cancel error message (NEService::RESULT_REQUEST_CANCELED) to all clients waiting for response.
     **/
    void unlockAllRequests( void );

//////////////////////////////////////////////////////////////////////////
// Start Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// System Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute SystemState functions
 ************************************************************************/

    /**
     * \brief   Returns true if SystemState attribute is valid
     **/
    inline bool isSystemStateValid( void ) const;
    /**
     * \brief   Invalidates SystemState attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateSystemState( void );
    /**
     * \brief   Force to send SystemState attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifySystemStateUpdated( void );
    /**
     * \brief   Set SystemState attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of SystemState attribute to set.
     *                      The SystemState attribute description:
     *                      The system state.
     **/
    virtual void setSystemState( const NESystem::eSystemState & newValue );
    /**
     * \brief   Returns the value of SystemState attribute.
     *          The SystemState attribute description:
     *          The system state.
     **/
    inline const NESystem::eSystemState & getSystemState( void ) const;
    inline NESystem::eSystemState & getSystemState( void );

//////////////////////////////////////////////////////////////////////////
// System Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Requested to power off.
     * \note    Has no response
     **/
    virtual void requestPowerOff( void ) = 0;

    /**
     * \brief   Request call.
     *          requested to power on
     * \note    Has no response
     **/
    virtual void requestPowerOn( void ) = 0;

    /**
     * \brief   Request call.
     *          requested to shutdown
     * \note    Has no response
     **/
    virtual void requestShutdown( void ) = 0;

    /**
     * \brief   Request call.
     *          requested to start.
     * \note    Has no response
     **/
    virtual void requestStart( void ) = 0;

//////////////////////////////////////////////////////////////////////////
// System Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// System Interface Broadcasts
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
     * \brief   Returns implemented version of service interface.
     **/
    virtual const Version & getImplVersion( void ) const;

    /**
     * \brief   Sends update notification message to all clients. 
     *          This method can be called manually to send update 
     *          notification message after updating attribute value.
     *
     * \param   msgId   The attribute message ID to notify clients.
     **/
    virtual void sendNotification( unsigned int msgId );

    /**
     * \brief   Sends error message to clients.
     *          If message ID is a request, it should send result NEService::RESULT_REQUEST_ERROR or NEService::RESULT_REQUEST_CANCELED, depending on msgCancel flag.
     *          If message ID is a response, it should send result NEService::RESULT_INVALID.
     *          If message ID is an attribute, it should send result NEService::RESULT_DATA_INVALID
     *          and invalidate attribute data value.
     *
     * \param   msgId       The message ID to send error message
     * \param   msgCancel   Indicates whether the request is canceled or should respond with error.
     *                      This parameter has sense only for request IDs.
     *                      It is ignored for response and attributes IDs.
     **/
    virtual void errorRequest( unsigned int msgId, bool msgCancel );

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
    virtual void startupServiceInterface( Component & holder );

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void shutdownServiceIntrface ( Component & holder );

/************************************************************************/
// StubBase overrides. Protected methods 
/************************************************************************/

    /**
     * \brief   Returns number of requests of Service Interface
     **/
    virtual unsigned int getNumberOfRequests(void) const;

    /**
     * \brief   Returns number of responses of Service Interface
     **/
    virtual unsigned int getNumberOfResponses( void ) const;

    /**
     * \brief   Returns number of attributes of Service Interface
     **/
    virtual unsigned int getNumberOfAttributes( void ) const;

    /**
     * \brief   Returns pointer of array of requests IDs of Service Interface
     **/
    virtual const unsigned int * getRequestIds(void) const;

    /**
     * \brief   Returns pointer of array of response IDs of Service Interface
     **/
    virtual const unsigned int * getResponseIds(void) const;

    /**
     * \brief   Returns pointer of array of attribute IDs of Service Interface
     **/
    virtual const unsigned int * getAttributeIds(void) const;

    /**
     * \brief   Creates Response event object to pass of client.
     * \param   proxy   The address of proxy object to send response event
     * \param   msgId   The message ID to send to client
     * \param   result  The result of message
     * \param   data    The buffer of data to send to client. Can be Invalid buffer.
     * \return  Returns valid pointer to Response event object
     **/
    virtual ResponseEvent * createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const;

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
    virtual void processRequestEvent( ServiceRequestEvent & eventElem );

    /**
     * \brief   Triggered to process attribute update notification event.
     *          Override method to process request to get attribute value and
     *          process notification request of attribute update.
     * \param   eventElem   Service Request Event object, contains attribute ID.
     **/
    virtual void processAttributeEvent( ServiceRequestEvent & eventElem );

//////////////////////////////////////////////////////////////////////////
// Hidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to SystemStub object
     **/
    inline SystemStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute SystemState
     *          The system state.
     **/
    NESystem::eSystemState      mSystemState;
    /**
     * \brief   Attribute SystemState data validation state.
     **/
    NEService::eDataStateType   mSystemStateState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SystemStub( void );
    SystemStub( const SystemStub & /* src */ );
    const SystemStub & operator = ( const SystemStub & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// SystemStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline SystemStub & SystemStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   SystemState attribute functions
 ************************************************************************/
 
inline bool SystemStub::isSystemStateValid( void ) const
{
    return (mSystemStateState  == NEService::DATA_OK);
}

inline void SystemStub::invalidateSystemState( void )
{
    errorRequest( static_cast<unsigned int>(NESystem::MSG_ID_SystemState), false );
}

inline void SystemStub::notifySystemStateUpdated( void )
{
    sendNotification( NESystem::MSG_ID_SystemState );
}

inline const NESystem::eSystemState & SystemStub::getSystemState( void ) const
{
    return mSystemState;
}

inline NESystem::eSystemState & SystemStub::getSystemState( void )
{
    return mSystemState;
}

//////////////////////////////////////////////////////////////////////////
// SystemStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SRC_GEN_SYSTEMSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/SystemStub.hpp file
//////////////////////////////////////////////////////////////////////////
